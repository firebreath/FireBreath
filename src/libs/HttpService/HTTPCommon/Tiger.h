/* 
 * Tiger.h
 *
 * Tiger cryptographic hash function (3 pass, 192 bit)
 *
 */

#pragma once

#ifdef __APPLE__ // Mac byteswap defs
#include <architecture/byte_order.h>
#include <stdint.h>
#include <stddef.h>
#elif defined(_WIN32) // Windows, which we can safely assume is x86
#include "../Platform/windows_defs.h"
#define __LITTLE_ENDIAN__ 1
#else // Linux
#include <endian.h>
#if __BYTE_ORDER == __BIG_ENDIAN
#define __BIG_ENDIAN__ 1
#else
#define __LITTLE_ENDIAN__ 1
#endif
#endif

#include "APITypes.h"
#include <string.h> // bzero and memcpy
#include <string>
#include <stdio.h>

#define ssize_t SSIZE_T
#define bzero(ptr, size) memset(ptr, 0, size);

#if ((__WORDSIZE == 64) || defined(__amd64__) || defined(__x86_64__) || defined(__alpha) || defined(__ia64__))
/* This is the official definition of round */
#define __tiger_round(a,b,c,x,mul) \
  c ^= x; \
  a -= table[((c)>>(0*8))&0xFF] ^ (table+256)[((c)>>(2*8))&0xFF] ^ \
  (table+512)[((c)>>(4*8))&0xFF] ^ (table+768)[((c)>>(6*8))&0xFF] ; \
  b += (table+768)[((c)>>(1*8))&0xFF] ^ (table+512)[((c)>>(3*8))&0xFF] ^ \
  (table+256)[((c)>>(5*8))&0xFF] ^ table[((c)>>(7*8))&0xFF] ; \
  b *= mul;
#else
/* This code works faster when compiled on 32-bit machines */
#define __tiger_round(a,b,c,x,mul) \
  c ^= x; \
  a -= table[(uint8_t)(c)] ^ \
  (table+256)[(uint8_t)(((uint32_t)(c))>>(2*8))] ^ \
  (table+512)[(uint8_t)((c)>>(4*8))] ^ \
  (table+768)[(uint8_t)(((uint32_t)((c)>>(4*8)))>>(2*8))] ; \
  b += (table+768)[(uint8_t)(((uint32_t)(c))>>(1*8))] ^ \
  (table+512)[(uint8_t)(((uint32_t)(c))>>(3*8))] ^ \
  (table+256)[(uint8_t)(((uint32_t)((c)>>(4*8)))>>(1*8))] ^ \
  table[(uint8_t)(((uint32_t)((c)>>(4*8)))>>(3*8))]; \
  b *= mul;
#endif

class Tiger {
public:
  uint64_t hash[3];
  
  uint64_t total_length;
  
  operator const uint64_t*() const {
    return hash;
  }
  
  const char* hash_data() const {
    return reinterpret_cast<const char*>(hash);
  }
  const size_t hash_size() const {
    return 24; //192bit
  }
    
  std::string toString() const {
    char buf[64];
    for (size_t i = 0; i < 24; ++i) snprintf(buf + (i*2), 3, "%.2x", reinterpret_cast<const char*>(hash)[i^7] & 0xff);
    return std::string(buf);
  }
  
  bool operator==(const Tiger& right) const {
    return (memcmp(hash, right.hash, 24) == 0);
  }
  
  bool operator!=(const Tiger& right) const {
    return (memcmp(hash, right.hash, 24) != 0);
  }
  
  bool operator<(const Tiger& right) const {
    return (memcmp(hash, right.hash, 24) < 0);
  }
  
  Tiger() {
    reset();
  }
  
  Tiger(const void* data, size_t len) {
    reset();
    process_bytes(data, len);
    finalize();
  }
  
  Tiger(const Tiger& right) {
    memcpy(this, &right, sizeof(Tiger));
  }
  
  Tiger& operator=(const Tiger& right) {
    memcpy(this, &right, sizeof(Tiger));
    return *this;
  }
  
