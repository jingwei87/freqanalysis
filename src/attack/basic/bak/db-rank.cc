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
	uint64_t rank = 1;

	while (fgets(read_buffer, 256, fp)) {
		// a new chunk
		char hash[FP_SIZE];
		memset(hash, 0, FP_SIZE);

		// store chunk hash and size
		item = strtok(read_buffer, ":\t\n ");
		int idx = 0;
		while (item != NULL && idx < FP_SIZE){
			hash[idx++] = strtol(item, NULL, 16);
			item = strtok(NULL, ":\t\n");
			//			printf("%02x", (unsigned char)hash[idx++]);
			//			if (idx < FP_SIZE) {
			//				printf(":");
			//			}
		}
		leveldb::Status status;
		leveldb::Slice key(hash, FP_SIZE);
		char rank_buf[32];
		memset(rank_buf, 0, 32);	
		sprintf(rank_buf, "%lu", rank);
		leveldb::Slice update(rank_buf, sizeof(uint64_t));
		status = db->Put(leveldb::WriteOptions(), key, update);
		assert(status.ok() != 0);

		rank++;
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
	return 0;
}
