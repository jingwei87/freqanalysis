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

uint64_t uniq = 0;
uint64_t total = 0;
double ratio = 0.0;

void init_db(char *db_name) {
	leveldb::Options options;
	options.create_if_missing = true;
	//	char db_name[20];
	//	sprintf(db_name, "%s", ANALYSIS_DB);
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

		uint64_t size = atoi((const char*)item);
		total += size;

		leveldb::Status status;
		leveldb::Slice key(hash, FP_SIZE);

		std::string existing_value;
		status = db->Get(leveldb::ReadOptions(), key, &existing_value);

		if (status.ok()) {
		} else{ 
			//		printf("%s\n", update.ToString().c_str());
			uniq += size;
		}

	}
	printf("unique: %lu\ttotal: %lu\n", uniq, total);
	ratio = (1-((double)uniq/total))*100;
	printf("dedup ratio: %lf%%\n", ratio);


}

int main (int argc, char *argv[]){
	assert(argc >= 3);
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
