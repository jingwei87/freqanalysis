#include "lrucache.h"
#include <cstdlib>
#include <cmath>
lrucache::lrucache()//constructor
{
	head = NULL;
	back = NULL;
	max_cache_size = 0;
	now_cache_size = 0;
	for(int i = 0 ; i < HASH_SIZE ; i++)hash_table[i] = NULL;
}
//new=======================
bool lrucache::init_conf(string path){
	
	fstream conf;
	conf.open(path.c_str());
	uint64_t cnt = 0;
	if(conf.is_open()){
		string temp;
		while(getline(conf,temp)){

			string data;
			for(int i = 0; i < FP_SIZE; i++){
				char tmp;
				int sum = 0;
				for(int j = 0; j < 8; j++){
					if(temp[(i*8)+j] == '1'){
						sum  += pow(2,(7-j));
					}
				}
				tmp = (char)sum;
				data += tmp;
			}
			//cout<<data<<endl;
			if(add_node_to_head(data.c_str())){
				cnt++;
			}
		}
		conf.close();
		// cout<<"LPC data read in num: "<<cnt<<endl;
		return true;		
	}
	else{
		conf.close();
		return false;
	}
	
}

bool lrucache::output_conf(){
	
	fstream conf;
	conf.open("./conf/LCPconf", ios::out);

	//char temp[FP_SIZE];
	uint64_t cnt = 0;

	listnode *p = new listnode;
	for (listnode *p = head; p != NULL; p = p ->next) {
		for(int k = 0; k < FP_SIZE; k++){
       			for(int j = 7; j >= 0; j--){
				if((p->hash_key[k] & 1<<j) != 0){
					conf<<"1";
				}
				else{
					conf<<"0";
				}
			}
		}
		conf<<endl;
		cnt++;
	}

	/*	
	for (uint64_t i = 0; i < HASH_SIZE; i++) {

		hashnode *h = hash_table[i];
		if(h == NULL){
			continue;
		}
		else{
			p = hash_table[i]->list_pos;
			while(p!= NULL) {
				for(int k = 0; k < FP_SIZE; k++){
        			for(int j = 7; j >= 0; j--){
					if((p->hash_key[k] & 1<<j) != 0){
						conf<<"1";
					}
					else{
						conf<<"0";
					}
				}
				}
				conf<<endl;
				p = p->next;
				cnt++;
    			}
		}
	}
	*/
	// cout<<"LPC data write num: "<<cnt<<endl;
	
	conf.close();
	if(cnt == 0){
		return false;
	}
	else{
		return true;
	}
}

//======================================
lrucache::lrucache(int size)
{
	head = NULL;
	back = NULL;
	max_cache_size = size;
	now_cache_size = 0;
	for(int i = 0 ; i < HASH_SIZE ; i++)hash_table[i] = NULL;
}
lrucache::~lrucache()
{
	listnode *p = head;
	while(p != NULL)
	{
		listnode *q = p->next;
		if(!delete_node(p)){printf("there is some error happen when deleting!\n");exit(1);}
		else p = q;
	}
	max_cache_size = 0;
	now_cache_size = 0;
	head = NULL;
	back = NULL;
	for(int i = 0; i < HASH_SIZE; i++)
	{
		while(hash_table[i] != NULL)
		{
			hashnode *q = hash_table[i]->next;
			delete hash_table[i];
			hash_table[i] = q;
		}
	}
}
void lrucache::set_max_size(int size)
{
	max_cache_size = size;
}

unsigned long lrucache::str_hash(const char *str)
{
	unsigned long key = 0 ;
	for(int i = 0; i < FP_SIZE; i++)
	{
		key = key * 5 + *(str + i);
	}
	key %= HASH_SIZE;
	key = abs(key);
	return key;
}

bool lrucache::insert_hash(const char *str, listnode *p)
{
	unsigned long pos = str_hash(str);
	hashnode * q = hash_table[pos];
	if(q == NULL)//first inserting
	{
		q = new hashnode;
		q->list_pos = p;
		q->next = NULL;
		q->front = NULL;
		hash_table[pos] = q;
		return 1;
	}else while(q->next != NULL)q = q->next;
	
	if(q != NULL && q -> next == NULL)
	{
		hashnode *t = new hashnode;
		if(t == NULL)return 0;
		t->list_pos = p;
		t->front = q ;
		t->next = NULL;
		q->next = t;
		return 1;
	}else return 0;
}

bool lrucache::find_hash(const char *str, listnode * &result)
{
	result = NULL;
	unsigned long pos = str_hash(str);
	hashnode * q = hash_table[pos];
	bool flag = 0;
	while(q != NULL)
	{
		if(strncmp( q->list_pos->hash_key, str, FP_SIZE) == 0)
		{
			result = q->list_pos;
			flag = 1;
			break;
		}else q = q->next;
	}
	if(flag){
		delete_node(result);
		add_node_to_head(str);
		result = head;
	}
	return flag;
}


bool lrucache::delete_hash(const char *str)
{
	unsigned long pos = str_hash(str);
	hashnode *q = hash_table[pos];
	while(q != NULL)
	{
		if(strncmp(q->list_pos->hash_key ,str ,FP_SIZE) == 0)
		{
			hashnode* t = q;
			if(t->front!= NULL)t->front->next = t-> next;
			else hash_table[pos] = t-> next;
			if(t->next != NULL)t->next->front = t-> front;
			delete q;
			return 1;
		}
		q = q -> next;
	}
	return 0;
}

bool lrucache::add_node_to_head(const char *str)
{
	listnode *q = new listnode;
	if(q == NULL)return 0;
	for(int i = 0;i < FP_SIZE ;i++)q->hash_key[i] = *(str + i);
	//string add(str, 6);
	//cout<<add<<endl;
	q->next = head;
	q->front = NULL;
	if(head == NULL) back =q;
	else head->front = q;
	head = q;
	insert_hash(str, q);
	if(now_cache_size < max_cache_size)
		now_cache_size ++ ; 
	else
		{delete_node(back);now_cache_size++;}
	return 1;
}

bool lrucache::delete_node(listnode *p)
{
	listnode *t =p;
	now_cache_size -- ;
	//for(int i = 0;i < 6; i++)printf("%02hhx:",t->hash_key[i]);printf("\n");
	if(p == NULL)return 0;
	if(t->front != NULL) t->front->next = t->next;
	else head = t->next;
	if(t->next != NULL) t->next->front = t->front;
	else back = t->front;
	delete_hash(t->hash_key);
	delete p;
	return 1;
}

bool lrucache::putdata(const char *str)
{
	if(find(str)){
		return false;
	}
	else{
		//cout<<"flag"<<endl;
		return add_node_to_head(str);
	}
	
}

bool lrucache::find(const char *str)
{
	listnode *t = NULL;
	bool flag = find_hash(str, t);
	if (!flag) return 0;
	else return 1;
}












