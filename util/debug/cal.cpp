#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
using namespace std;
long long int sum = 0;
int tmp, a[5000];
int n = 0;
int main(int argc, char *argv[])
{
	FILE *fp = fopen(argv[1] , "r");
	if (fp == NULL){
		printf("File open fail\n");
	}
	int pos = 0;
	while(fscanf(fp, "%d%d", &tmp, &a[pos++]) != EOF){
		sum += a[pos - 1];
	}
	n = pos;
	n -= 1;
	double ans, t;
	printf("Total:%d\n", n);
	for (int i = 0; i < n; i++){
		t = (double)a[i] / sum;
		ans += t * log(t);	
	}	
	printf("%f\n", ans);
	return 0;
}
