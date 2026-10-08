#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <curl/curl.h>

#include <errno.h>

#include <signal.h>

#include "config.h"

char src_sae_id[16];

int main() {
    /* Ignore broken pipe signals */
    signal(SIGPIPE, SIG_IGN);

    /// 1. Receiving message
    int src_sock = create_server_socket();

    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    SSL_CTX_use_certificate_chain_file(ctx, "./sae_certs/relay.pem");
    SSL_CTX_use_PrivateKey_file(ctx, "./sae_certs/relay.key", SSL_FILETYPE_PEM);

    SSL* src_ssl = SSL_new(ctx);
    SSL_set_fd(src_ssl, src_sock);

    if(SSL_accept(src_ssl) != 1){
        ERR_print_errors_fp(stderr);
        close_ssl(src_ssl);
        return EXIT_FAILURE;
    }else{
        printf("SAE %s : waiting for a message from SAE %s ... \n", MY_SAE_ID, src_sae_id);
        printf("TLS connection established : %s\n", SSL_get_cipher(src_ssl));
    }

    struct message msg;
    memset(&msg, 0, sizeof(msg));
    if(read_data(src_ssl, &msg) < 0){
        fprintf(stderr, "fail read data\n");
        close_ssl(src_ssl);
        return EXIT_FAILURE;

    };
    memcpy(src_sae_id, msg.src_sae_id, sizeof(src_sae_id));

    print_message(&msg);
    printf("\n");

    /// 2. Retreving local key
    struct keys src_local_key;
    memset(&src_local_key, 0, sizeof(src_local_key));
    strcpy(src_local_key.id, msg.key_id);

    curl_global_init(CURL_GLOBAL_ALL);
    curl_dec_keys(MY_KMS_IP, &src_local_key, src_sae_id);

    /// 3. Decrypt K
    unsigned char K[32];
    unsigned char src_local_key_key[33]; //32 vrais + 1 octet de padding
    base64_decode(src_local_key.key, src_local_key_key, sizeof(src_local_key_key));

    for(int i=0; i < 32; i++){
        K[i] = msg.key_xor[i] ^ src_local_key_key[i];
    }

    /// 4. Are we a relay? If so, send K to destination
    if(strcmp(msg.dest_final_sae_id, MY_SAE_ID) != 0){
        printf("SAE %s : I am a relay, sending to the next sae %s\n", MY_SAE_ID, NEXT_SAE_ID);
        struct keys dest_local_key;
        memset(&dest_local_key, 0, sizeof(dest_local_key));
        curl_enc_keys(MY_KMS_IP, &dest_local_key);

        unsigned char K_xor[32] = {0};
        unsigned char decode_dest_local_key[33]; //32 real + 1 padding byte
        base64_decode(dest_local_key.key, decode_dest_local_key, sizeof(decode_dest_local_key));

        for(int i=0; i < sizeof(K_xor); i++){
            K_xor[i] = K[i] ^ decode_dest_local_key[i];
        }

        memset((char *)&msg + sizeof(msg.dest_final_sae_id), 0, sizeof(msg) - sizeof(msg.dest_final_sae_id));
        strcpy(msg.src_sae_id, MY_SAE_ID);
        strcpy(msg.key_id, dest_local_key.id);
        memcpy(msg.key_xor, K_xor, sizeof(K_xor));

        int sock = create_client_socket();

        SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
        SSL_CTX_load_verify_locations(ctx, "./sae_certs/rootca.pem", NULL);

        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);

        if (SSL_connect(ssl) != 1) {
            ERR_print_errors_fp(stderr);
            close_ssl(ssl);
            return EXIT_FAILURE;
        }else{
            printf("SAE %s : sending the message to SAE %s ... \n", MY_SAE_ID, NEXT_SAE_ID);
            printf("TLS connection established : %s\n", SSL_get_cipher(ssl));
        }

        if(send_data(ssl, &msg) < 0){
            fprintf(stderr, "fail send data\n");
            close_ssl(ssl);
            return EXIT_FAILURE;
        }
        print_message(&msg);

    }else{
        printf("SAE %s : I am a slave, the secret key is ...\n", MY_SAE_ID);
        printf("SAE %s : K=", MY_SAE_ID);
        for(int i = 0; i < sizeof(K); i++) {
            printf("%02x", K[i]);
        }
        printf("\n");
    }

    /// close
    curl_global_cleanup();
    close_ssl(src_ssl);

    return 0;
}