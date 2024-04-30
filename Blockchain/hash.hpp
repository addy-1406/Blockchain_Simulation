//author: tko
#ifndef _HASH_H_
#define _HASH_H_
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstring>

using namespace std;


constexpr unsigned int ROTRIGHT(unsigned int value, unsigned int shift) {
    return (value >> shift) | (value << (32 - shift));
}

constexpr unsigned int CH(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ ((~x) & z);
}

constexpr unsigned int MAJ(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

constexpr unsigned int EP0(unsigned int x) {
    return ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22);
}

constexpr unsigned int EP1(unsigned int x) {
    return ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25);
}

constexpr unsigned int SIG0(unsigned int x) {
    return ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ (x >> 3);
}

constexpr unsigned int SIG1(unsigned int x) {
    return ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ (x >> 10);
}

std::string sha256(const std::string& str) {
    unsigned int hash[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    unsigned int k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    const uint64_t ml = str.size() * 8;
    unsigned char* msg = new unsigned char[str.size() + 64];
    memcpy(msg, str.c_str(), str.size());
    msg[str.size()] = 0x80;

    for (size_t i = str.size() + 1; i % 64 != 56; ++i)
        msg[i] = 0;

    for (size_t i = 0; i < 8; ++i)
        msg[str.size() + 56 + i] = (ml >> ((7 - i) * 8)) & 0xff;

    for (size_t chunk = 0; chunk < (str.size() + 64) / 64; ++chunk) {
        unsigned int w[64];
        for (size_t i = 0; i < 16; ++i)
            w[i] = (msg[chunk * 64 + i * 4] << 24) | (msg[chunk * 64 + i * 4 + 1] << 16) |
                (msg[chunk * 64 + i * 4 + 2] << 8) | msg[chunk * 64 + i * 4 + 3];

        for (size_t i = 16; i < 64; ++i)
            w[i] = SIG1(w[i - 2]) + w[i - 7] + SIG0(w[i - 15]) + w[i - 16];

        unsigned int a = hash[0];
        unsigned int b = hash[1];
        unsigned int c = hash[2];
        unsigned int d = hash[3];
        unsigned int e = hash[4];
        unsigned int f = hash[5];
        unsigned int g = hash[6];
        unsigned int h = hash[7];

        for (size_t i = 0; i < 64; ++i) {
            unsigned int t1 = h + EP1(e) + CH(e, f, g) + k[i] + w[i];
            unsigned int t2 = EP0(a) + MAJ(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    std::stringstream ss;
    for (int i = 0; i < 8; ++i) {
        ss << std::hex << std::setw(8) << std::setfill('0') << hash[i];
    }

    delete[] msg;
    return ss.str();
}

#endif