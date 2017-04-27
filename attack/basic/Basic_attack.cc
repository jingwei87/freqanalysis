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

#define TRAINING 1
#define TARGET 2

struct Node
{
	char hash_name[FP_SIZE];
	int count;

	bool operator<(const Node& a) const
	{
		return count < a.count;
	}
	bool operator>(const Node& a) const
	{
		return count > a.count;
	}
};
leveldb::DB *Trainingdb = NULL;
leveldb::DB *Targetdb = NULL;
leveldb::DB *Uniquedb = NULL;

int Unique, Correct;
int Max_lenth;
priority_queue<Node, vector<Node> >Training_queue;
priority_queue<Node, vector<Node> >Target_queue;


void Init_Db(char *Db_name, int type)
{
	leveldb::Options options;
 	options.create_if_missing = true;
	leveldb::Status status;
	if(type == 1)
 	status = leveldb::DB::Open(options, Db_name, &Trainingdb);
	else if(type ==2)
	status = leveldb::DB::Open(options, Db_name, &Targetdb);
	else if(type == 3)
	status = leveldb::DB::Open(options, Db_name, &Uniquedb);

 	assert(status.ok());
}

void ReadDbs(int type)
{
	leveldb::DB *temp_db = NULL;
	priority_queue<Node, vector<Node> > *temp_que;
	if(type == 1)
	{
		temp_db = Trainingdb;
		temp_que = &Training_queue;
	}else
	if(type ==2)
	{
		temp_db = Targetdb;
		temp_que = &Target_queue;
	}
	assert(temp_db != NULL)	;
	leveldb::Iterator* it = temp_db->NewIterator(leveldb::ReadOptions());
	leveldb::Status status;
	for(it->SeekToFirst(); it->Valid(); it->Next())
	{
		string value;
		status = temp_db->Get(leveldb::ReadOptions(), it->key(), &value);
		assert(status.ok());
		int temp_count = *(int *)value.c_str();
		Node temp_Node;
		memcpy(temp_Node.hash_name, it->key().ToString().c_str(), FP_SIZE);
		temp_Node.count = temp_count;
		//-------------Inserting begin------------------
		if((*temp_que).size() < (unsigned int)Max_lenth)  // Max_lenth > 0
		{
			(*temp_que).push(temp_Node);
		}else
		{
			if((*temp_que).top().count < temp_Node.count)
			{
				(*temp_que).pop();
				(*temp_que).push(temp_Node);
			}
		}
	}

}

void Stat_Unique()
{
	Unique = Target_queue.size();
}

void Fre_Analysis()
{
	int size = min(Training_queue.size(), Target_queue.size());
	for(int i = 0;i < size; i++)
	{
		if(memcmp(Training_queue.top().hash_name, Target_queue.top().hash_name, FP_SIZE) == 0) Correct ++ ;
		Training_queue.pop();Target_queue.pop();
	}
}
int main(int argc, char *argv[])
{
	Init_Db(argv[1], TRAINING);
	Init_Db(argv[2], TARGET);
	Max_lenth = 99999999;
	ReadDbs(TRAINING);
	ReadDbs(TARGET);
	Stat_Unique();
	Fre_Analysis();
	printf("Total number of unique chunks:%d\nCorrect inference:%d\n", Unique, Correct);
	return 0;
}