#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <transform.h>

#include <time.h>
#include "pk.h"
#include "sha256.h"
#include "sha1.h"
#include "aes.h"


void mbedtls_aes_ecb_test() {
    int i;
    mbedtls_aes_context aes_ctx;

    unsigned char key[16] = "ECBPASSWD1234";
    unsigned char plain[16] = "MbedTLS_AES";
    unsigned char decrypted_plain[16] = {0};
    unsigned char cipher[16] = {0};

    mbedtls_aes_init(&aes_ctx);

    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);
    printf("\nAES ECB:\nBefore encrypt: %s\n", plain);
    mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, plain, cipher);

    printf("After encrypt: ");
    for (i = 0; i < 16; ++i) {
        printf("%c", cipher[i]);
    }
    printf(" -End\n");

    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);
    mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_DECRYPT, cipher, decrypted_plain);
    printf("After decrypt: %s\n", decrypted_plain);
    mbedtls_aes_free(&aes_ctx);
};

void mbedtls_aes_cbc_test() {
    int i;
    mbedtls_aes_context aes_ctx;

    unsigned char key[16] = "CBCPASSWD4321";
    unsigned char iv[16];
    unsigned char plain[64] = "Port Mbed_TLS to XiUOS success.";
    unsigned char decrypted_plain[64] = {0};
    unsigned char cipher[64] = {0};

    mbedtls_aes_init(&aes_ctx);

    printf("AES CBC:\nBefore encrypt: %s\n", plain);
    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);
    for (i = 0; i < 16; ++i) {
        iv[i] = 0x01;
    }
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, 64, iv, plain, cipher);
    printf("After encrypt: ");
    for (i = 0; i < 64; ++i) {
        printf("%c", cipher[i]);
    }
    printf(" -End\n");

    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);
    for (i = 0; i < 16; ++i) {
        iv[i] = 0x01;
    }
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, 64, iv, cipher, decrypted_plain);
    printf("After decrypt: %s\n", decrypted_plain);
    mbedtls_aes_free(&aes_ctx);
};


const char publickey[] = {
    "-----BEGIN PUBLIC KEY----- \r\n"
    "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE6NjtyqaRPShUnTn3OrM9CNnIxKHf3yWv4iFR/LPCcCTfnzGvIb3n/9REss3wjbeBNpZBFStPsYbY+iPWXA3ASw== \r\n"
    "-----END PUBLIC KEY-----"
};

void mbedtls_init() {
    mbedtls_pk_context ctx_pk;
    mbedtls_pk_init(&ctx_pk);

    int ret = mbedtls_pk_parse_public_key(&ctx_pk, (uint8_t *)publickey, sizeof(publickey));
    if (ret != 0) {
        printf("Can't import public key\n");
    } else {
        printf("Imported public key:\n %s\n", publickey);
    }
}

int test() {
    printf("test mbedtls ......\n");
    mbedtls_init();
    
    int i;
    unsigned char content_to_encrypt[] = "Hello Mbed-TLS";
    unsigned char content_encrypted[32];

    mbedtls_sha1_context sha1_ctx;
    mbedtls_sha1_init(&sha1_ctx);
    mbedtls_sha1_starts(&sha1_ctx);
    mbedtls_sha1_update(&sha1_ctx, content_to_encrypt, strlen((char *)content_to_encrypt));
    mbedtls_sha1_finish(&sha1_ctx, content_encrypted);
    printf("\nOriginal Data: %s \nAfter encrypt by sha1:", content_to_encrypt);
    mbedtls_sha1_free(&sha1_ctx);
    for (i = 0; i < 20; ++i) {
        printf("%02x", content_encrypted[i]);
    }
    printf("\n");

    mbedtls_sha256_context sha256_ctx;
    mbedtls_sha256_init(&sha256_ctx);
    mbedtls_sha256_starts(&sha256_ctx, 0);
    mbedtls_sha256_update(&sha256_ctx, content_to_encrypt, strlen((char *)content_to_encrypt));
    mbedtls_sha256_finish(&sha256_ctx, content_encrypted);
    printf("\nOriginal Data: %s \nAfter encrypt by sha256:", content_to_encrypt);
    mbedtls_sha256_free(&sha256_ctx);
    for (i = 0; i < 32; ++i) {
        printf("%02x", content_encrypted[i]);
    }
    printf("\n");

    mbedtls_aes_ecb_test();
    mbedtls_aes_cbc_test();
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                                                test_mbedtls, test, test_mbedtls);