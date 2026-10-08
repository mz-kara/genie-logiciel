#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>

#include <openssl/evp.h>
#include <openssl/ssl.h>

#include <curl/curl.h>

#include <errno.h>


int base64_decode(const char *input, unsigned char *output, int max_len) {
    return EVP_DecodeBlock(output, (const unsigned char *)input, strlen(input));
}

void extract_key(struct MemoryStruct* chunk, struct keys* keys){
    // key_id extraction
    char *p = strstr(chunk -> memory, "\"key_ID\"");
    if(p == NULL){
        perror("strstr");
        return;
    }
    p = p + 11;
    memcpy(keys -> id, p, 36);
    keys -> id[36] = '\0';

    // key extraction
    char *a = strstr(chunk -> memory, "\"key\"");
    if(a == NULL){
        perror("strstr");
        return;
    }
    a = a + 8;
    memcpy(keys -> key, a, 44);
    keys -> key[44] = '\0';
}
 
size_t write_cb(char *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

int create_client_socket(){
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    int s = getaddrinfo(NEXT_HOST, NEXT_PORT, &hints, &res);
    if(s != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return EXIT_FAILURE;
    }

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sock < 0){
        perror("Error in socket creation");
        return EXIT_FAILURE;
    }

    int ret = connect(sock, res->ai_addr, res->ai_addrlen);
    if(ret < 0){
        perror("Error in connection");
        close(sock);
        return EXIT_FAILURE;
    }

    freeaddrinfo(res);
    return sock;
}

int create_server_socket(){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("Error in socket creation");
        close(sock);
        return EXIT_FAILURE;
    }

    struct sockaddr_in relay_addr;
    relay_addr.sin_family = AF_INET;
    relay_addr.sin_port = htons(MY_PORT);
    relay_addr.sin_addr.s_addr = htonl(0);

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int ret = bind(sock, (struct sockaddr *) &relay_addr, sizeof(relay_addr));
    if(ret < 0){
        perror("Error in bind");
        close(sock);
        return EXIT_FAILURE;
    }

    ret = listen(sock, 1);
    if(ret < 0){
        perror("Error in listen");
        close(sock);
        return EXIT_FAILURE;
    }

    int src_sock = accept(sock, NULL, NULL);
    if(src_sock < 0){
        perror("accept");
        close(sock);
        return EXIT_FAILURE;
    }

    return src_sock;
}

