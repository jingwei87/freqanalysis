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
leveldb::DB *relate;
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
	if(type == 4) relate = db;
}

// enqueue leaked chunks based on leakage rate
void stat_db()
{
        leveldb::Iterator* it = target->NewIterator(leveldb::ReadOptions());
        leveldb::Status status;
        leveldb::Status cst;
        std::string existing_value;
        std::string exs;
	for (it->SeekToFirst(); it->Valid(); it->Next())
	{
		total ++;

                cst = relate->Get(leveldb::ReadOptions(), it->key(), &exs);
                leveldb::Slice ckey(exs.c_str(), FP_SIZE);
		status = origin->Get(leveldb::ReadOptions(), ckey, &existing_value);
		if(LEAK_RATE != 0 && rand()%10000 <= LEAK_RATE*10000)//original is INT_MAX as diviser
		{
			common ++;
			if (status.ok())
			{
				node entry_o;
				memcpy(entry_o.key, it->key().ToString().c_str(), FP_SIZE);
			//	entry_o.count = strtoimax(existing_value.data(), NULL, 10);
				entry_o.count = *(uint64_t *)existing_value.c_str();
				q_o.push(entry_o);

				node entry_t;
				memcpy(entry_t.key, it->key().ToString().c_str(), FP_SIZE);
			//	entry_t.count = strtoimax(it->value().ToString().c_str(), NULL, 10);
				entry_t.count = *(uint64_t *)it->value().ToString().c_str();
				leveldb::Status s;
				leveldb::Slice k(entry_t.key, FP_SIZE);
				char buf[32];
				memset(buf, 0, 32);
				sprintf(buf, "%lu", entry_t.count);
				leveldb::Slice u(buf, sizeof(uint64_t));
				s = uniq->Put(leveldb::WriteOptions(), k, u);
				q_t.push(entry_t);

				leak ++;		
			}			
		}
	}

	printf("unique chunk: %lu\nleak count: %lu\nenqueue count: %lu\n", total, common, leak);
}

void print_fp(node a)
{
	int len = 0;
	for (len = 0; len < FP_SIZE; len++)
	{
		printf("%02x", (unsigned char)a.key[len]);
		if (len < FP_SIZE - 1) printf(":");
	}
	printf("\t");
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
	}
}

void right_insert(int type, char* fp, uint64_t k)
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

	if(type == 0)status = right_o->Get(leveldb::ReadOptions(), key, &existing_value);
	else status = right_t->Get(leveldb::ReadOptions(), key, &existing_value);

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
	}

}

// insert top-k frequent chunks (in db) into pq
void db_insert(leveldb::DB* db, uint64_t k)
{
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for(it->SeekToFirst(); it->Valid(); it->Next())
	{
		//uint64_t val = strtoimax(it->value().ToString().c_str(), NULL, 10);
		uint64_t val = *(uint64_t *)it->value().ToString().c_str();
		node entry;
		memcpy(entry.key, it->key().ToString().c_str(), FP_SIZE);
		entry.count = val;
		if(pq.size()<k) 
		{
			pq.push(entry);
		}else
		{
			node min = pq.top();
			if(val > min.count)
			{
				pq.pop();
				pq.push(entry);
			}
		}
	}	

}

