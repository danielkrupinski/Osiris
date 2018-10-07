#pragma once

// 16 bytes == 128 bit digest
#define MD5_DIGEST_LENGTH 16  

// MD5 Hash
typedef struct
{
    unsigned int	buf[4];
    unsigned int	bits[2];
    unsigned char	in[64];
} MD5Context_t;

void MD5Init(MD5Context_t *context);
void MD5Update(MD5Context_t *context, unsigned char const *buf, unsigned int len);
void MD5Final(unsigned char digest[MD5_DIGEST_LENGTH], MD5Context_t *context);

char *MD5_Print(unsigned char *digest, int hashlen);

unsigned int MD5_PseudoRandom(unsigned int nSeed);