void curl_enc_keys(char *ip_kms, struct keys* local_key){
    CURL *curl;
    curl = curl_easy_init();

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0; 
    
    char url[256];
    snprintf(url, sizeof(url), "https://%s/api/v1/keys/%s/enc_keys?number=2&size=%d", ip_kms, NEXT_SAE_ID, 256);
    
    curl_easy_setopt(curl, CURLOPT_CAINFO, "./kms_certs/ca_rsa.crt");  
    curl_easy_setopt(curl, CURLOPT_SSLCERT, "./kms_certs/client.crt");
    curl_easy_setopt(curl, CURLOPT_SSLKEY, "./kms_certs/client.key");

    curl_easy_setopt(curl, CURLOPT_URL , url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

    CURLcode result = curl_easy_perform(curl);

    extract_key(&chunk, local_key);

    free(chunk.memory);
    curl_easy_cleanup(curl);
}

void curl_dec_keys(char *ip_kms, struct keys* local_key, char *src_sae_id){
    CURL *curl;
    curl = curl_easy_init();

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0; 
    
    char url[256];
    snprintf(url, sizeof(url), "https://%s/api/v1/keys/%s/dec_keys?key_ID=%s", ip_kms, src_sae_id, local_key -> id);

    curl_easy_setopt(curl, CURLOPT_CAINFO, "./kms_certs/ca_rsa.crt");  
    curl_easy_setopt(curl, CURLOPT_SSLCERT, "./kms_certs/client.crt");
    curl_easy_setopt(curl, CURLOPT_SSLKEY, "./kms_certs/client.key");

    curl_easy_setopt(curl, CURLOPT_URL , url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

    CURLcode result = curl_easy_perform(curl);

    extract_key(&chunk, local_key);

    free(chunk.memory);
    curl_easy_cleanup(curl);
}

int send_data(SSL* ssl, struct message* msg){
    int ret = 0;
    int sent = 0;
    int to_sent = sizeof(*msg);
    while(sent != to_sent){
        ret = SSL_write(ssl, (char *)msg + sent, to_sent - sent);
        if(ret < 0){
            return -1;
        }
        sent += ret;
    }
    
    return ret;
}

int read_data(SSL* ssl, struct message* msg){
    int ret = 0;
    int received = 0;
    int to_receive = sizeof(*msg);
    while(received < to_receive){
        ret = SSL_read(ssl, (char *)msg + received, to_receive - received);
        if(ret <= 0){
            return -1;
        }
        received += ret;
    }

    return received;
}

void print_message(const struct message *msg) {
    printf("dest    : %s\n", msg->dest_final_sae_id);
    printf("src     : %s\n", msg->src_sae_id);
    printf("key_id  : %s\n", msg->key_id);
    printf("key_xor : ");
    for(int i=0; i < sizeof(msg->key_xor); i++){
        printf("%02x", msg->key_xor[i]);
    }
    printf("\n");
}

void close_ssl(SSL* ssl){
    int sock = SSL_get_fd(ssl);
    SSL_CTX * ctx = SSL_get_SSL_CTX(ssl);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
}

SSL* receive_ssl_message(struct message *msg, char *src_id) {
    int src_sock = create_server_socket();

    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    SSL_CTX_use_certificate_chain_file(ctx, "./sae_certs/relay.pem");
    SSL_CTX_use_PrivateKey_file(ctx, "./sae_certs/relay.key", SSL_FILETYPE_PEM);

    SSL* src_ssl = SSL_new(ctx);
    SSL_set_fd(src_ssl, src_sock);

    if(SSL_accept(src_ssl) != 1){
        ERR_print_errors_fp(stderr);
        close_ssl(src_ssl);
        return NULL;
    } else {
        // Affichage de l'attente avec une valeur générique car src_sae_id n'est pas encore connu
        printf("SAE %s : waiting for a message ... \n", MY_SAE_ID);
        printf("TLS connection established : %s\n", SSL_get_cipher(src_ssl));
    }

    memset(msg, 0, sizeof(*msg));
    if(read_data(src_ssl, msg) < 0){
        fprintf(stderr, "fail read data\n");
        close_ssl(src_ssl);
        return NULL;
    }
    
    memcpy(src_id, msg->src_sae_id, 16);
    print_message(msg);
    printf("\n");
    
    return src_ssl;
}

int relay_message(struct message *msg, unsigned char *K) {
    printf("SAE %s : I am a relay, sending to the next sae %s\n", MY_SAE_ID, NEXT_SAE_ID);
    
    struct keys quantum_key_dest;
    memset(&quantum_key_dest, 0, sizeof(quantum_key_dest));
    curl_enc_keys(MY_KMS_IP, &quantum_key_dest);

    unsigned char xor_encrypted_key[32] = {0};
    unsigned char quantum_key_bytes[33]; // 32 real + 1 padding byte
    base64_decode(quantum_key_dest.key, quantum_key_bytes, sizeof(quantum_key_bytes));

    /// encryption of K by quantum key
    for(int i=0; i < sizeof(xor_encrypted_key); i++){
        xor_encrypted_key[i] = K[i] ^ quantum_key_bytes[i];
    }

    /// Prepare the message to send to the next SAE
    memset((char *)msg + sizeof(msg->dest_final_sae_id), 0, sizeof(*msg) - sizeof(msg->dest_final_sae_id));
    strcpy(msg->src_sae_id, MY_SAE_ID);
    strcpy(msg->key_id, quantum_key_dest.id);
    memcpy(msg->key_xor, xor_encrypted_key, sizeof(xor_encrypted_key));

    int sock = create_client_socket();

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    SSL_CTX_load_verify_locations(ctx, "./sae_certs/rootca.pem", NULL);

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        close_ssl(ssl);
        return EXIT_FAILURE;
    } else {
        printf("SAE %s : sending the message to SAE %s ... \n", MY_SAE_ID, NEXT_SAE_ID);
        printf("TLS connection established : %s\n", SSL_get_cipher(ssl));
    }

    if(send_data(ssl, msg) < 0){
        fprintf(stderr, "fail send data\n");
        close_ssl(ssl);
        return EXIT_FAILURE;
    }
    
    print_message(msg);
    close_ssl(ssl);
    
    return EXIT_SUCCESS;
}