void main_loop()
{

	stack<node> tmp;
	stack<node> omp;

	if(LEAK_RATE == 0)
	{
		db_insert(origin, INIT);
		while (!pq.empty())//inverting chunk sequence (i.e., sort u-frequent chunks by frequency) 
		{
			tmp.push(pq.top());
			pq.pop();
		}
		while (!tmp.empty())//inserting into a queue
		{
			q_o.push(tmp.top());
			tmp.pop();
		}

		db_insert(target, INIT);
		while (!pq.empty())//inverting the sequence (i.e., sort u-frequent chunks by frequency) 
		{
			tmp.push(pq.top());
			pq.pop();
		}
		while (!tmp.empty())//count unique chunk 
		{
			leveldb::Status s;
			leveldb::Slice k(tmp.top().key, FP_SIZE);
			char buf[32];
			memset(buf,0,32);
			sprintf(buf, "%lu", tmp.top().count);
			leveldb::Slice u(buf, sizeof(uint64_t));
			s = uniq->Put(leveldb::WriteOptions(), k, u);

			// insert chunks into q_t
			q_t.push(tmp.top());
			tmp.pop();
		}
	}
	// MAIN LOOP
	while(!q_o.empty() && !q_t.empty())
	{
                leveldb::Status cst;
                leveldb::Slice key(q_t.front().key, FP_SIZE);
                std::string existing_value;
                cst = relate->Get(leveldb::ReadOptions(), key, &existing_value);
/*		if(!cst.ok())printf("CNM!!\n");
                if(cst.ok() && memcmp(q_o.front().key, existing_value.data(), FP_SIZE) == 0) correct++;
		for(int j = 0; j < FP_SIZE; j++)
                        printf(":%.2hhx", q_t.front().key[j]);printf("\t");
		for(int j = 0; j < FP_SIZE; j++)
                        printf(":%.2hhx", q_o.front().key[j]);printf("\n");
	*/	
		// clear
		while(!pq.empty()) pq.pop();
		while(!pc.empty()) pc.pop();
		while(!omp.empty()) omp.pop();
		while(!tmp.empty()) tmp.pop();

		left_insert(0, q_o.front().key, TH_K);
		left_insert(1, q_t.front().key, TH_K);

		// sort chunks by frequency
		while (!pq.empty() && !pc.empty())
		{
			omp.push(pq.top());
			tmp.push(pc.top());
			pq.pop();
			pc.pop();
		}

		while (!tmp.empty() && !omp.empty())
		{
			leveldb::Status ss;
			leveldb::Slice kk(tmp.top().key, FP_SIZE);
			std::string ex;
			ss = uniq->Get(leveldb::ReadOptions(), kk, &ex);

			// if  ciphertext chunk has not been inferred
			if(!ss.ok())
			{
				if(q_o.size()+1 > QUEUE_LIMIT) break;
				char buf[32];
				memset(buf,0,32);
				sprintf(buf, "%lu", tmp.top().count);
				leveldb::Slice uu(buf, sizeof(uint64_t));
				ss = uniq->Put(leveldb::WriteOptions(), kk, uu);
				q_o.push(omp.top());
				q_t.push(tmp.top());
			}
			omp.pop();
			tmp.pop();
		}

		while(!pq.empty()) pq.pop();
		while(!pc.empty()) pc.pop();
		while(!omp.empty()) omp.pop();
		while(!tmp.empty()) tmp.pop();

		right_insert(0, q_o.front().key, TH_K);
		right_insert(1, q_t.front().key, TH_K);

		while (!pq.empty() && !pc.empty())
		{
			omp.push(pq.top());
			tmp.push(pc.top());
			pq.pop();
			pc.pop();
		}
		while (!tmp.empty() && !omp.empty())
		{
			leveldb::Status ss;
			leveldb::Slice kk(tmp.top().key, FP_SIZE);
			std::string ex;
			ss = uniq->Get(leveldb::ReadOptions(), kk, &ex);

			if(!ss.ok())
			{
				if(q_o.size()+1 > QUEUE_LIMIT) break;
				char buf[32];
				memset(buf,0,32);
				sprintf(buf, "%lu", tmp.top().count);
				leveldb::Slice uu(buf, sizeof(uint64_t));
				ss = uniq->Put(leveldb::WriteOptions(), kk, uu);
				q_o.push(omp.top());
				q_t.push(tmp.top());
			}
			omp.pop();
			tmp.pop();
		}
		q_o.pop();
		q_t.pop();
		involve ++;
	}
	printf("correct %lu\ninvolve chunk %lu\n", correct, involve);
}

int main (int argc, char *argv[])
{
	init_db(argv[5], 1);
	init_db(argv[6], 11);
	init_db(argv[7], 12);
	init_db(argv[8], 2);
	init_db(argv[9], 21);
	init_db(argv[10], 22);

	init_db("./uniq-db/", 3);
	init_db("./relate_db/", 4);	
	INIT = atoi(argv[1]);	// u
	TH_K = atoi(argv[2]);	// v
	QUEUE_LIMIT = atoi(argv[3]);	// w

	LEAK_RATE = atof(argv[4]);

	stat_db();
	main_loop();

	return 0;
}
