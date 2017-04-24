#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <inttypes.h>
#include "leveldb/db.h"

#define ANALYSIS_DB "./db/"
#define FP_SIZE 6

leveldb::DB *db;
uint64_t record = 0;
uint64_t max_count = 0;
uint64_t total_count = 0;

void init_db(char *db_name) {
 	leveldb::Options options;
	options.create_if_missing = true;
//	char db_name[20];
//	sprintf(db_name, "%s", ANALYSIS_DB);
	leveldb::Status status = leveldb::DB::Open(options, db_name, &db);
	assert(status.ok());
	assert(db != NULL);
}

void print_db() {
	int len = 0;

	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		for (len = 0; len < FP_SIZE; len++) {
			printf("%02x", (unsigned char)it->key().ToString().c_str()[len]);
			if (len < FP_SIZE - 1) {
				printf(":");
			}
		}
		printf("\t");

		printf("%s", it->value().ToString().c_str());
		uint64_t val = strtoimax(it->value().ToString().c_str(), NULL, 10);
		if (max_count < val) {
			max_count = val;
		}
		record++;
		total_count += val;
		printf("\n");
  	}
  	assert(it->status().ok());  // Check for any errors found during the scan
	delete it;
}


int main (int argc, char *argv[]){
	assert(argc >= 2 && argv[1] != NULL);
	// argv[1] points to a db name 
	init_db(argv[1]);

	print_db();
	delete db;
	db = NULL;

	//printf("Db records: %llu\nTotal counts: %llu\nMaximum counts: %llu\n", record, total_count, max_count);
	return 0;
}
