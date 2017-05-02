#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <inttypes.h>
#include <vector>
#include "leveldb/db.h"

// #define ANALYSIS_DB "./db/"
#define FP_SIZE 6

leveldb::DB *db;
leveldb::DB *store_left;
leveldb::DB *store_right;

uint64_t uniq = 0;
uint64_t total = 0;
double ratio = 0.0;

void init_left(char *left){
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, left, &store_left);
	assert(status.ok());
	assert(store_left != NULL);
}

void init_right(char *right){
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, right, &store_right);
	assert(status.ok());
	assert(store_right != NULL);
}

void init_db(char *db_name) {
	leveldb::Options options;
	options.create_if_missing = true;
	//	char db_name[20];
	//	sprintf(db_name, "%s", ANALYSIS_DB);
	leveldb::Status status = leveldb::DB::Open(options, db_name, &db);
	assert(status.ok());
	assert(db != NULL);
}

void timerstart(double *t){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*t = (double)tv.tv_sec+(double)tv.tv_usec*1e-6;
}

double timersplit(const double *t){
	struct timeval tv;
	double cur_t;
	gettimeofday(&tv, NULL);
	cur_t = (double)tv.tv_sec + (double)tv.tv_usec*1e-6;
	return (cur_t - *t);
}

void verify_db(){
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	leveldb::Status status;
	std::string existing_value;

	uint64_t count = 0;
	uint64_t tmp = 0;
	uint64_t addr = 0;
	uint64_t addl = 0;

	for (it->SeekToFirst(); it->Valid(); it->Next()){
		status = db->Get(leveldb::ReadOptions(), it->key(), &existing_value);
		if (status.ok()){
			count = strtoimax(existing_value.data(), NULL, 10);
			std::string list_value;
			leveldb::Status st;

			st = store_left->Get(leveldb::ReadOptions(), it->key(), &list_value);
			int len = 0;

			while(len < list_value.size()){
				const char* t_int = list_value.c_str()+len+FP_SIZE;
				tmp = *(int*)t_int;
				addr += tmp;

				len += (FP_SIZE+sizeof(int));
			}

			if(count > 20) printf("count %lu left %lu\n", count, addr);

			st = store_right->Get(leveldb::ReadOptions(), it->key(), &list_value);
			len = 0;
			while(len < list_value.size()){
				const char* t_int = list_value.c_str()+len+FP_SIZE;
				tmp = *(int*)t_int;
				addl += tmp;
				len += (FP_SIZE+sizeof(int));
			}
			if(count > 20) printf("count %lu right %lu\n", count, addl);

		}
		addr = 0;
		addl = 0;
	}
}

