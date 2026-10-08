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

    struct message msg;
    SSL* src_ssl = receive_ssl_message(&msg, src_sae_id);
    memset(&msg, 0, sizeof(msg));
    if (src_ssl == NULL) {
        return EXIT_FAILURE;
    }
    memcpy(src_sae_id, msg.src_sae_id, sizeof(src_sae_id));

    print_message(&msg);
    printf("\n");

    /// 2. Retreving local quantum key
    struct keys quantum_key = {0};
    memset(&quantum_key, 0, sizeof(quantum_key));
    strcpy(quantum_key.id, msg.key_id);

    curl_global_init(CURL_GLOBAL_ALL);
    curl_dec_keys(MY_KMS_IP, &quantum_key, src_sae_id);

    /// 3. Decrypt K = quantum_key XOR key received by https
    unsigned char K[32];
    unsigned char quantum_key_bytes[33]; //32 vrais + 1 octet de padding
    base64_decode(quantum_key.key, quantum_key_bytes, sizeof(quantum_key_bytes));

    for(int i=0; i < 32; i++){
        K[i] = msg.key_xor[i] ^ quantum_key_bytes[i];
    }

    /// 4. Are we a relay? If so, send K to destination
    if(strcmp(msg.dest_final_sae_id, MY_SAE_ID) != 0){ ///Relay
        if (relay_message(&msg, K) != EXIT_SUCCESS) {
            curl_global_cleanup();
            close_ssl(src_ssl);
            return EXIT_FAILURE;
        }
    }else{ /// Slave SAE
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