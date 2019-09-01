#pragma once

constexpr auto MD5_DIGEST_LENGTH = 16;

// MD5 Hash
typedef struct
{
    unsigned int	buf[4];
    unsigned int	bits[2];
    unsigned char	in[64];
} MD5Context_t;

namespace MD5 {
    unsigned int PseudoRandom(unsigned int nSeed) noexcept;
}