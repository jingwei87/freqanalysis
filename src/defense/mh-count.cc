#include <assert.h>
#include <stdio.h>
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
leveldb::DB *relate;

uint64_t uniq = 0;
uint64_t total = 0;
double ratio = 0.0;

void init_relate(char *rel){
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, rel, &relate);
	assert(status.ok());
	assert(relate != NULL);
}

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

void read_hashes(FILE *fp, FILE* ft) {
	char read_buffer[256];
	char plain_buffer[256];
	char *item;
	char *citem;
	int flag = 0;
	char last[FP_SIZE];
	memset(last, 0, FP_SIZE);

	while (fgets(read_buffer, 256, fp)) {
		// skip title line
		if (strpbrk(read_buffer, "Chunk")) {
			continue;
		}

		fgets(plain_buffer, 256, ft);
		if (strpbrk(plain_buffer, "Chunk")){
			fgets(plain_buffer, 256, ft);
		}

		// a new chunk
		char hash[FP_SIZE];
		memset(hash, 0, FP_SIZE);
		char plain[FP_SIZE];
		memset(plain, 0, FP_SIZE);


		// store chunk hash and size
		item = strtok(read_buffer, ":\t\n ");
		int idx = 0;
		while (item != NULL && idx < FP_SIZE){
			hash[idx++] = strtol(item, NULL, 16);
			item = strtok(NULL, ":\t\n");
		}

		uint64_t sit = atoi((const char*)item);

		citem = strtok(plain_buffer, ":\t\n ");
		int cidx = 0;
		while (citem != NULL && cidx < FP_SIZE){
			plain[cidx++] = strtol(citem, NULL, 16);
			citem = strtok(NULL, ":\t\n");
		}

		uint64_t scit = atoi((const char*)citem);//This is chunk size

		leveldb::Status status;
		leveldb::Status cst;
		leveldb::Slice key(hash, FP_SIZE);
		leveldb::Slice pkey(plain, FP_SIZE);

		if(scit == sit){
			cst = relate->Put(leveldb::WriteOptions(), key, pkey);
		}

		total += sit;

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
			uniq += sit;
		count = 1;	// set 1
		char count_buf[32];
		memset(count_buf, 0, 32);
		sprintf(count_buf, "%lu", count);
		leveldb::Slice update(count_buf, sizeof(uint64_t));
		//		printf("%s\n", update.ToString().c_str());
		status = db->Put(leveldb::WriteOptions(), key, update);

		if (status.ok() == 0) 
			fprintf(stderr, "error msg=%s\n", status.ToString().c_str());

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

		}

		// update last chunk
		memcpy(last, hash, FP_SIZE);
		if(flag == 0) flag = 1;

	}

	//	ratio = 1-((double)uniq/total);
	//	printf("total: %lu\nuniq: %lu\nratio: %lf\n", total, uniq, ratio);

}

int main (int argc, char *argv[]){
	assert(argc >= 7);
	// argv[1] points to hash file; argv[2] points to analysis db  

	assert(argv[2] != NULL);
	init_db(argv[2]);
	init_left(argv[3]);
	init_right(argv[4]);
	init_relate(argv[6]);

	FILE *fp = NULL;
	fp = fopen(argv[1], "r");
	assert(fp != NULL);

	FILE *ft = NULL;
	ft = fopen(argv[5], "r");
	assert(ft != NULL);
	read_hashes(fp, ft);

	fclose(fp);
	fclose(ft);
	delete db;
	db = NULL;
	return 0;
}
