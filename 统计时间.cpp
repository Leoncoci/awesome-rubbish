#include<stdio.h>
#include<windows.h>
int main(){
	LARGE_INTEGER litmp;
	double frequency,runTime,minus;//CPUʱ��Ƶ�ʣ�����ʱ�䣬����������֮��
 	LONGLONG timeStart,timeEnd;//����ǰ������������ִ�к�������
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
