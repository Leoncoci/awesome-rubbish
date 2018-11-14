#include<stdio.h> 
#include<stdlib.h>
#include<time.h>
//����

#define SPACEMAX 10 
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define OVERFLOW -2
#define PAUSE 0
//����״̬���� 

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
//����״̬����

#define CloseTest 40
#define OverTime 300
#define Accelerate 15
#define UpTime 51
#define DownTime 61
#define UpDecelerate 14
#define DownDecelerate 23
#define DoorTime 20
#define IOTime 25
//���ݶ�̬��������ʱ��

typedef int Status;
typedef struct{//�����˵�Ԫ�� 
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

int timeline;//����ʱ���� ��λ0.1����˼��time=100��ʾ����10s 
int endtime;//�������ʱ�� 
int nexttime;//��һ���˵������ʱ�� 
int SLDT;//�����½�ʱ�� 
int SLUT;//��������ʱ��
int SPDT;//�����½�ʱ��
int SPUT;//��������ʱ�� 
int UT;//����ʱ�� 
int DT;//�½�ʱ�� 
int EWT;//���ݵȴ�ʱ��
int DOT; //���ݿ���ʱ��
int DCT;//���ݹ���ʱ�� 
int ETT;//���ݿ���ʱ���Լ��ʱ�� 
int HIOT;//ÿ���˽���ʱ�� 
int Aim;//����Ŀ��¥�� 
Elevator E;
Status LPW;     //���һ���˰����ϻ����� 
Queue floor[6];//Ϊ�˱�������floor[0]��ʹ��
Status FloorButton[6][2];
Human JoiningMan[100000]; 
int J,O;

Status InitElevatorSpace(){//��ʼ�����ݶ��пռ� 
	E.ES.front=E.ES.rear=(Hnode*)malloc(sizeof(Hnode));
	if(!E.ES.front) exit(OVERFLOW);
	E.ES.front->next=NULL;
	E.ES.front->pre=NULL;
	return OK;
}
Status InitElevator(){//��ʼ������ 
	InitElevatorSpace();
	for(int i=0;i<5;i++){
		E.CallCar[i]=0;
	}
	E.State=Idle;
	E.TotalNum=0;
	E.floor=1;
	return OK;
}
Status InitQueueFloor(Queue &Q){//��ʼ��¥���ŶӶ��� 
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
int SomeOnePushButton(){//�ҳ�����Ӧ��ȥ�Ĳ����������� 
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
Status InitFiveQ(){//��ʼ������ŶӶ��� 
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
Status EnQueue(Queue &Q,Human A){//�Ӷ�β���� 
	Qnode *p=(Qnode*)malloc(sizeof(Qnode));
	if(!p) exit(OVERFLOW);
	p->data=A;
	p->next=NULL;
	p->pre=Q.rear;
	Q.rear->next=p;
	Q.rear=p;
}
Status DestroyQueue(Queue &Q){//���ٶ���Q,�����˴Ӷ�ͷ��ȥ 
	if(Q.front==Q.rear) return OK;
	while(Q.front!=Q.rear){
		Qnode *p=Q.front->next;
		JoiningMan[J]= p->data;
		J++;
   	 printf("��ǰ%d¥�Ŷӵĵ�һ������%d,������������\n",E.floor,JoiningMan[J--].name);
		if(Q.rear==p){
			printf("�������Ѿ�û������\n");
			Q.rear=Q.front;
		}
		else{
			printf("�����滹����\n");
			Q.front->next=p->next;
			p->next->pre=Q.front;
		}
		free(p);
	}
	return OK; 
}
Status JudgeComingBehave(Human &A){//A����ĳһ���ж���Ϊ 
	int i;
	Qnode* p=floor[A.Infloor].front;
	for(i=0;p!=floor[A.Infloor].rear;p=p->next,i++);
	printf("������%d¥��ǰ�Ŷӵ���%d��\n",A.Infloor,i);
	if(i<10){//����Ŷ�����С��10 ��ȥ�Ŷ� 
		if(floor[A.Infloor].front==floor[A.Infloor].rear){
			printf("û�������"); 
		if(A.wannafloor>A.Infloor){
		FloorButton[A.Infloor][GoingUp]=1;
		printf("%dֱ��վ����һ����ȥ�������ϰ�ť\n",A.name);			
		}

		else {
			FloorButton[A.Infloor][GoingDown]=1;
			printf("%dֱ��վ����һ����ȥ�������°�ť\n",A.name);
		}
	}
	else if(i>0){
		printf("��ǰ¥�������Ŷӣ�%dֻ��վ�������Ŷ�\n",A.name);
		if(A.wannafloor>A.Infloor&&FloorButton[A.Infloor][GoingUp]==0){
			FloorButton[A.Infloor][GoingUp]=1;
			printf("%d����û�˰����ϰ�ť��%dȥ�������ϰ�ť\n",A.name,A.name);			
		}
		if(A.wannafloor<A.Infloor&&FloorButton[A.Infloor][GoingDown]==0){
			FloorButton[A.Infloor][GoingDown]=1;
			printf("%d����û�˰����°�ť��%dȥ�������°�ť\n",A.name,A.name);
		}
	}
	EnQueue(floor[A.Infloor],A);
	return OK;
	}
	else{
		printf("�Ѿ�ʮ������%d�����ֱ������\n",A.name);
		return OK;
	}
}
Status DeQueue(Queue &Q,Human &A){//�Ӷ�ͷ���� 
	if(Q.front==Q.rear){
		return ERROR;
	}
	Qnode* p=(Qnode*) malloc (sizeof(Qnode));
	p=Q.front->next;
	A = p->data;
    printf("��ǰ%d¥�Ŷӵĵ�һ������%d,�������������\n",E.floor,A.name);
	if(Q.rear==p){
		printf("�������Ѿ�û������\n");
		Q.rear=Q.front;
	}
	else{
		printf("�����滹����\n");
		Q.front->next=p->next;
		p->next->pre=Q.front;
	}
	free(p);
	return OK; 
}
Status JudgeQueueingBehave(){//ĳһʱ���ж϶��������Ƿ���Ҫ�뿪
	Qnode *p=(Qnode*)malloc(sizeof(Qnode));
	for(int i=1;i<6;i++){//����������� 
//	printf("�жϵ�%d������\n",i);
		p=floor[i].front;
		while(p!=floor[i].rear){
			if(timeline-p->next->data.name>=p->next->data.durtime){//�ҳ������ﳬ���ȴ�ʱ��ģ�����ȥ�� 
				if(p==floor[i].front){
				    printf("floor[%d]����ͷ�Ǹ���%d���������ˣ�\n",i,p->next->data.name);
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
					printf("floor[%d]�ж�β�Ǹ���%d���������ˣ�\n",i,p->next->data.name);
				}
				else{
					printf("floor[%d]�м�һ����%d���������ˣ�\n",i,p->next->data.name);
					p->next->next->pre=p;
					p->next=p->next->next;
				}
			}
			p=p->next;
		}
	}
}
Status Push(Human &e){ //�����ж������Ƿ񳬹�SPACEMAX���������������������pushǰ�ж� 
	Hnode *p=(Hnode*)malloc(sizeof(Hnode));
	if(!p) exit(OVERFLOW);
	p->data=e;
	printf("%d�߽��˵���\n",e.name);
	E.CallCar[e.wannafloor-1]=1;
	FloorButton[E.floor][GoingUp]=FloorButton[E.floor][GoingDown]=0;
	p->next=NULL;
	p->pre=E.ES.rear;
	E.ES.rear->next=p;
	E.ES.rear=p;
}
Status Pop(){//�������һ���� 
	if(E.ES.front==E.ES.rear) {
		return ERROR;
	}
	Hnode* p=(Hnode*)malloc(sizeof(Hnode));
	p=E.ES.front->next;
	while(p->data.wannafloor!=E.floor&&p->next!=E.ES.rear) p=p->next;
	if(p->data.wannafloor==E.floor)
	printf("%d��������˳���\n",p->data.name); 
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
Status CreateOnePeople(Human &One){//�������һ���˽ڵ㣬�����´β����˽ڵ��ʱ�丳ֵnexttime 
	One.name=timeline;
	nexttime=(int)(rand()%101)+200;
	One.durtime=(int)(rand()%601)+8800;
	One.Infloor=(int)(rand()%5+1);
	int t=(rand()%5+1);
	for(;t==One.Infloor;t=(rand()%5+1));
	One.wannafloor=(int)t;	
}
Status ElevatorBehave(){//���ݵ���״̬�жϵ�����Ϊ ��������0.1s��״̬ 
	if(E.State==DoorOpening){//״̬Ϊ                   ���ڿ���ʱ���ж�     ���ᱻ���
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
			E.State=ElevatorOut;//�ſ��������˳� 
			printf("�����ſ���\n");
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
			printf("������û��Ҫ��\n");
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
			printf("�Ѿ���Ա�˲����ٽ�����\n");
			E.State=DoorOpen;//�ȴ�4s��ֱ�ӹ���
			DestroyQueue(floor[E.floor]);
			ElevatorBehave();
			return OK;
		}
		else{
			if(IsQueueEmpty()){
				printf("û���Ŷӣ�ֱ�ӵȴ�����\n");
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
	else if(E.State==DoorOpen){//״̬Ϊ                    ���ż����ж�
		if(SomeOnePushNowFloorButton()){
			FloorButton[E.floor][GoingUp]=0;
			FloorButton[E.floor][GoingDown]=0;
			printf("���˸��ˣ��ٽ�\n"); 
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
			printf("���ݹ���\n");
			E.State=DoorCloseing;
			return OK;
		}
	}
	else if(E.State==DoorCloseing){//״̬Ϊ                ���ڹ���ʱ���ж� 
		if(SomeOnePushNowFloorButton()){
			FloorButton[E.floor][GoingUp]=0;
			FloorButton[E.floor][GoingDown]=0;
			E.State=DoorOpening;
			printf("���Ź���%dʱ���������ˣ��ٿ���\n",DCT);
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
					printf("%d�������������ˣ��չ����ֵÿ��ţ�\n",i);
					E.State=DoorOpening;
					return OK;
				}
				else{
					i++;
					while(i<6){
						if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
							printf("���ݿ�ʼ��������\n"); 
							E.State=SpeedUp;
							return OK;					
						}
					i++;
					}
				}
				i=5;
				while(i>E.floor){
					if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
						printf("���ݿ�ʼ��������\n"); 
						E.State=SpeedUp;
						return OK;
					}
					i--;
				}
				if(i==E.floor){
					if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
						printf("%d�������������ˣ��չ����ֵÿ��ţ�\n",i);
						E.State=DoorOpening;
						return OK;
					}
				}
				{
					i--;
					while(i>0){
						if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
							printf("���ݿ�ʼ�����½�\n");
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
					printf("��%d�������������ˣ��չ����ֵÿ��ţ�\n",i);
					E.State=DoorOpening;
					return OK;
				}
				else{
					i--;
					while(i>0){
						if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
							printf("���ݿ�ʼ�����½�\n");
							E.State=SpeedDown;
							return OK;
						}
						i--;
					}
				}
				i=1;
				while(i<E.floor){
					if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
						printf("���ݿ�ʼ�����½�\n");
						E.State=SpeedDown;
						return OK;
					}
					i++;
				}
				if(i==E.floor){
					if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
						printf("��%d�������������ˣ��չ����ֵÿ��ţ�\n",i);
						E.State=DoorOpening;
						return OK;
					}
				}
				{
					i++;
					while(i<6){
						if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
							printf("���ݿ�ʼ��������\n");
							E.State=SpeedUp;
							return OK;
						}
						i++;
					}
					printf("���ݲ�����\n");
					E.State=Idle;
					return OK;
				}
			}
		}
		else return ERROR;
	}
	else if(E.State==SpeedUp){//״̬Ϊ             ��������ʱ���ж� 
		if(SPUT<Accelerate){
			SPUT++;
			return PAUSE;
		}
		else if(SPUT==Accelerate){
			SPUT=0;
			printf("���ݿ�ʼ����������\n");
			E.State=GoingUp;
			return OK;
		}
	}
	else if(E.State==GoingUp){//״̬Ϊ              ��������ʱ���ж� 
		LPW=GoingUp;
		if(UT<UpTime){
			UT++;
			return PAUSE;
		}
		else if(UT==UpTime){//�ж�һ����һ��Ŀ�� 
			UT=0;
			E.floor++;
			printf("����������%d��\n",E.floor);
			int i=E.floor;
			if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
				E.CallCar[i-1]=FloorButton[i][GoingUp]=FloorButton[i][GoingDown]=0;
				printf("��Ҫ��%d��ͣ�£����ݼ��٣�\n",i);
				E.State=SlowUp;
				return OK;
			}
			else{
				i++;
				while(i<6){
				if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
					printf("���ݲ���Ҫͣ��������������\n");
					E.State=GoingUp;
					return OK;					
				}
				i++;
				}
			}
			i=5;
			while(i>E.floor){
				if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
					printf("���ݲ���Ҫͣ��������������\n");
					E.State=GoingUp;
					return OK;
				}
				i--;
			}
			if(i==E.floor){
				if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
					E.CallCar[i-1]=FloorButton[i][GoingDown]=FloorButton[i][GoingUp]=0;
					printf("��Ҫ��%d��ͣ�£����ݼ��٣�\n",i);
					E.State=SlowUp;
					return OK;
				}
			}	
		}
		else return ERROR;
	}
	else if(E.State==SlowUp){//״̬Ϊ          ��������ʱ���ж�   �޷������ 
		if(SLUT<UpDecelerate){
			SLUT++;
			return PAUSE;
		}
		else if(SLUT==UpDecelerate){
			SLUT=0;
			printf("���ݿ��ţ�\n");
			E.State=DoorOpening;
			FloorButton[E.floor][GoingDown]=FloorButton[E.floor][GoingUp]=0;
			E.CallCar[E.floor-1]=0;
			return OK;
		}
		else return ERROR;
	}
	else if(E.State==SpeedDown){//״̬Ϊ        �����½�ʱ���ж�   �޷������ 
		if(SPDT<Accelerate){
			SPDT++;
			return PAUSE;
		}
		else if(SPDT==Accelerate){
			SPDT=0;
			printf("���ݿ�ʼ�����½���\n");
			E.State=GoingDown;
			return OK;
		}
		else return ERROR;
	}
	else if(E.State==GoingDown){//״̬Ϊ         �����½����ж� 
		LPW=GoingDown;
		if(DT<DownTime){
			DT++;
			return PAUSE;
		}
		else if(DT==DownTime){
			DT=0;
			E.floor--;
			printf("����������%d��\n",E.floor); 
			int i=E.floor;
			if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
				E.CallCar[i-1]=FloorButton[i][GoingDown]=FloorButton[i][GoingUp]=0;
				printf("��%d����Ҫͣ�£����ݼ����½���\n",i);
				E.State=SlowDown;
				return OK;
			}
			else{
				i--;
				while(i>0){
					if(E.CallCar[i-1]||FloorButton[i][GoingDown]){
						printf("���ݲ���Ҫͣ�����������½�\n");
						E.State=GoingDown;
						return OK;
					}
					i--;
				}
			}
			i=1;
			while(i<E.floor){
				if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
					printf("���ݲ���Ҫͣ�����������½�\n");
					E.State=GoingDown;
					return OK;
				}
				i++;
			}
			if(E.CallCar[i-1]||FloorButton[i][GoingUp]){
				E.CallCar[i-1]=FloorButton[i][GoingUp]=FloorButton[i][GoingDown]=0;
				printf("��%d����Ҫͣ�£����ݼ����½���\n",i);
				E.State=SlowDown;
				return OK;
			}
		}
		else return ERROR;
	}
	if(E.State==SlowDown){//״̬Ϊ          �����½�ʱ���ж�   �޷������ 
			if(SLDT<DownDecelerate){
			SLDT++;
			return PAUSE;
			}
			else if(SLDT==DownDecelerate){
				SLDT=0;
				printf("���ݿ��ţ�\n");
				E.State=DoorOpening;
				FloorButton[E.floor][GoingDown]=FloorButton[E.floor][GoingUp]=0;
				E.CallCar[E.floor-1]=0;
				return OK;
			}
			else return ERROR;
	}
	else if(E.State==Idle){//״̬Ϊ                ����ʱ���ж�
		int flag=SomeOnePushButton();
		if(flag){
			EWT=0;
			if(E.floor==flag){
				printf("��ǰ¥���������ˣ����ţ�\n");
				E.State=DoorOpening;
				ElevatorBehave();
				return OK;
			}
			else if(E.floor>flag){
				printf("���ݷ�������¥������Ҫ�����ݣ����¼���\n");
				E.State=SpeedDown;
	//			E.CallCar[flag-1]=1;
				ElevatorBehave();
				return OK;
			}
			else if(E.floor<flag){
				E.State=SpeedUp;
				printf("���ݷ�������¥������Ҫ�����ݣ����ϼ���\n");
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
				printf("����Stop��\n");
				E.CallCar[0]=0;
				E.State=Stop;
				return OK;
			}
			else{
				printf("���ݼ����½�׼���ص�һ¥��\n");
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
		printf("���������룡\n");
		scanf("%d",&endtime);
	} 
	CreateOnePeople(A[i]);
	printf("����һ����%d�������%d����ȥ%d�㣬����%d\n",A[i].name,A[i].Infloor,A[i].wannafloor,A[i].durtime);
	printf("��һ������%d����\n",nexttime); 
	JudgeComingBehave(A[i]);
	i++;
	while(timeline<endtime){
		if(timeline-A[i-1].name>=nexttime){
			CreateOnePeople(A[i]);
			printf("��һ�������ˣ�%d�������%d����ȥ%d�㣬����%d\n",A[i].name,A[i].Infloor,A[i].wannafloor,A[i].durtime);
			JudgeComingBehave(A[i]);
			i++;
		}
		JudgeQueueingBehave();
		ElevatorBehave();
		timeline++;
	}
} 






