#include <assert.h>
#include <stdio.h>
#include <queue>
#include <stack>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <deque>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <inttypes.h>
#include "leveldb/db.h"

using namespace std;

#define FP_SIZE 6

uint64_t TH_K;
uint64_t INIT;
uint64_t QUEUE_LIMIT;
double LEAK_RATE;

struct node
{
	char key[FP_SIZE];
	uint64_t count;
};

struct cmp
{
	bool operator()(node a, node b) 
	{
		return a.count > b.count;
	}
};

leveldb::DB *origin;	// F_M
leveldb::DB *left_o;	// L_M
leveldb::DB *right_o;	// R_M

leveldb::DB *target;	// F_C
leveldb::DB *left_t;	// L_C
leveldb::DB *right_t;	// R_C

// unique db is used to record all inferred chunks
leveldb::DB *uniq;

priority_queue<node, vector<node>, cmp > pq;
priority_queue<node, vector<node>, cmp > pc;

// q_o and q_t implement inferred set G
queue<node> q_o;
queue<node> q_t;


uint64_t total = 0;
uint64_t common = 0;
uint64_t correct = 0;
uint64_t uniq_count = 0;
uint64_t involve = 0;
uint64_t leak = 0;
void stat_db()
{
        leveldb::Iterator* it = left_o->NewIterator(leveldb::ReadOptions());
        leveldb::Status status;
	printf("dbstat:\n");
        for (it->SeekToFirst(); it->Valid(); it->Next())
        {
	 	int len = 0;
        	for (len = 0; len < FP_SIZE; len++)
	        {
        	        printf("%02x", (unsigned char)(it->key()).ToString().c_str()[len]);
                	if (len < FP_SIZE - 1) printf(":");
        	}printf("\n");
	
		
        }
}

void init_db(std::string db_name, int type) 
{
	leveldb::DB *db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, db_name.c_str(), &db);
	assert(status.ok());
	assert(db != NULL);

	if(type == 1) origin = db;
	if(type == 11) left_o = db;
	if(type == 12) right_o = db;
	if(type == 2) target = db;
	if(type == 21) left_t = db;
	if(type == 22) right_t = db;
	if(type == 3) uniq = db;
}

void print_fp(node a)
{
	int len = 0;
	for (len = 0; len < FP_SIZE; len++)
	{
		printf("%02x", (unsigned char)a.key[len]);
		if (len < FP_SIZE - 1) printf(":");
	}
	printf("\t%lld\n",a.count);
}

void left_insert(int type, char* fp, uint64_t k)
{
	leveldb::Status status;
	leveldb::Slice key(fp, FP_SIZE);
	std::string existing_value;
	priority_queue<node, vector<node>, cmp >* pt;
	
	if(type == 0) pt = &pq;
	else pt = &pc;


	uint64_t len = 0;
	char tar[FP_SIZE];
	uint64_t tmp;

	if(type == 0) status = left_o->Get(leveldb::ReadOptions(), key, &existing_value);
	else 	status = left_t->Get(leveldb::ReadOptions(), key, &existing_value);

	if(status.ok())
	{
		while(len < existing_value.size())
		{
			memcpy(tar, existing_value.c_str()+len, FP_SIZE);

			const char* t_int = existing_value.c_str()+len+FP_SIZE;
			tmp = *(uint64_t*)t_int;
			node entry;
			memcpy(entry.key, tar, FP_SIZE);
			entry.count = tmp;
			if(pt->size()<k)
			{
				pt->push(entry);
			}else
			{
				node min = pt->top();
				if(tmp > min.count)
				{
					pt->pop();
					pt->push(entry);
				}
			}
			len += (FP_SIZE+sizeof(uint64_t));	
		}
	}else printf("this is not exist\n");
}

void main_loop(char * hash,int k)
{
	stack<node> tmp;
	stack<node> omp;
		// clear
	while(!pq.empty()) pq.pop();
	while(!omp.empty()) omp.pop();

	left_insert(0, hash, k);
	while (!pq.empty())
	{
		omp.push(pq.top());
		pq.pop();
	}
	while(!omp.empty())
	{
		print_fp(omp.top());
		omp.pop();
	}
}

int main (int argc, char *argv[])
{
	init_db(argv[2], 11);// refer to original L_db
	int k = 50;
	char key[FP_SIZE];
	memset(key, 0, FP_SIZE);
	int idx = 0 ;
	char *item;
	item = strtok(argv[1], ":\t\n ");
	while (item != NULL && idx < FP_SIZE)
        {
           	key[idx++] = strtol(item, NULL, 16);
                item = strtok(NULL, ":\t\n");
       	}	
	printf("%02x", (unsigned char)key[0]);
	printf("%02x", (unsigned char)key[1]);
	printf("%02x", (unsigned char)key[2]);
        printf("%02x", (unsigned char)key[3]);
	printf("%02x", (unsigned char)key[4]);
        printf("%02x\n", (unsigned char)key[5]);
//	stat_db();
	main_loop(key, k);
	return 0;
}
