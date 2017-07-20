#define _FP_INDEX_H
#include <cstdlib>
#include <cstdio>
#include "leveldb/db.h"
#include <string>
#include <assert.h>
#define FP_SIZE 6
class fpindex
{
	public:
	fpindex(const char *path);
	fpindex();
	~fpindex();
	void ini(const char *path);
	int insert(char *str, int ID);
	int find(char *str);
	private:
	leveldb::DB *db;
};
