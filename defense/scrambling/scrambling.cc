#include <assert.h>
#include <stdio.h>
#include <queue>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <vector>
#include <deque>
#define FP_SIZE 6
#define SEG_SIZE ((2<<20)) //1MB default
#define SEG_MIN ((2<<19)) //512KB
#define SEG_MAX ((2<<21)) //2MB

using namespace std;

struct node
{
	char key[FP_SIZE];
	uint64_t size;
};

queue<node> sq;
uint64_t sq_size = 0;

void process_seg()
{
	deque <node> deq;
	deq.clear();
	while(!sq.empty())
	{
		int k =( rand() * rand() ) % 2;
		node tmp = sq.front();
		if(k == 0)deq.push_front(tmp);
		else deq.push_back(tmp);
		sq.pop();
	}
	while (!deq.empty())
	{
		node tmp = deq.front();
		deq.pop_front();
		printf("%.2hhx", tmp.key[0]);
	        for(int j = 1; j < FP_SIZE; j++)
			printf(":%.2hhx", tmp.key[j]);
	        printf("\t\t%" PRIu64 " ", tmp.size);
	        printf("\t\t\t10\n");
	}
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
		while (item != NULL && idx < FP_SIZE){
			hash[idx++] = strtol(item, NULL, 16);
			item = strtok(NULL, ":\t\n");
		}

		uint64_t size = atoi((const char*)item);


		if (sq_size + size > SEG_MAX || (sq_size >= SEG_MIN && (hash[5] << 2) >> 2 == 0x3f))
		{
			process_seg();
			while(!sq.empty()) sq.pop();
			sq_size = 0;
		}

		node entry;
		memcpy(entry.key, hash, FP_SIZE);
		entry.size = size;

		sq_size += size;
		sq.push(entry);
	}
}

int main (int argc, char *argv[])
{
	srand((unsigned)time(NULL));
	assert(argc >= 2);
	// argv[1] points to hash file; argv[2] points to analysis db  

	FILE *fp = NULL;
	fp = fopen(argv[1], "r");
	printf("Chunk Hash\t\t\tChunk Size (bytes)\tCompression Ratio (tenth)\n");
	assert(fp != NULL);
	read_hashes(fp);
	process_seg();
	fclose(fp);
	return 0;
}
