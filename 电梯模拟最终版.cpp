#include<stdio.h> 
#include<stdlib.h>
#include<time.h>
//申明

#define SPACEMAX 10 
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define OVERFLOW -2
#define PAUSE 0
//函数状态申明 

#define GoingDown 0
#define GoingUp 1
#define SpeedUp 2
#define SpeedDown 3
#define SlowUp 4
#define SlowDown 5
#define Idle 6
#define Stop 7
#define DoorOpen 8
#define DoorOpening 9
#define DoorCloseing 10
#define ElevatorOut 11
#define QueueIn 12
//电梯状态申明

#define CloseTest 40
#define OverTime 300
#define Accelerate 15
#define UpTime 51
#define DownTime 61
#define UpDecelerate 14
#define DownDecelerate 23
#define DoorTime 20
#define IOTime 25
//电梯动态过程所用时间

typedef int Status;
typedef struct{//定义人的元素 
	int name;
	int durtime;
	int wannafloor; 
	int Infloor;
}Human;
typedef struct Hnode{
	Human data;
	Hnode *next;
	Hnode *pre;
}Hnode;
typedef struct{
	Hnode *front;
	Hnode *rear;
}ElevatorSpace;     

typedef struct{
	ElevatorSpace ES;
	int State;
	int TotalNum;
	int floor; 
	int CallCar[6]; 
}Elevator;

typedef struct Qnode{
	Human data;
	Qnode *next;
	Qnode *pre;
}Qnode;

typedef struct{
	Qnode *front;
	Qnode *rear;	
}Queue;

int timeline;//定义时间轴 单位0.1，意思是time=100表示现在10s 
int endtime;//输入结束时间 
int nexttime;//下一个人到来间隔时间 
int SLDT;//减速下降时间 
int SLUT;//减速上升时间
int SPDT;//加速下降时间
int SPUT;//加速上升时间 
int UT;//上升时间 
int DT;//下降时间 
int EWT;//电梯等待时间
int DOT; //电梯开门时间
int DCT;//电梯关门时间 
int ETT;//电梯开门时测试间隔时间 
int HIOT;//每个人进出时间 
int Aim;//电梯目标楼层 
Elevator E;
Status LPW;     //最后一个人按的上还是下 
Queue floor[6];//为了避免歧义floor[0]不使用
Status FloorButton[6][2];
Human JoiningMan[100000]; 
int J,O;

