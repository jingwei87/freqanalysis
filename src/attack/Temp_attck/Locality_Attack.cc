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
//#define TH_K 20
//#define LEAK_RATE 0.0
//#define QUEUE_LIMIT 50000

uint64_t TH_K;
uint64_t INIT;
uint64_t QUEUE_LIMIT;
double LEAK_RATE;

struct node{
	//leveldb::Slice key;
	char key[FP_SIZE];
	uint64_t count;
	//node(leveldb::Slice x=NULL, uint64_t y=0):key(x), count(y){}
};

struct cmp{
	bool operator()(node a, node b){
		return a.count > b.count;
		//return memcmp(a.key, b.key, FP_SIZE);
	}
};

leveldb::DB *origin;
leveldb::DB *left_o;
leveldb::DB *right_o;
leveldb::DB *target;
leveldb::DB *left_t;
leveldb::DB *right_t;
leveldb::DB *uniq;


priority_queue<node, vector<node>, cmp > pq;
priority_queue<node, vector<node>, cmp > pc;
queue<node> q_o;
queue<node> q_t;


uint64_t total = 0;
uint64_t common = 0;
uint64_t correct = 0;
uint64_t uniq_count = 0;
uint64_t involve = 0;
uint64_t leak = 0;

void init_db(std::string db_name, int type) {
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

void stat_db(){
	leveldb::Iterator* it = target->NewIterator(leveldb::ReadOptions());
	leveldb::Status status;
	std::string existing_value;

	for (it->SeekToFirst(); it->Valid(); it->Next()){
		total ++;
		status = origin->Get(leveldb::ReadOptions(), it->key(), &existing_value);
		if(LEAK_RATE != 0 && rand()%10000 <= LEAK_RATE*10000){
			common ++;
			if (status.ok()){
				node entry_o;
				memcpy(entry_o.key, it->key().ToString().c_str(), FP_SIZE);
				entry_o.count = strtoimax(existing_value.data(), NULL, 10);
				q_o.push(entry_o);


				node entry_t;
				memcpy(entry_t.key, it->key().ToString().c_str(), FP_SIZE);
				entry_t.count = strtoimax(it->value().ToString().c_str(), NULL, 10);
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

void stat_uniq(){
	leveldb::Iterator* it = uniq->NewIterator(leveldb::ReadOptions());
	leveldb::Status status;
	std::string existing_value;
	for (it->SeekToFirst(); it->Valid(); it->Next()){
		uniq_count ++;
	}

	printf("output count: %lu\n", uniq_count);
}

void print_fp(node a){
	int len = 0;
	for (len = 0; len < FP_SIZE; len++){
		printf("%02x", (unsigned char)a.key[len]);
		if (len < FP_SIZE - 1) printf(":");
	}
	printf("\t");
}

void left_insert(int type, char* fp, uint64_t k){
	leveldb::Status status;
	leveldb::Slice key(fp, FP_SIZE);
	std::string existing_value;
	priority_queue<node, vector<node>, cmp >* pt;

	if(type == 0) pt = &pq;
	else pt = &pc;


	uint64_t len = 0;
	char tar[FP_SIZE];
	uint64_t tmp;

	if(type == 0)
		status = left_o->Get(leveldb::ReadOptions(), key, &existing_value);
	else 
		status = left_t->Get(leveldb::ReadOptions(), key, &existing_value);

	if(status.ok()){
		//printf("left %d\n", existing_value.size());
		while(len < existing_value.size()){
			memcpy(tar, existing_value.c_str()+len, FP_SIZE);

			const char* t_int = existing_value.c_str()+len+FP_SIZE;
			tmp = *(int*)t_int;
			//TODO
			node entry;
			memcpy(entry.key, tar, FP_SIZE);
			entry.count = tmp;

			if(pt->size()<k){
				pt->push(entry);
			}else{
				node min = pt->top();
				if(tmp > min.count){
					pt->pop();
					pt->push(entry);
				}
			}


			len += (FP_SIZE+sizeof(int));	
		}
	}
}

void right_insert(int type, char* fp, uint64_t k){
	leveldb::Status status;
	leveldb::Slice key(fp, FP_SIZE);
	std::string existing_value;
	priority_queue<node, vector<node>, cmp >* pt;

	if(type == 0) pt = &pq;
	else pt = &pc;


	uint64_t len = 0;
	char tar[FP_SIZE];
	uint64_t tmp;

	if(type == 0)
		status = right_o->Get(leveldb::ReadOptions(), key, &existing_value);
	else 
		status = right_t->Get(leveldb::ReadOptions(), key, &existing_value);

	if(status.ok()){
		//printf("right %d\n", existing_value.size());
		while(len < existing_value.size()){
			memcpy(tar, existing_value.c_str()+len, FP_SIZE);
			const char* t_int = existing_value.c_str()+len+FP_SIZE;
			tmp = *(int*)t_int;
			//TODO
			node entry;
			memcpy(entry.key, tar, FP_SIZE);
			entry.count = tmp;

			if(pt->size()<k){
				pt->push(entry);
			}else{
				node min = pt->top();
				if(tmp > min.count){
					pt->pop();
					pt->push(entry);
				}
			}


			len += (FP_SIZE+sizeof(int));	
		}
	}

}

void db_insert(leveldb::DB* db, uint64_t k){
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for(it->SeekToFirst(); it->Valid(); it->Next()){
		uint64_t val = strtoimax(it->value().ToString().c_str(), NULL, 10);
		node entry;
		memcpy(entry.key, it->key().ToString().c_str(), FP_SIZE);
		entry.count = val;
		if(pq.size()<k) {
			pq.push(entry);
		}else{
			node min = pq.top();
			if(val > min.count){
				pq.pop();
				pq.push(entry);
			}
		}
	}	


}

void main_loop(){

	stack<node> tmp;
	stack<node> omp;

	//	if(LEAK_RATE == 0){
	db_insert(origin, INIT);
	while (!pq.empty()){
		tmp.push(pq.top());
		pq.pop();
	}
	while (!tmp.empty()){

		q_o.push(tmp.top());
		tmp.pop();
	}

	db_insert(target, INIT);
	while (!pq.empty()){
		tmp.push(pq.top());
		pq.pop();
	}
	while (!tmp.empty()){
		leveldb::Status s;
		leveldb::Slice k(tmp.top().key, FP_SIZE);
		char buf[32];
		memset(buf,0,32);
		sprintf(buf, "%lu", tmp.top().count);
		leveldb::Slice u(buf, sizeof(uint64_t));
		s = uniq->Put(leveldb::WriteOptions(), k, u);
		q_t.push(tmp.top());
		tmp.pop();
	}
	//	}

	//	printf("queue size %d\n",q_t.size());
	// MAIN LOOP
	while(!q_o.empty() && !q_t.empty()){
		if(memcmp(q_o.front().key, q_t.front().key, FP_SIZE) == 0) correct++;
		//		printf("trav %lu queu %d\n", involve, q_o.size());	
		while(!pq.empty()) pq.pop();
		while(!pc.empty()) pc.pop();
		while(!omp.empty()) omp.pop();
		while(!tmp.empty()) tmp.pop();
		left_insert(0, q_o.front().key, TH_K);
		left_insert(1, q_t.front().key, TH_K);
		int timer = 0;
		while (!pq.empty() && !pc.empty()){
			omp.push(pq.top());
			tmp.push(pc.top());
			pq.pop();
			pc.pop();
			timer++;
		}
		//		printf("%d chunk selected ", timer);
		timer = 0;
		while (!tmp.empty() && !omp.empty()){
			leveldb::Status ss;
			leveldb::Slice kk(tmp.top().key, FP_SIZE);
			std::string ex;
			ss = uniq->Get(leveldb::ReadOptions(), kk, &ex);

			if(!ss.ok()){
				if(q_o.size()+1 > QUEUE_LIMIT) break;
				char buf[32];
				memset(buf,0,32);
				sprintf(buf, "%lu", tmp.top().count);
				leveldb::Slice uu(buf, sizeof(uint64_t));
				ss = uniq->Put(leveldb::WriteOptions(), kk, uu);
				timer++;
				q_o.push(omp.top());
				q_t.push(tmp.top());
			}
			omp.pop();
			tmp.pop();
		}
		//		printf("enqueue left %d chunks\n", timer);


		while(!pq.empty()) pq.pop();
		while(!pc.empty()) pc.pop();
		while(!omp.empty()) omp.pop();
		while(!tmp.empty()) tmp.pop();
		right_insert(0, q_o.front().key, TH_K);
		right_insert(1, q_t.front().key, TH_K);
		timer = 0;
		while (!pq.empty() && !pc.empty()){
			omp.push(pq.top());
			tmp.push(pc.top());
			pq.pop();
			pc.pop();
			timer++;
		}
		//		printf("%d chunk selected ", timer);
		timer = 0;
		while (!tmp.empty() && !omp.empty()){
			leveldb::Status ss;
			leveldb::Slice kk(tmp.top().key, FP_SIZE);
			std::string ex;
			ss = uniq->Get(leveldb::ReadOptions(), kk, &ex);

			if(!ss.ok()){
				if(q_o.size()+1 > QUEUE_LIMIT) break;
				char buf[32];
				memset(buf,0,32);
				sprintf(buf, "%lu", tmp.top().count);
				leveldb::Slice uu(buf, sizeof(uint64_t));
				ss = uniq->Put(leveldb::WriteOptions(), kk, uu);
				timer++;
				q_o.push(omp.top());
				q_t.push(tmp.top());
			}
			omp.pop();
			tmp.pop();
		}
		//		printf("enqueue right %d chunks\n\n", timer);
		q_o.pop();
		q_t.pop();
		involve ++;
	}
	printf("correct %lu\ninvolve chunk %lu\n", correct, involve);
}

int main (int argc, char *argv[]){
	// argv[1] points to a db name 
	init_db(argv[5], 1);
	init_db(argv[6], 11);
	init_db(argv[7], 12);
	init_db(argv[8], 2);
	init_db(argv[9], 21);
	init_db(argv[10], 22);
	init_db("./uniq-db/", 3);
	INIT = atoi(argv[1]);
	TH_K = atoi(argv[2]);
	QUEUE_LIMIT = atoi(argv[3]);
	LEAK_RATE = atof(argv[4]);

	stat_db();
	main_loop();
	//stat_uniq();

	return 0;
}