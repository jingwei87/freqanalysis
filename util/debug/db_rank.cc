#include<assert.h>
#include<stdio.h>
#include<string.h>
#include<cstdlib>
#include<queue>
#include<vector>
#include "leveldb/db.h"
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <stack>
#include <inttypes.h>
using namespace std;
#define FP_SIZE 6

leveldb::DB *Ori_db = NULL;//original db refer to plaintext
leveldb::DB *Tar_db = NULL;//target db , refer to encryped text

struct node
{
	char key[FP_SIZE];
	uint64_t count;
	bool operator<(const node& a) const
	{
		return count > a.count;
	}
};
priority_queue<node> ori_rank;
priority_queue<node> tar_rank;

void Init_Original_db(string name)
{
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, name, &Ori_db);
	assert(status.ok());
	assert(Ori_db != NULL);
}

void Init_Target_db(string name)
{
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options, name, &Tar_db);
	assert(status.ok());
	assert(Tar_db != NULL);
}

void read_db(leveldb::DB* db, uint64_t k, bool type)
{
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	priority_queue<node, vector<node> > *rank;
	node entry;
	if(type==0){rank = & ori_rank;}
	else {rank = & tar_rank;}
//  	printf("Nod");
//	it->SeekToFirst();
//	printf("%d", it->Valid());
//	printf("%s\n",it->value().ToString().c_str());
//	printf("hello world\n");
	for(it->SeekToFirst(); it->Valid(); it->Next())
	{
		memcpy(entry.key, it->key().ToString().c_str(), FP_SIZE);
		//entry.count = strtoimax(it->value().ToString().c_str(), NULL, 10);
		const char* sst= it->value().ToString().c_str();
		entry.count = *(uint64_t *)sst;
		if((*rank).size()<k) (*rank).push(entry);
		else 
		{
			if((*rank).top().count < entry.count){(*rank).pop();(*rank).push(entry);}	
		}
	}
	
}

void out()
{
	node tmp;
	stack<node> a;
	while(!ori_rank.empty())
	{
		tmp=ori_rank.top();ori_rank.pop();a.push(tmp);
	}
	while(!a.empty())
	{
		for(int i=0; i<FP_SIZE; i++)
		{
			printf("%02x",(unsigned char)a.top().key[i]);
			if(i!=FP_SIZE-1)printf(":");
		}printf("\t%ld\n",a.top().count);
		a.pop(); 
	}
}
int main(int arc,char *arcv[])
{
	if(arc < 2) {printf("Invailid Input!\n");return 1;}
	//int u = arcv[1][0] - '0';
	int u=50000000;
//	printf("u=%d\n",u);
	Init_Original_db(arcv[1]);
	read_db(Ori_db, u, 0);
	out();
	return 0;
}
