/*
 *  Copyright (c) 2012-2017, Jyri J. Virkki
 *  All rights reserved.
 *
 *  This file is under BSD license. See LICENSE file.
 */

/*
 * Refer to bloom.h for documentation on the public interfaces.
 */

#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include "bloom.h"
#include "murmurhash2.h"
#include <iostream>
#define MAKESTRING(n) STRING(n)
#define STRING(n) #n

using namespace std;

inline static int test_bit_set_bit(unsigned char * buf,
                                   unsigned int x, int set_bit)
{
  unsigned int byte = x >> 3;
  unsigned char c = buf[byte];        // expensive memory access
  unsigned int mask = 1 << (x % 8);

  if (c & mask) {
    return 1;
  } else {
    if (set_bit) {
      buf[byte] = c | mask;
    }
    return 0;
  }
}


static int bloom_check_add(struct bloom * bloom,
                           const void * buffer, int len, int add)
{
  if (bloom->ready == 0) {
    printf("bloom at %p not initialized!\n", (void *)bloom);
    return -1;
  }

  int hits = 0;
  register unsigned int a = murmurhash2(buffer, len, 0x9747b28c);
  register unsigned int b = murmurhash2(buffer, len, a);
  register unsigned int x;
  register unsigned int i;

  for (i = 0; i < bloom->hashes; i++) {
    x = (a + i*b) % bloom->bits;
    if (test_bit_set_bit(bloom->bf, x, add)) {
      hits++;
    }
  }

  if (hits == bloom->hashes) {
    return 1;                // 1 == element already in (or collision)
  }

  return 0;
}


int bloom_init_size(struct bloom * bloom, int entries, double error,
                    unsigned int cache_size)
{
  return bloom_init(bloom, entries, error);
}


int bloom_init(struct bloom * bloom, int entries, double error)
{
  bloom->ready = 0;

  if (entries < 1000 || error == 0) {
    return 1;
  }

  bloom->entries = entries;
  bloom->error = error;

  double num = log(bloom->error);
  double denom = 0.480453013918201; // ln(2)^2
  bloom->bpe = -(num / denom);

  double dentries = (double)entries;
  bloom->bits = (int)(dentries * bloom->bpe);

  if (bloom->bits % 8) {
    bloom->bytes = (bloom->bits / 8) + 1;
  } else {
    bloom->bytes = bloom->bits / 8;
  }

  bloom->hashes = (int)ceil(0.693147180559945 * bloom->bpe);  // ln(2)

  bloom->bf = (unsigned char *)calloc(bloom->bytes, sizeof(unsigned char));
  if (bloom->bf == NULL) {
    return 1;
  }

  bloom->ready = 1;
  return 0;
}


int bloom_check(struct bloom * bloom, const void * buffer, int len)
{
  return bloom_check_add(bloom, buffer, len, 0);
}


int bloom_add(struct bloom * bloom, const void * buffer, int len)
{
  return bloom_check_add(bloom, buffer, len, 1);
}


void bloom_print(struct bloom * bloom)
{
  printf("bloom at %p\n", (void *)bloom);
  printf(" ->entries = %d\n", bloom->entries);
  printf(" ->error = %f\n", bloom->error);
  printf(" ->bits = %d\n", bloom->bits);
  printf(" ->bits per elem = %f\n", bloom->bpe);
  printf(" ->bytes = %d\n", bloom->bytes);
  printf(" ->hash functions = %d\n", bloom->hashes);
}


void bloom_free(struct bloom * bloom)
{
  if (bloom->ready) {
    free(bloom->bf);
  }
  bloom->ready = 0;
}


const char * bloom_version()
{
  return MAKESTRING(BLOOM_VERSION);
}
// load old data
bool bloom_init_conf(struct bloom * bloom) {

	ifstream in("./conf/bloomConf", std::ios::in | std::ios::binary);
	string contents;
	string data;
	uint64_t cnt = 0;
	if (in) {
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		uint64_t size_tmp = bloom->bytes;
		for (uint64_t i = 0; i < size_tmp; i++) {
			int sum = 0;
			for(int j = 0; j < 8; j++){
				if(contents[(i*8)+j] == '1'){
					cnt++;
					sum  += pow(2,(7-j));
				}
			}
			char tmp0 = (char)sum;
			memcpy(bloom->bf + i,&tmp0,sizeof(tmp0));
		}
		// cout<<"bloom data read number: "<<cnt<<endl;
		in.close();
		return true;
	}
	in.close();
	return false;
}

//output data 
bool bloom_conf_out(struct bloom * bloom) {

	fstream BLM;
	BLM.open("./conf/bloomConf", ios::out | ios::binary);
	if (!BLM.is_open()) {

		// cout<<"creat file faile"<<endl;
		return false;
	}
	int cnt = 0;
	for (int i = 0; i < bloom->bytes; i++) {
        for (int j = 7; j >= 0; j--) {					
			if ((bloom->bf[i] & 1<<j) != 0) {			
				BLM<<"1";
				cnt++;
			}
			else {			
				BLM<<"0";
			}				
		}
    }
	// cout<<endl<<"bloom data write number: "<<cnt<<endl;
	BLM.close();
	return true;
}
