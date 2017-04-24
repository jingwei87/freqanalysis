#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <inttypes.h>
#include "leveldb/db.h"

// #define ANALYSIS_DB "./db/"
#define FP_SIZE 6

leveldb::DB *db;


uint64_t total = 0;
uint64_t target = 0;
uint64_t correct = 0;

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

	while (fgets(read_buffer, 256, fp)) {
		// a new chunk
		char hash[FP_SIZE];
		memset(hash, 0, FP_SIZE);

		total++;

		// store chunk hash
		item = strtok(read_buffer, ":\t\n ");
		int idx = 0;
		while (item != NULL && idx < FP_SIZE){
			hash[idx++] = strtol(item, NULL, 16);
			item = strtok(NULL, ":\t\n");
		}

		leveldb::Status status;
		leveldb::Slice key(hash, FP_SIZE);
		std::string existing_value;

		status = db->Get(leveldb::ReadOptions(), key, &existing_value);
		if (status.ok()) {
			target++;
			uint64_t rank = strtoimax(existing_value.data(), NULL, 10); 
			if (total == rank) {
				correct++;
//				printf("1\n");
			}
			else {
//				printf("0\n");
			}
		} 
	}
}

int main (int argc, char *argv[]){
	assert(argc >= 3);
	// argv[1] points to histogram; argv[2] points to analysis db  

	assert(argv[2] != NULL);
	init_db(argv[2]);

	FILE *fp = NULL;
	fp = fopen(argv[1], "r");
	assert(fp != NULL);
	read_hashes(fp);

	fclose(fp);
	delete db;
	db = NULL;

	printf("Unique chunks: %lu\nIn target: %lu\nCorrect guess: %lu\n", total, target, correct);
	return 0;
}