void read_hashes(FILE *fp) {
	char read_buffer[256];
	char *item;
	int flag = 0;
	char last[FP_SIZE];
	memset(last, 0, FP_SIZE);
	double t1 = 0;
	double t2 = 0;
	double timer, split;

	while (fgets(read_buffer, 256, fp)) {
		// skip title line
		if (strpbrk(read_buffer, "Chunk")) {
			continue;
		}

		timerstart(&timer);
		// a new chunk
		char hash[FP_SIZE];
		memset(hash, 0, FP_SIZE);


		// store chunk hash and size
		item = strtok(read_buffer, ":\t\n ");
		int idx = 0;
		while (item != NULL && idx < FP_SIZE){
			hash[idx++] = strtol(item, NULL, 16);
			item = strtok(NULL, ":\t\n");
		}

		leveldb::Status status;
		leveldb::Slice key(hash, FP_SIZE);

		// reference count
		uint64_t count;
		std::string existing_value;
		status = db->Get(leveldb::ReadOptions(), key, &existing_value);

		if (status.ok()) {
			//increment counter
			//			count = strtoimax(existing_value.c_str(), NULL, 10);
			count = strtoimax(existing_value.data(), NULL, 10);
			count++;
			status = db->Delete(leveldb::WriteOptions(), key);
		} else 
			count = 1;	// set 1
		char count_buf[32];
		memset(count_buf, 0, 32);
		sprintf(count_buf, "%lu", count);
		leveldb::Slice update(count_buf, sizeof(uint64_t));
		//		printf("%s\n", update.ToString().c_str());
		status = db->Put(leveldb::WriteOptions(), key, update);

		if (status.ok() == 0) 
			fprintf(stderr, "error msg=%s\n", status.ToString().c_str());

		split = timersplit(&timer);
		t1 += split;
		timerstart(&timer);
		//record adjacent chunk
		if(flag){
			//left
			status = store_left->Get(leveldb::ReadOptions(), key, &existing_value);

			std::string last_value;
			//last_value.resize(FP_SIZE);
			last_value.assign(last);
			last_value.resize(FP_SIZE);


			if(status.ok()){
				int pos = 0;
				int xi = 0;
				while(pos < existing_value.size()-1){
					if(memcmp(existing_value.c_str()+pos, last_value.c_str(), FP_SIZE) == 0){
						xi = 1;
						break;
					}
					pos += FP_SIZE + sizeof(int);
				}
				//std::size_t pos = existing_value.find(last_value);
				if(xi == 0){
					existing_value += last_value;
					std::string str_count;
					str_count.resize(sizeof(int));
					int init_value = 1;
					str_count.assign((char*)&init_value, sizeof(int));
					existing_value += str_count;
				}else{
					const char* tc = existing_value.c_str()+pos+FP_SIZE;
					int icm = *(int*)tc;
					icm ++;
					char tmp[sizeof(int)];
					memcpy(tmp, (const char*)&icm, sizeof(int));
					existing_value.replace(pos+FP_SIZE, sizeof(int), tmp, sizeof(int));
				}
				status = store_left->Delete(leveldb::WriteOptions(), key);
			}else{
				existing_value = last_value;
				std::string i_str;
				i_str.resize(sizeof(int));
				int i_v = 1;
				i_str.assign((char*)&i_v, sizeof(int));
				existing_value += i_str;
			}

			int size = existing_value.length();
			if(size % 10 != 0) printf("current size %d :: last size %lu\n", size, last_value.size());
			//printf("now count: %d\n", size / 10);
			leveldb::Slice current(existing_value.c_str(), existing_value.size());
			//printf("current strlen: %lu\n", existing_value.size());
			status = store_left->Put(leveldb::WriteOptions(), key, current);

			//right
			leveldb::Slice pre(last, FP_SIZE);
			status = store_right->Get(leveldb::ReadOptions(), pre, &existing_value);
			std::string next_value;
			next_value.assign(hash);
			next_value.resize(FP_SIZE);

			if(status.ok()){
				int ind = 0;
				int yi = 0;
				while(ind < existing_value.size()-1){
					if(memcmp(existing_value.c_str()+ind, next_value.c_str(), FP_SIZE) == 0){
						yi = 1;
						break;
					}
					ind += FP_SIZE + sizeof(int);
				}
				//std::size_t ind = existing_value.find(next_value);
				if(yi == 0){
					existing_value += next_value;
					std::string str_count;
					str_count.resize(sizeof(int));
					int init_value = 1;
					str_count.assign((char*)&init_value, sizeof(int));
					existing_value += str_count;
				}else{
					const char* tc = existing_value.c_str()+ind+FP_SIZE;
					int icm = *(int*)tc;
					icm ++;
					char tmp[sizeof(int)];
					memcpy(tmp, (const char*)&icm, sizeof(int));
					existing_value.replace(ind+FP_SIZE, sizeof(int), tmp, sizeof(int));
				}
				status = store_right->Delete(leveldb::WriteOptions(), pre);
			}else{
				existing_value = next_value;
				std::string i_str;
				i_str.resize(sizeof(int));
				int i_v = 1;
				i_str.assign((char*)&i_v, sizeof(int));
				existing_value += i_str;
			}

			leveldb::Slice now(existing_value.c_str(), existing_value.size());
			status = store_right->Put(leveldb::WriteOptions(), pre, now);

			split = timersplit(&timer);
			t2 += split;
		}


		// update last chunk
		memcpy(last, hash, FP_SIZE);
		if(flag == 0) flag = 1;

	}

	printf("insert %lf\tadjacent %lf\n", t1 ,t2);

}

int main (int argc, char *argv[]){
	assert(argc >= 5);
	// argv[1] points to hash file; argv[2] points to analysis db  

	assert(argv[2] != NULL);
	init_db(argv[2]);
	init_left(argv[3]);
	init_right(argv[4]);

	FILE *fp = NULL;
	fp = fopen(argv[1], "r");
	assert(fp != NULL);
	read_hashes(fp);
	//	verify_db();

	fclose(fp);
	delete db;
	db = NULL;
	return 0;
}
