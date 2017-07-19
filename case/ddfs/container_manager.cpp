#include "container_manager.h"
#include <sstream>
int container_manager::init(char *path)
{
	string mainfest(path);
	int flag = -1; 
	mainfest+="MAINFEST";
	FILE *fl = fopen (mainfest.c_str(), "rt");
	if(fl = NULL)// this is new container
	{
		fl = fopen (mainfest.c_str(), "w");
		fprintf(fl, "0\n");
		now_id = 0;
		now_size = 0;
		now_key = "";
		tmp.clear();
		paths = path;flag = 0;
	}
	else
	{
		fscanf(fl, "%d", &now_id);
		now_size = 0;
		now_key = "";
		tmp.clear();
		paths = path;flag =1;
	}
	fclose(fl);
	return flag;
}

void container_manager::pocessw(char *path)
{
	string filename = "", fi = "";
	char ter;int ee = now_id ;now_id++;
	while(ee != 0)
	{
		fi += (char)((ee % 10) + '0');
		ee /= 10;
	}
	int len = fi.length();
	filename.resize(len);
	for (int i = 0; i < len ; i++)filename[len - 1 - i] = fi[i];
	FILE * fw = fopen(filename , "w");
	vector<string>::iterator it;
	for (it = tmp.begin(); it != tmp.end(); it++)
	{
		fprintf(fw,"%s\n", it->c_str());
	}
	tmp.clear();
	now_size = 0;
	fclose(fw);
	string mainfest(path);
        mainfest+="MAINFEST";
	fw = fopen (mainfest.c_str(), "w");
	fprintf(fw, "%d\n", now_id);
	fclose(fw);
}
bool container_manager::insert(char *str, int size, char *path)
{
	string hash(str);
	if(now_size + size > MAX_SIZE)
	{
		pocessw();
	}
	now_size += size;
	tmp.push(hash);
}

bool container_manager::loadtonode (char *path, vector<string> &ans, int ID)
{
	string fi="",filename(path);
	ans.clear();
	stringstream stream;
	stream << ID;
	fi = stream.str();
	filename += fi;
	FILE * fr = fopen (filename.c_str(), "r");
	assert(fr != NULL)
	char T[FP_SZIE];
	while(fscanf(fr, “%s”, T))
	{
		string si(T);
		ans.push_back(T);
	}
	return 1;
}








