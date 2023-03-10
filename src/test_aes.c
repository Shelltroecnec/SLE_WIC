#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Enable ECB, CTR and CBC mode. Note this can be done before including aes.h or at compile-time.
// E.g. with GCC by using the -D flag: gcc -c aes.c -DCBC=0 -DCTR=1 -DECB=1
#define CBC 1
#define CTR 1
#define ECB 1

#include "aes.h"

#if defined(AES256)
    uint8_t key[] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
#elif defined(AES192)
    uint8_t key[] = { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
#elif defined(AES128)
    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
#endif
uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };


static void phex(uint8_t* str);
static int test_encrypt_cbc(char *, int);
static int test_decrypt_cbc(char *, int);


int main(void)
{
    int exit;

#if defined(AES256)
    ILOG("Testing AES256\n");
#elif defined(AES192)
    ILOG("Testing AES192\n");
#elif defined(AES128)
    ILOG("Testing AES128\n");
#else
    ILOG("You need to specify a symbol between AES128, AES192 or AES256. Exiting");
    return 0;
#endif
    char in[] = "This is the text to be encrypted by aes 128 bit ";
    test_encrypt_cbc(in, sizeof(in));
    test_decrypt_cbc(in, sizeof(in));// +
	// test_encrypt_ctr() + test_decrypt_ctr() +
	// test_decrypt_ecb() + test_encrypt_ecb();
    //();

    return exit;
}


// prints string as hex
static void phex(uint8_t* str)
{

#if defined(AES256)
    uint8_t len = 32;
#elif defined(AES192)
    uint8_t len = 24;
#elif defined(AES128)
    uint8_t len = 16;
#endif

    unsigned char i;
    for (i = 0; i < len; ++i)
        ILOG("%c", str[i]);
    ILOG("");
}


static int test_decrypt_cbc(char *in, int len)
{
    int i =0;

    struct AES_ctx ctx;
    ILOG("Input:\n");
    for (i = (uint8_t) 0; i < (uint8_t) len/16; ++i)
    {
        phex(in + i * (uint8_t) 16);
    }
    ILOG("");

    ILOG("key:\n");
    phex(key);
    ILOG("");

    // print the resulting cipher as 4 x 16 byte strings
    ILOG("Decrypted:\n");


    
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, in, len);

    for (i = (uint8_t) 0; i < (uint8_t) len/16; ++i)
    {
        phex(in + i * (uint8_t) 16);
    }
}

static int test_encrypt_cbc(char *in, int len)
{
    int i =0;
    struct AES_ctx ctx;


    ILOG("Input to enchiper:\n");
    for (i = (uint8_t) 0; i < (uint8_t) len/16; ++i)
    {
        phex(in + i * (uint8_t) 16);
    }
    ILOG("");

    ILOG("key:\n");
    phex(key);
    ILOG("");

    // print the resulting cipher as 4 x 16 byte strings
    ILOG("Encrypted:\n");

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, in, 64);

    
    for (i = (uint8_t) 0; i < (uint8_t) len/16; ++i)
    {
        phex(in + i * (uint8_t) 16);
    }

}


