#include<stdio.h>
#include<windows.h>
int main(){
	LARGE_INTEGER litmp;
	double frequency,runTime,minus;//CPU时钟频率，运行时间，运行周期数之差
 	LONGLONG timeStart,timeEnd;//排序前周期数，排序执行后周期数
 	QueryPerformanceFrequency(&litmp);
  	frequency=(double) litmp.QuadPart;
  	QueryPerformanceCounter(&litmp);
  	timeStart=litmp.QuadPart;
  
  	int a,b;
  	a=1;
  	b=0;
  	int temp;
  	temp = a;
  	a = b;
  	b = temp;
 
 	QueryPerformanceCounter(&litmp);
 	timeEnd=litmp.QuadPart;
 	minus=(double)(timeEnd-timeStart);
 	runTime=minus/frequency;
 	printf("%lf",runTime);
} 
