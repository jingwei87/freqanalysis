#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<vector>
#include"lrucache.h"
#include"bloom.h"
#include"container_manager.h"
#include"fp_index.h"

#define FP_SZIE 6
#define ERROR 0.5
#define BLOOM_lenth 200000

#define LRU_SIZE 500000
using namespace std;

bloom *BLM;
lrucache LPC(LRU_SIZE);
container_manager CMR;
fpindex FPI;
char * container_path;
char * index_path;
uint64_t index_access = 0;
uint64_t loading_access = 0;
uint64_t update_access = 0;
void sys_ini(char * path1, char *path2){

	BLM = new bloom;
	bloom_init(BLM, BLOOM_lenth, ERROR);
	bloom_init_conf(BLM); // init with conf
	CMR.init(path1);
	FPI.ini(path2);
	LPC.init_conf("./conf/LCPconf"); //iniit with conf data
}
void punique(char * key, int chunk_size)
{
	bloom_add(BLM, key, FP_SIZE);
	CMR.insert(key, chunk_size, container_path);
	if(FPI.insert(key, CMR.now_id) == 0){
		cout<<"error"<<endl;
	}
	update_access++;	// increment update_access for accessing index
}
void read_hashes(FILE *fp) {

    char read_buffer[256];
    char *item;
    char last[FP_SIZE];
    memset(last, 0, FP_SIZE);
	int count = 0,cnt = 0;
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
		int chunk_size = atoi((const char*)item);
		//Find it in LPC
		if(LPC.find(hash))
		{
			 cnt++;
			 
		}else
		{
			if(bloom_check(BLM, hash, FP_SIZE) == 0) //this is a unique_chunk
			{
				punique(hash, chunk_size);
			}else
			{
				int FPID = FPI.find(hash);
				index_access++;		// increment index_access for querying index
				if(FPID == -1) //this is a unique_chunk
				{
					punique(hash, chunk_size);
					count++;
				}else// dup it & load container
				{
					vector <string> conti;
					bool flag = CMR.loadtonode(container_path, conti, FPID);
					if(flag == 0){printf("load failed\n");exit(1);}
					vector<string>::iterator it;
					for(it = conti.begin(); it!= conti.end(); it++)
					{
						loading_access++;	// increment loading_access for loading metadata of a chunk
						LPC.putdata((*it).c_str());
					}
					conti.clear();
				}
			}
		}	
	}		
	CMR.pocessw(container_path);	// push in-memory container into disk
	storage_access = CMR.now_id;	// storage_access is ID of last container
	bloom_conf_out(BLM); //bloom output conf
}
int main(int arg, char *argv[])
{
	container_path = argv[1];
	index_path = argv[2];
	sys_ini(container_path, index_path);
	FILE * fp =fopen(argv[3], "r");
	if(fp == NULL){printf("OPen hash file failed!!!!\n");return 1;}

	read_hashes(fp);
	LPC.output_conf(); // LPC output
	
	printf("Index access: %lu\n", index_access);
	printf("Update access: %lu\n", update_access);
	printf("Loading access: %lu\n", loading_access);
	return 0;
}
