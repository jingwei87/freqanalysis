
#define _CONTAINER_MANAGER_H
#include<cstdlib>
#include<cstdio>
#include<string>
#include<fstream>
#include<dirent.h>
#include<vector>
#define FP_SIZE 6
#define MAX_SIZE 4*1024*1024
class container_manager
{
	public:
	bool insert(char *str,int size,char *path);
	int init(char *path);
	bool loadtonode(char *path, vector<string> ans, int ID);
	int now_id;
	private:
	void pocessw(char *path);
	vector<string>tmp;
	int now_size;
	string now_key;
	int chunk_size;
}
