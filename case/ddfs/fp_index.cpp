#include "fp_index.h"
#define INT_LENGTH 4
fpindex::fpindex(const char *path)
{
	db = NULL;
	leveldb::Options options;
        options.create_if_missing = true;
        leveldb::Status status;
        status = leveldb::DB::Open(options, path, &db);
	assert(status.ok());
}

fpindex::~fpindex()//not safe
{
	db = NULL;
}

bool fpindex::insert(char *str, int ID)
{
	leveldb::Slice key(str, FP_SZIE);
	leveldb::Status status;
	string existing_value = "";
	status = db->Get(leveldb::ReadOptions(), key, &existing_value);
	
	if(!status.ok()) renturn 0;

	leveldb::Slice value((char *)&ID, sizeof(int));
	status = db->Put(leveldb::WriteOptions(), key, value);
	if(status.ok()) return 1;
	else return 0;

}

int fpindex::find(char *str)
{
	leveldb::Slice key(str, FP_SIZE);
	leveldb::Status status;
        string existing_value = "";
        status = db->Get(leveldb::ReadOptions(), key, &existing_value);
	if(!status.ok()) return -1;
	
	int contain_id = -1;
	contain_id = * (int *)(existing_value.c_str());
	return contain_id ;
}
