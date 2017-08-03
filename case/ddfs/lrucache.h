//this a simple LRU cache
//#ifdef LRUCACHE_H
//#define LRUCACHE_H

#include <cstdlib>
#include <stdint.h>
#include <string>
#include <cstdio>
#include <cmath>
#include <string.h>
#include <bits/stdc++.h>

using namespace std;
#define FP_SIZE 6
#define HASH_SIZE 3000000
struct listnode
{
	char hash_key[FP_SIZE];
	listnode *next;
	listnode *front;
};
struct hashnode
{
	listnode *list_pos;
	hashnode *next;
	hashnode *front;
};
class lrucache
{
	public:
		int64_t max_cache_size;
		int64_t now_cache_size;
		lrucache();// Constructor
		lrucache(int size);// Constructor with size
		~lrucache();
		//new======================
		bool init_conf(string path); // load conf
		bool output_conf(); // out put conf
		//========================
		void set_max_size(int size);
		bool putdata(const char *str);//put a sting into cache, with a ruturn 1 when inserting success;
		bool find(const char *str);//find a strnig in cache, with 1 when find it;
	private:
		//------------this is hash operation ---------------------
		unsigned long str_hash(const char *str);
		bool insert_hash(const char *str, listnode *p);
		bool find_hash(const char *str, listnode * &result);
		bool delete_hash(const char *str);
		//------------this is list operation ---------------------
		bool add_node_to_head(const char *str);
		bool delete_node(listnode *p);
		//---------------------------------------------------
		hashnode *hash_table[HASH_SIZE];
		listnode *head;
		listnode *back;
};
//#endif
