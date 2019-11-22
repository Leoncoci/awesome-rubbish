#include<stdio.h>
#include<windows.h>
/*申明计时用全局变量*/ 
LARGE_INTEGER litmp;
double frequency,runTime,minus;
LONGLONG timeStart,timeEnd;

void time_count_start(){//开始计时 
	QueryPerformanceFrequency(&litmp);
  	frequency=(double) litmp.QuadPart;
  	QueryPerformanceCounter(&litmp);
  	timeStart=litmp.QuadPart;
}
void time_count_end(){	//计算从上次计时到结束的运行时间 
	QueryPerformanceCounter(&litmp);
 	timeEnd=litmp.QuadPart;
 	minus=(double)(timeEnd-timeStart);
 	runTime=minus/frequency;
}

int main(){
	time_count_start(); //写在开始某个时间统计之前
	//TODO 要统计运行时间的程序
	time_count_end(); 
	printf("%lf",runTime);
}
