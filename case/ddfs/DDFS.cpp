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
#define BLOOM_lenth 2000000
#define LRU_SIZE 500000
using namespace std;

bloom *BLM;
lrucache LPC(LRU_SIZE);
container_manager CMR;
fpindex FPI;
char * container_path;
char * index_path;
uint64_t lpc_q_amount = 0, lpc_q_success = 0, lpc_q_fail = 0;
uint64_t bloom_q_fail = 0;
uint64_t fpi_q_amount = 0, load_time = 0;
uint64_t unique_amount = 0;
void sys_ini(char * path1, char *path2)
{
	BLM = new bloom;
	bloom_init(BLM, BLOOM_lenth, ERROR);
	CMR.init(path1);
	FPI.ini(path2);
}
void punique(char * key, int chunk_size)
{
	bloom_add(BLM, key, FP_SIZE);
	LPC.putdata(key);
	CMR.insert(key, chunk_size, container_path);
	FPI.insert(key, CMR.now_id);
	printf("syscheck%lld\t%lld\n", unique_amount,lpc_q_amount);
}
void read_hashes(FILE *fp) 
{
        char read_buffer[256];
        char *item;
        char last[FP_SIZE];
        memset(last, 0, FP_SIZE);
        while (fgets(read_buffer, 256, fp)) 
	{
                // skip title line
                if (strpbrk(read_buffer, "Chunk")) 
		{
                        continue;
                }

                // a new chunk
                char hash[FP_SIZE];
                memset(hash, 0, FP_SIZE);


                // store chunk hash and size
                item = strtok(read_buffer, ":\t\n ");
                int idx = 0;
                while (item != NULL && idx < FP_SIZE)
		{
                        hash[idx++] = strtol(item, NULL, 16);
                        item = strtok(NULL, ":\t\n");
                }
		int chunk_size = atoi((const char*)item);
		//Find it in LPC
		lpc_q_amount ++ ;
		if(LPC.find(hash))
		{
			//----dup it ------------
			 lpc_q_success ++;
		}else
		{
			if(bloom_check(BLM, hash, FP_SIZE) == 0) //this is a unique_chunk
			{
				unique_amount ++ ;bloom_q_fail ++;
				punique(hash, chunk_size);
			}else
			{
				fpi_q_amount++;
				int FPID = FPI.find(hash);
				if(FPID == -1) //this is a unique_chunk
				{
					unique_amount ++;
					punique(hash, chunk_size);
				}else// dup it & load container
				{
					//printf("n:%dL:%d\n", CMR.now_id, FPID);
					load_time ++ ;
					vector <string> conti;
					bool flag = CMR.loadtonode(container_path, conti, FPID);
					if(flag == 0){printf("load failed\n");exit(1);}
					vector<string>::iterator it;
					for(it = conti.begin(); it!= conti.end(); it++)
					{
						LPC.putdata((*it).c_str());
					}
					conti.clear();
				}
			}
		}	
	}		
}
int main(int arg, char *argv[])
{
	container_path = argv[1];
	index_path = argv[2];
	sys_ini(container_path, index_path);
	FILE * fp =fopen(argv[3], "r");
	if(fp == NULL){printf("OPen hash file failed!!!!\n");return 1;}
	read_hashes(fp);
	char hash0[FP_SIZE];
	memset(hash0, 0 ,FP_SZIE);
	CMR.insert(hash0, 4*1024*1024+1 , container_path);	
	printf("Total chunk:%lld\n" , lpc_q_amount);
	printf("dup with LPC:%lld\n", lpc_q_success);
	printf("unique chunk:%lld\n", unique_amount);
	printf("find new chunk with BLOOM:%lld\n", bloom_q_fail);
	printf("load container time:%lld\n",load_time);

	return 0;
}