  void reset() {
    total_length = 0;
    
    hash[0]=0x0123456789ABCDEFLL;
    hash[1]=0xFEDCBA9876543210LL;
    hash[2]=0xF096A5B4C3B2E187LL;
  }
  
  uint64_t processed_size() {
    return total_length;
  }
  
  void process_bytes(const void* str, size_t length) {
    if (! length) return;
    uint64_t j = (total_length & 63LL);
    const uint8_t* strptr = static_cast<const uint8_t*>(str);
    if (j) {
      for (; length && j < 64; ++j) {
#if __BIG_ENDIAN__
        temp[j^7] = *(strptr++);
#else
        temp[j] = *(strptr++);
#endif
        --length;
        ++total_length;
      }
      if (j == 64) {
        tiger_compress(reinterpret_cast<const uint64_t*>(temp));
      }
      if (! length) return;
    }
    while (length >= 64) {
      tiger_compress(reinterpret_cast<const uint64_t*>(strptr));
      strptr += 64;
      length -= 64;
      total_length += 64;
    }
    if (length) {
      for (j = 0; j < length; ++j) {    
#if __BIG_ENDIAN__
        temp[j^7] = *(strptr++);
#else    
        temp[j] = *(strptr++);
#endif
      }
      total_length += length;
    }
  }
  
  void finalize() {
    uint64_t j = (total_length & 63LL);
#if __BIG_ENDIAN__
    temp[(j++)^7] = 0x01;
    for (; j&7; ++j) temp[j^7] = 0;
#else
    temp[j++] = 0x01;
    for (; j&7; ++j) temp[j] = 0;
#endif
    if (j > 56) {
      for(; j < 64; ++j) temp[j] = 0;
      tiger_compress(reinterpret_cast<const uint64_t*>(temp));
      j = 0;
    }
    bzero(temp + j, 64 - (size_t)j);
    uint64_t adj_len = total_length << 3;
    memcpy(temp + 56, &adj_len, 8);
    tiger_compress(reinterpret_cast<const uint64_t*>(temp));
  } 
  
protected:
  
  void tiger_compress(const uint64_t* str) {
    register uint64_t a, b, c, tmpa;
    uint64_t aa, bb, cc;
    register uint64_t x0, x1, x2, x3, x4, x5, x6, x7;
    int pass_no;
    
    a = hash[0];
    b = hash[1];
    c = hash[2];
    
    x0=str[0]; x1=str[1]; x2=str[2]; x3=str[3];
    x4=str[4]; x5=str[5]; x6=str[6]; x7=str[7];
    
    aa = a;
    bb = b;
    cc = c;
    
    for(pass_no=0; pass_no<3; pass_no++) {
      if(pass_no != 0) {
        x0 -= x7 ^ 0xA5A5A5A5A5A5A5A5LL;
        x1 ^= x0;
        x2 += x1;
        x3 -= x2 ^ ((~x1)<<19);
        x4 ^= x3;
        x5 += x4;
        x6 -= x5 ^ ((~x4)>>23);
        x7 ^= x6;
        x0 += x7;
        x1 -= x0 ^ ((~x7)<<19);
        x2 ^= x1;
        x3 += x2;
        x4 -= x3 ^ ((~x2)>>23);
        x5 ^= x4;
        x6 += x5;
        x7 -= x6 ^ 0x0123456789ABCDEFLL;
      }
      
      uint64_t mul = (pass_no==0?5:pass_no==1?7:9);
      
      __tiger_round(a,b,c,x0,mul)
      __tiger_round(b,c,a,x1,mul)
      __tiger_round(c,a,b,x2,mul)
      __tiger_round(a,b,c,x3,mul)
      __tiger_round(b,c,a,x4,mul)
      __tiger_round(c,a,b,x5,mul)
      __tiger_round(a,b,c,x6,mul)
      __tiger_round(b,c,a,x7,mul)
      
      tmpa=a; a=c; c=b; b=tmpa;
    }
    
    a ^= aa;
    b -= bb;
    c += cc;
    
    hash[0] = a;
    hash[1] = b;
    hash[2] = c;
  }
protected:
  uint8_t temp[64];
  static const uint64_t table[1024];
};