Status InitElevatorSpace(){//初始化电梯队列空间 
	E.ES.front=E.ES.rear=(Hnode*)malloc(sizeof(Hnode));
	if(!E.ES.front) exit(OVERFLOW);
	E.ES.front->next=NULL;
	E.ES.front->pre=NULL;
	return OK;
}
Status InitElevator(){//初始化电梯 
	InitElevatorSpace();
	for(int i=0;i<5;i++){
		E.CallCar[i]=0;
	}
	E.State=Idle;
	E.TotalNum=0;
	E.floor=1;
	return OK;
}
Status InitQueueFloor(Queue &Q){//初始化楼层排队队列 
	Q.front=Q.rear=(Qnode*)malloc(sizeof(Qnode));
	if(!Q.front) exit(OVERFLOW);
	Q.front->next=NULL;
	Q.front->pre=NULL;
	return OK;
}
Status IsQueueEmpty(){
	Qnode* p=floor[E.floor].front;
	while(p!=floor[E.floor].rear) return FALSE;
	return OK;
}
Status SomeOnePushNowFloorButton(){
	if(LPW==GoingUp&&FloorButton[E.floor][GoingUp]) return OK;
	else if(LPW==GoingDown&&FloorButton[E.floor][GoingDown]) return OK;
	else return FALSE;	
}
int SomeOnePushButton(){//找出电梯应该去的层数，并返回 
	int i;
	for(i=1;i<6;i++){
		if(FloorButton[i][GoingUp])
			break;
	}
	if(i<6) return i;
	else{
		for(i=5;i>0;i--){
			if(FloorButton[i][GoingDown])
				break;
		}
		if(i>0) return i;
		else return 0;
	} 
}
Status InitFiveQ(){//初始化五层排队队列 
	for(int i=1;i<6;i++){
		InitQueueFloor(floor[i]);
	}
}
Status HasPeopleOut(){
	Hnode* p=(Hnode*)malloc(sizeof(Hnode));
	p=E.ES.front;
	while(p!=E.ES.rear){	
		if(p->next->data.wannafloor==E.floor) {
			return OK;
		}
		p=p->next;
	}
	return FALSE;
}
Status EnQueue(Queue &Q,Human A){//从队尾进队 
	Qnode *p=(Qnode*)malloc(sizeof(Qnode));
	if(!p) exit(OVERFLOW);
	p->data=A;
	p->next=NULL;
	p->pre=Q.rear;
	Q.rear->next=p;
	Q.rear=p;
}
Status DestroyQueue(Queue &Q){//销毁队列Q,所有人从队头出去 
	if(Q.front==Q.rear) return OK;
	while(Q.front!=Q.rear){
		Qnode *p=Q.front->next;
		JoiningMan[J]= p->data;
		J++;
   	 printf("当前%d楼排队的第一个人是%d,他不等了走了\n",E.floor,JoiningMan[J--].name);
		if(Q.rear==p){
			printf("他后面已经没有人了\n");
			Q.rear=Q.front;
		}
		else{
			printf("他后面还有人\n");
			Q.front->next=p->next;
			p->next->pre=Q.front;
		}
		free(p);
	}
	return OK; 
}
Status JudgeComingBehave(Human &A){//A刚来某一层判断行为 
	int i;
	Qnode* p=floor[A.Infloor].front;
	for(i=0;p!=floor[A.Infloor].rear;p=p->next,i++);
	printf("他发现%d楼当前排队的有%d人\n",A.Infloor,i);
	if(i<10){//如果排队人数小于10 就去排队 
		if(floor[A.Infloor].front==floor[A.Infloor].rear){
			printf("没有人真好"); 
		if(A.wannafloor>A.Infloor){
		FloorButton[A.Infloor][GoingUp]=1;
		printf("%d直接站到第一个上去按了向上按钮\n",A.name);			
		}

		else {
			FloorButton[A.Infloor][GoingDown]=1;
			printf("%d直接站到第一个上去按了向下按钮\n",A.name);
		}
	}
	else if(i>0){
		printf("当前楼层有人排队，%d只能站到后面排队\n",A.name);
		if(A.wannafloor>A.Infloor&&FloorButton[A.Infloor][GoingUp]==0){
			FloorButton[A.Infloor][GoingUp]=1;
			printf("%d发现没人按向上按钮，%d去按了向上按钮\n",A.name,A.name);			
		}
		if(A.wannafloor<A.Infloor&&FloorButton[A.Infloor][GoingDown]==0){
			FloorButton[A.Infloor][GoingDown]=1;
			printf("%d发现没人按向下按钮，%d去按了向下按钮\n",A.name,A.name);
		}
	}
	EnQueue(floor[A.Infloor],A);
	return OK;
	}
	else{
		printf("已经十个人了%d这个人直接走了\n",A.name);
		return OK;
	}
}
Status DeQueue(Queue &Q,Human &A){//从队头出队 
	if(Q.front==Q.rear){
		return ERROR;
	}
	Qnode* p=(Qnode*) malloc (sizeof(Qnode));
	p=Q.front->next;
	A = p->data;
    printf("当前%d楼排队的第一个人是%d,他迈步走向电梯\n",E.floor,A.name);
	if(Q.rear==p){
		printf("他后面已经没有人了\n");
		Q.rear=Q.front;
	}
	else{
		printf("他后面还有人\n");
		Q.front->next=p->next;
		p->next->pre=Q.front;
	}
	free(p);
	return OK; 
}
Status JudgeQueueingBehave(){//某一时刻判断队列里人是否需要离开
	Qnode *p=(Qnode*)malloc(sizeof(Qnode));
	for(int i=1;i<6;i++){//遍历五个队列 
//	printf("判断第%d个队列\n",i);
		p=floor[i].front;
		while(p!=floor[i].rear){
			if(timeline-p->next->data.name>=p->next->data.durtime){//找出队列里超过等待时间的，把它去掉 
				if(p==floor[i].front){
				    printf("floor[%d]中排头那个人%d不坐电梯了！\n",i,p->next->data.name);
				    if(floor[i].front->next==floor[i].rear){
				    	printf("lala\n");
				    	floor[i].rear=floor[i].front;
				    	printf("la\n");
					}
				    else{
				    	floor[i].front->next=p->next->next;
					 	floor[i].front->next->pre=floor[i].front;
					}			
				}
				else if(p==floor[i].rear->pre){
					floor[i].rear->pre->next=NULL;
					printf("floor[%d]中队尾那个人%d不坐电梯了！\n",i,p->next->data.name);
				}
				else{
					printf("floor[%d]中间一个人%d不坐电梯了！\n",i,p->next->data.name);
					p->next->next->pre=p;
					p->next=p->next->next;
				}
			}
			p=p->next;
		}
	}
}
Status Push(Human &e){ //无需判断人数是否超过SPACEMAX（最大容纳量），我们在push前判断 
	Hnode *p=(Hnode*)malloc(sizeof(Hnode));
	if(!p) exit(OVERFLOW);
	p->data=e;
	printf("%d走进了电梯\n",e.name);
	E.CallCar[e.wannafloor-1]=1;
	FloorButton[E.floor][GoingUp]=FloorButton[E.floor][GoingDown]=0;
	p->next=NULL;
	p->pre=E.ES.rear;
	E.ES.rear->next=p;
	E.ES.rear=p;
}
Status Pop(){//电梯里出一个人 
	if(E.ES.front==E.ES.rear) {
		return ERROR;
	}
	Hnode* p=(Hnode*)malloc(sizeof(Hnode));
	p=E.ES.front->next;
	while(p->data.wannafloor!=E.floor&&p->next!=E.ES.rear) p=p->next;
	if(p->data.wannafloor==E.floor)
	printf("%d这个人走了出来\n",p->data.name); 
	if(E.ES.rear==p){
		p->pre->next=NULL;
		E.ES.rear=E.ES.front;
	}
	else{
		p->pre->next=p->next;
		p->next->pre=p->pre;
	}
	free(p);
	return OK;
}
Status CreateOnePeople(Human &One){//随机产生一个人节点，并将下次产生人节点的时间赋值nexttime 
	One.name=timeline;
	nexttime=(int)(rand()%101)+200;
	One.durtime=(int)(rand()%601)+8800;
	One.Infloor=(int)(rand()%5+1);
	int t=(rand()%5+1);
	for(;t==One.Infloor;t=(rand()%5+1));
	One.wannafloor=(int)t;	
}
Status ElevatorBehave(){//根据电梯状态判断电梯行为 并更新下0.1s的状态 
	if(E.State==DoorOpening){//状态为                   正在开门时的判断     不会被打断
		if(SomeOnePushNowFloorButton()){
			FloorButton[E.floor][GoingUp]=0;
			FloorButton[E.floor][GoingDown]=0;
		} 
		if(DOT<DoorTime){
			DOT++;
			return PAUSE;
		}
		else if(DOT==DoorTime){
			DOT=0;
			E.State=ElevatorOut;//门开，先让人出 
			printf("电梯门开了\n");
			return OK;
		}
		return ERROR;
	}
	else if(E.State==ElevatorOut){
		if(SomeOnePushNowFloorButton()){
			FloorButton[E.floor][GoingUp]=0;
			FloorButton[E.floor][GoingDown]=0;
		}
		if(HasPeopleOut()==OK){
			if(HIOT<IOTime){
				HIOT++;
				return PAUSE;
			}
			else{
				HIOT=0;
				Pop();
				E.TotalNum--;
				return PAUSE;
			}
		}
		else{
			printf("电梯里没人要出\n");
			E.State=QueueIn;
			ElevatorBehave();
			return OK;
		}
	}
	else if(E.State==QueueIn){
		if(SomeOnePushNowFloorButton()){
			FloorButton[E.floor][GoingUp]=0;
			FloorButton[E.floor][GoingDown]=0;
		}
		if(E.TotalNum>=10){
			printf("已经满员了不能再进人了\n");
			E.State=DoorOpen;//等待4s后直接关门
			DestroyQueue(floor[E.floor]);
			ElevatorBehave();
			return OK;
		}
		else{
			if(IsQueueEmpty()){
				printf("没人排队，直接等待关门\n");
				E.State=DoorOpen;
				ElevatorBehave();
				return OK;
			}
			else{
				if(HIOT<IOTime){
					HIOT++;
					return PAUSE;
				}
				else{
					HIOT=0;
					DeQueue(floor[E.floor],JoiningMan[J]);
					J++;
					Push(JoiningMan[J-1]);
					E.CallCar[JoiningMan[J-1].wannafloor-1]=1;
					return PAUSE;
				}
			}
		} 
	}
	else if(E.State==DoorOpen){//状态为                    关门检测的判断
		if(SomeOnePushNowFloorButton()){
			FloorButton[E.floor][GoingUp]=0;
			FloorButton[E.floor][GoingDown]=0;
			printf("来了个人，再进\n"); 
			E.State=QueueIn;
			ETT=0;
			ElevatorBehave();
			return OK;	
		}
		else if(ETT<CloseTest){
			ETT++;
			return PAUSE;
		}
		else if(ETT==CloseTest){
			ETT=0;
			printf("电梯关门\n");
			E.State=DoorCloseing;
			return OK;
		}
	}
	else if(E.State==DoorCloseing){//状态为                正在关门时的判断 
		if(SomeOnePushNowFloorButton()){
			FloorButton[E.floor][GoingUp]=0;
			FloorButton[E.floor][GoingDown]=0;
			E.State=DoorOpening;
			printf("关门关了%d时间有人来了，再开门\n",DCT);
			DOT=40-DCT;
			DCT=0;
			ElevatorBehave();
			return OK;
		}
		else if(DCT<DoorTime){
			DCT++;
			return PAUSE;
		}
		else if(DCT==DoorTime){ 
			DCT=0;
			if(LPW==GoingUp){
				int i=E.floor;
				if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
					printf("%d层正好又来了人，刚关门又得开门！\n",i);
					E.State=DoorOpening;
					return OK;
				}
				else{
					i++;
					while(i<6){
						if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
							printf("电梯开始加速上升\n"); 
							E.State=SpeedUp;
							return OK;					
						}
					i++;
					}
				}
				i=5;
				while(i>E.floor){
					if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
						printf("电梯开始加速上升\n"); 
						E.State=SpeedUp;
						return OK;
					}
					i--;
				}
				if(i==E.floor){
					if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
						printf("%d层正好又来了人，刚关门又得开门！\n",i);
						E.State=DoorOpening;
						return OK;
					}
				}
				{
					i--;
					while(i>0){
						if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
							printf("电梯开始加速下降\n");
							E.State=SpeedDown;
							return OK;
						}
						i--; 
					}
					E.State=Idle;
					return OK;
				}
			}
			else{
				int i=E.floor;
				if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
					printf("在%d层正好又来了人，刚关门又得开门！\n",i);
					E.State=DoorOpening;
					return OK;
				}
				else{
					i--;
					while(i>0){
						if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
							printf("电梯开始加速下降\n");
							E.State=SpeedDown;
							return OK;
						}
						i--;
					}
				}
				i=1;
				while(i<E.floor){
					if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
						printf("电梯开始加速下降\n");
						E.State=SpeedDown;
						return OK;
					}
					i++;
				}
				if(i==E.floor){
					if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
						printf("在%d层正好又来了人，刚关门又得开门！\n",i);
						E.State=DoorOpening;
						return OK;
					}
				}
				{
					i++;
					while(i<6){
						if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
							printf("电梯开始加速上升\n");
							E.State=SpeedUp;
							return OK;
						}
						i++;
					}
					printf("电梯不动了\n");
					E.State=Idle;
					return OK;
				}
			}
		}
		else return ERROR;
	}
	else if(E.State==SpeedUp){//状态为             加速上升时的判断 
		if(SPUT<Accelerate){
			SPUT++;
			return PAUSE;
		}
		else if(SPUT==Accelerate){
			SPUT=0;
			printf("电梯开始匀速上升！\n");
			E.State=GoingUp;
			return OK;
		}
	}
	else if(E.State==GoingUp){//状态为              匀速上升时的判断 
		LPW=GoingUp;
		if(UT<UpTime){
			UT++;
			return PAUSE;
		}
		else if(UT==UpTime){//判断一下下一个目标 
			UT=0;
			E.floor++;
			printf("电梯来到了%d层\n",E.floor);
			int i=E.floor;
			if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
				E.CallCar[i-1]=FloorButton[i][GoingUp]=FloorButton[i][GoingDown]=0;
				printf("需要在%d层停下，电梯减速！\n",i);
				E.State=SlowUp;
				return OK;
			}
			else{
				i++;
				while(i<6){
				if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
					printf("电梯不需要停，继续匀速上升\n");
					E.State=GoingUp;
					return OK;					
				}
				i++;
				}
			}
			i=5;
			while(i>E.floor){
				if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
					printf("电梯不需要停，继续匀速上升\n");
					E.State=GoingUp;
					return OK;
				}
				i--;
			}
			if(i==E.floor){
				if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
					E.CallCar[i-1]=FloorButton[i][GoingDown]=FloorButton[i][GoingUp]=0;
					printf("需要在%d层停下，电梯减速！\n",i);
					E.State=SlowUp;
					return OK;
				}
			}	
		}
		else return ERROR;
	}
	else if(E.State==SlowUp){//状态为          减速上升时的判断   无法被打断 
		if(SLUT<UpDecelerate){
			SLUT++;
			return PAUSE;
		}
		else if(SLUT==UpDecelerate){
			SLUT=0;
			printf("电梯开门！\n");
			E.State=DoorOpening;
			FloorButton[E.floor][GoingDown]=FloorButton[E.floor][GoingUp]=0;
			E.CallCar[E.floor-1]=0;
			return OK;
		}
		else return ERROR;
	}
	else if(E.State==SpeedDown){//状态为        加速下降时的判断   无法被打断 
		if(SPDT<Accelerate){
			SPDT++;
			return PAUSE;
		}
		else if(SPDT==Accelerate){
			SPDT=0;
			printf("电梯开始匀速下降！\n");
			E.State=GoingDown;
			return OK;
		}
		else return ERROR;
	}
	else if(E.State==GoingDown){//状态为         匀速下降的判断 
		LPW=GoingDown;
		if(DT<DownTime){
			DT++;
			return PAUSE;
		}
		else if(DT==DownTime){
			DT=0;
			E.floor--;
			printf("电梯来到了%d层\n",E.floor); 
			int i=E.floor;
			if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
				E.CallCar[i-1]=FloorButton[i][GoingDown]=FloorButton[i][GoingUp]=0;
				printf("在%d层需要停下，电梯减速下降！\n",i);
				E.State=SlowDown;
				return OK;
			}
			else{
				i--;
				while(i>0){
					if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
						printf("电梯不需要停，继续匀速下降\n");
						E.State=GoingDown;
						return OK;
					}
					i--;
				}
			}
			i=1;
			while(i<E.floor){
				if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
					printf("电梯不需要停，继续匀速下降\n");
					E.State=GoingDown;
					return OK;
				}
				i++;
			}
			if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
				E.CallCar[i-1]=FloorButton[i][GoingUp]=FloorButton[i][GoingDown]=0;
				printf("在%d层需要停下，电梯减速下降！\n",i);
				E.State=SlowDown;
				return OK;
			}
		}
		else return ERROR;
	}
	if(E.State==SlowDown){//状态为          减速下降时的判断   无法被打断 
			if(SLDT<DownDecelerate){
			SLDT++;
			return PAUSE;
			}
			else if(SLDT==DownDecelerate){
				SLDT=0;
				printf("电梯开门！\n");
				E.State=DoorOpening;
				FloorButton[E.floor][GoingDown]=FloorButton[E.floor][GoingUp]=0;
				E.CallCar[E.floor-1]=0;
				return OK;
			}
			else return ERROR;
	}
	else if(E.State==Idle){//状态为                闲置时的判断
		int flag=SomeOnePushButton();
		if(flag){
			EWT=0;
			if(E.floor==flag){
				printf("当前楼层有人来了，开门！\n");
				E.State=DoorOpening;
				ElevatorBehave();
				return OK;
			}
			else if(E.floor>flag){
				printf("电梯发现下面楼层有人要坐电梯，向下加速\n");
				E.State=SpeedDown;
	//			E.CallCar[flag-1]=1;
				ElevatorBehave();
				return OK;
			}
			else if(E.floor<flag){
				E.State=SpeedUp;
				printf("电梯发现上面楼层有人要坐电梯，向上加速\n");
//				E.CallCar[flag-1]=1;
				ElevatorBehave();
				return OK;
			}
		}
		else if(EWT<OverTime){
			EWT++;
			return PAUSE;
		}
		else if(EWT==OverTime){
			EWT=0;
			if(E.floor==1){
				printf("电梯Stop！\n");
				E.CallCar[0]=0;
				E.State=Stop;
				return OK;
			}
			else{
				printf("电梯加速下降准备回到一楼！\n");
				E.State=SpeedDown;
				E.CallCar[0]=1;
				return OK;
			}
		}
		else return ERROR;	
	}
	else if(E.State==Stop){
		int flag=SomeOnePushButton();
		if(flag){
			if(flag==1){
				E.State=DoorOpening;
				ElevatorBehave();
				return OK;
			}
			else{
				E.State=SpeedUp;
				E.CallCar[flag-1]=1;
				ElevatorBehave();
				return OK;
			}
		}
	} 
}

int main(){
	int i=0,j=1,t=0;Human A[100000];
	srand((unsigned)time(NULL));
	InitElevator();
	InitFiveQ();
	scanf("%d",&endtime);
	while(endtime<=0){
		printf("请重新输入！\n");
		scanf("%d",&endtime);
	} 
	CreateOnePeople(A[i]);
	printf("新来一个人%d这个人在%d，想去%d层，最多等%d\n",A[i].name,A[i].Infloor,A[i].wannafloor,A[i].durtime);
	printf("下一个人在%d后到来\n",nexttime); 
	JudgeComingBehave(A[i]);
	i++;
	while(timeline<endtime){
		if(timeline-A[i-1].name>=nexttime){
			CreateOnePeople(A[i]);
			printf("下一个人来了，%d这个人在%d，想去%d层，最多等%d\n",A[i].name,A[i].Infloor,A[i].wannafloor,A[i].durtime);
			JudgeComingBehave(A[i]);
			i++;
		}
		JudgeQueueingBehave();
		ElevatorBehave();
		timeline++;
	}
} 






