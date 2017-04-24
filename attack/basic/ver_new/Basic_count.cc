#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <vector>
#include "leveldb/db.h"

// #define ANALYSIS_DB "./db/"
#define FP_SIZE 6

leveldb::DB *db;

uint64_t uniq = 0;
uint64_t total = 0;
double ratio = 0.0;


void init_db(char *db_name) {
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, db_name, &db);
	assert(status.ok());
	assert(db != NULL);
}

void read_hashes(FILE *fp) {
	char read_buffer[256];
	char *item;
	char last[FP_SIZE];
	memset(last, 0, FP_SIZE);

	while (fgets(read_buffer, 256, fp)) {
		// skip title line
		if (strpbrk(read_buffer, "Chunk")) {
			continue;
		}

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

		}

}

int main (int argc, char *argv[]){
//	assert(argc >= 5);
	// argv[1] points to hash file; argv[2] points to analysis db  

	assert(argv[2] != NULL);
	init_db(argv[2]);

	FILE *fp = NULL;
	fp = fopen(argv[1], "r");
	assert(fp != NULL);
	read_hashes(fp);

	fclose(fp);
	delete db;
	db = NULL;
	return 0;
}
