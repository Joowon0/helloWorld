//FillBlank2D
#include<stdio.h>
#include<stdlib.h>
#include<Time.h>
#define HUMAN 0
#define COMPUTER 1
#define GIVEUP	-1
#define MAX 10000
#define W 10
#define H 10
#define Change_Turn(x) ((x) = ((x+1)%2))

typedef int State_Type[H][W];

typedef struct{
	int Col;
	int Row;
}Point;

State_Type Base_Block[1024] = { 0 };
State_Type Block[] = {
	{ { 1, 1 }, { 0, 1, 1 } },
	{ { 1, 1, 1, 1 } },
	{ { 1, 1 }, { 1, 1 } },
	{ { 0, 1, 1 }, { 1, 1 } },
	{ { 1 }, { 1, 1, 1 } },
	{ { 0, 0, 1 }, { 1, 1, 1 } },
	{ { 0, 1 }, { 1, 1, 1 } },
};
State_Type Full_Choice[MAX];
int Poss_Choice[MAX];
State_Type Choice_Order[W*H] ;
int Order = 1;
int Num_Blk = sizeof(Block) / sizeof(*Block);
int Num_Poss_Cho = MAX;
int Num_Full_Cho,Num_Base_Cho ;
char* Message[10] = {
	"This choice is not possible.......\n",
	"Input X Failed......\n",
	"Invalid X ranges......\n",
	"Input Y Failed ......\n",
	"Invalid Y ranges ......\n",
	"\n\t\t\tThis turn is first........\n\n",
	"\n\t\t\tTabie is Roll_Back.........\n\n",
};

//Print_State_VF: 주어진 게임 테이블을 출력
void Print_State_VF(State_Type st){
	int Col, Row;
	printf("\nCurrent state:\n");
	printf("\t\t\t  ");
	for (Col = 0; Col < W; Col++)
		printf("%2d", Col);
	printf("\n");
	for (Row = 0; Row < H; Row++){
		printf("\t\t\t%2d", Row);
		for (Col = 0; Col < W; Col++){
			if (st[Row][Col] >= 1){
				if (st[Row][Col] == 2)
					printf("▦");
				else if (st[Row][Col]==1)
					printf("■");
			}
			else
				printf("□");
		}
		printf("\n");
	}
}
// Print_Block_VF : 주어진 블록을 출력 
//
//			디버깅용
//
void Print_Block_VF(State_Type blk){
	int Col, Row;
	printf("\t\t\t  ");
	for (Col = 0; Col < W; Col++)
		printf("%2d", Col);
	printf("\n");
	for (Row = 0; Row < H; Row++){
		printf("\t\t\t%2d ", Row);
		for (Col = 0; Col < W; Col++)
			if (blk[Row][Col] == 1)
				printf("■");
			else
				printf("□");
		printf("\n");
	}
}
void Copy_VF(State_Type dst, State_Type src){
	int count = 0;
	int Col, Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (src[Row][Col] == 1 || dst[Row][Col] == 1)
				dst[Row][Col] = src[Row][Col];

}

int Num_Of_One_IF(State_Type st){
	int Row, Col, check = 0;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (st[Row][Col] != 0)
				check++;
	return check;
}

void Del_Selected_Blk_VF(State_Type st ,int  choice){
	int Row, Col;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++){
			if (Full_Choice[choice][Row][Col] >= 1 )
				st[Row][Col] = 0;
		}
}
int Overlap_One_BF(State_Type s1, State_Type s2){
	int Col, Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if ((s1[Row][Col] != 0 && s2[Row][Col]) != 0)
				return 1;
	return 0;
}
void Remake_Poss_Choice(State_Type st){
	int b_i ;
	Num_Poss_Cho = 0;
	for (b_i = 0; b_i < Num_Full_Cho; b_i++)
		if (!Overlap_One_BF(st, Full_Choice[b_i]))
			Poss_Choice[Num_Poss_Cho++] = b_i;
}

int Roll_Back_IF(State_Type st){
	if (Order >= 2){
		Order -= 3;
		Copy_VF(st, Choice_Order[Order++]);
		Remake_Poss_Choice(st);
		Print_State_VF(st);
		printf("%s", Message[6]);
		
	}
	else
		printf("%s", Message[5]);

	if (Order == 0){
		Order = 1;
		return -1;
	}
	return 1;
}

void Save_Choice_VF(State_Type st){
	int Row, Col;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			Choice_Order[Order][Row][Col] = st[Row][Col];
	Order++;
}	

void Or_VF(State_Type s1, State_Type s2, State_Type Result){
	int Col, Row = 0;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (s1[Row][Col] != 0 || s2[Row][Col] != 0)
				Result[Row][Col] = 1;
			else
				Result[Row][Col] = 0;
}
//Win_IF : 주어진 게임테이블의 승리여부를 판단.
int Win_IF(State_Type st, time_t Start_Time){
	int Ret_Value, i;
	State_Type Tmp = { 0 };
//	if (time(NULL) - Start_Time >= 5)
//		return -1; // Timeover --> unknown
	for (i = 0; i < Num_Poss_Cho; i++){
		if (!Overlap_One_BF(st, Full_Choice[Poss_Choice[i]])){
			Or_VF(st, Full_Choice[Poss_Choice[i]], Tmp);
			Ret_Value = Win_IF(Tmp, Start_Time);
			if (Ret_Value == -1)
				return -1; // Timeover --> unknown
			else if (Ret_Value == 0)
				return 1; // true
		}
	}
	return 0; // false
}
//Best_Choice_IF : 주어진 게임테이블에서 승리방법 반환, 승리방법 없을 경우 G_POS반환
int Best_Choice_IF(State_Type st){
	int Ret_Value, i;
	State_Type Tmp = { 0 };
	time_t Start_Time = time(NULL);
	for (i = 0; i < Num_Poss_Cho; i++){
		Or_VF(st, Full_Choice[Poss_Choice[i]], Tmp);
		Ret_Value = Win_IF(Tmp, Start_Time);
		if (Ret_Value == -1)
			return -1; // Timeover --> unknown
		else if (Ret_Value == 0)
			return Poss_Choice[i]; // return Full_Choice_id
	}
	return -1;
}

// Computer_Choice_IF : 컴퓨터가 최선의 방법을 선택 반환
int Computer_Choice_IF(State_Type st){
	int best_c = Best_Choice_IF(st);
	if (best_c == -1)
		return Poss_Choice[rand()%Num_Poss_Cho];
	
	else
		return best_c;
}

void Make_Block_by_Points_VF(Point* _p,int P_N,State_Type Result){
	int i = 0;
	for (i = 0; i < P_N; i++)
		Result[_p[i].Row][_p[i].Col] = 1;
}
int Equal_BF(State_Type s1, State_Type s2){
	int Col,Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (s1[Row][Col] != s2[Row][Col])
				return 0;
	return 1;
}
int Find_Poss_Choice_IF(State_Type A){
	int i;
	for (i = 0; i <= Num_Poss_Cho; i++)
		if (Equal_BF(A, Full_Choice[Poss_Choice[i]]))
			return Poss_Choice[i];
	
	return -1;
}
int Valid_IF(State_Type st, Point* _p,int P_N){
	int c_i;
	State_Type Tmp = { 0 };
	Make_Block_by_Points_VF(_p, P_N, Tmp);
	if((c_i=Find_Poss_Choice_IF(Tmp))==-1)
		return -10;
	else
		return c_i;
}
int Get_Choice_IF(State_Type st,Point* _p){
	
	char buffer[MAX];
	int i=0,n1 = 0, n2 = 0,Point_Num=0;
	gets(buffer);
	for (i = 0; !(sscanf(buffer + n1, "%d%d%n", &_p[i].Col, &_p[i].Row, &n2) != 2); i++){
		if (!(0 <= _p[i].Col&&_p[i].Col < W))
			return -8;
		if (!(0 <= _p[i].Row&&_p[i].Row < H))
			return -6;
		n1 += n2;
	}

	Point_Num = i;

	if (_p[0].Col == -1)
		return -1;
	else if (_p[0].Col == -2)
		return -2;

	return Valid_IF(st, _p,Point_Num);
}
//Human_Choice_IF : 사용자로부터 선택을 입력 받음. 잘못된 선택일 경우 다시 입력,
//				 -1이면 게임을 포기
int Human_Choice_IF(State_Type st){

	int V;
	Point points[W*H];
	do{
		int n = 0, size = 0;
		printf("Input 4 Point ( Roll_Back : -2 /// Give up : -1 ///  Type Ex: Col Row ) \n");
		V = Get_Choice_IF(st,points);
		if (-2<= V &&V <= -1)
			return V;
		else if (V < -2)
			printf("%s", Message[V + 10]);
		
	} while (!(V >= 0));

	return V;
}

//Set_Turn_IF : 사용자로부터 누가 먼저 시작할지 여부를 입력받아 그 순서를 반환.
int Set_Turn_IF(){
	int turn = -1;
	while (!(turn == 0 || turn == 1)){
		printf("Turm(0-human or 1-computer) >> ");
		scanf("%d", &turn);
	}
	getchar();
	return turn;
}
void Rotation_Block_90_VF(State_Type dst){
	int Col,Row,i=0,tmp;
	int Max_L;

	Max_L = W > H ? H : W;
	for (Row = 0; Row < Max_L; Row++)
		for (Col = Row + 1; Col < Max_L; Col++){
				tmp = dst[Row][Col];
				dst[Row][Col] = dst[Col][Row];
				dst[Col][Row] = tmp;
			}
	for (Row = 0; Row < Max_L; Row++)
		for (Col = 0; Col < Max_L / 2; Col++){
				tmp = dst[Row][Col];
				dst[Row][Col] = dst[Row][Max_L - 1 - Col];
				dst[Row][Max_L - 1 - Col] = tmp;
			}
}
int Duplication_in_Full_Choice_BF(State_Type A){
	int i;
	if (Num_Full_Cho == -1)
		return 0;
	for (i = 0; i <= Num_Full_Cho; i++){
		if (Equal_BF(A, Full_Choice[i]))
			return 1;
	}
	return 0;
}

void Make_Base_Block_VF(){
	int b_i,count,NumofOne;
	Num_Base_Cho = 0;
	for (b_i=0; b_i < Num_Blk; b_i++){
		State_Type Tmp = { 0 };
		NumofOne = Num_Of_One_IF(Block[b_i]);
		Copy_VF(Base_Block[Num_Base_Cho], Block[b_i]);
		Copy_VF(Tmp, Block[b_i]);
		for (count = 1; count <= 3; count++){
			Rotation_Block_90_VF(Tmp);
			Copy_VF(Base_Block[++Num_Base_Cho], Tmp);
		}
		Num_Base_Cho++;
	}
}

void Paste_VF(State_Type dst, int D_row, int D_col, State_Type src){
	int S_row,S_col,col;
	for (S_row = 0; S_row < H; D_row++, S_row++)
		if (0 <= D_row&&D_row < H )
			for (col=D_col, S_col = 0; S_col<W; col++, S_col++)
				if (0 <= col&&col < W )
					dst[D_row][col] = src[S_row][S_col];
}

// Make_Full_Choice_VF : 주어진 블록과 게임테이블 크기로 가능한 모든 선택을 만들어 배열 Full_Choice 에 저장.

void Make_Full_Choice_VF(){
	int b_i, NofState, row, col;
	Num_Full_Cho = -1;
	Num_Poss_Cho = 0;
	Make_Base_Block_VF();

	for (b_i = 0; b_i < Num_Base_Cho; b_i++){
		NofState = Num_Of_One_IF(Base_Block[b_i]);
		for (row = -H + 1; row < H; row++)
			for (col = -W + 1; col < W; col++){
				State_Type Tmp = { 0 };
				Paste_VF(Tmp, row, col, Base_Block[b_i]);
				if (Num_Of_One_IF(Tmp) == NofState){
					if (!Duplication_in_Full_Choice_BF(Tmp)){
						Poss_Choice[Num_Poss_Cho++] = ++Num_Full_Cho;
						Copy_VF(Full_Choice[Num_Full_Cho], Tmp);
					}
				}
			}
	}
}
void Add_Choice_Block_to_State_VF(State_Type st, int S){
	int Row, Col;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (Full_Choice[S][Row][Col] == 1)
				st[Row][Col] = 2;
		
}
void Modify_State_VF(State_Type st){
	int Row, Col;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (st[Row][Col] == 2)
				st[Row][Col] = 1;

}

int main()
{
	int turn;
	State_Type	Table = { 0 };
	int S = 0;
	srand((unsigned)time(NULL));
	turn = Set_Turn_IF();
	Make_Full_Choice_VF();
	Print_State_VF(Table);
	while (Num_Poss_Cho != 0 && S != -1){
		Modify_State_VF(Table);
		if (turn == HUMAN)
			S = Human_Choice_IF(Table);

		else if (turn == COMPUTER)
			S = Computer_Choice_IF(Table);
		
		if (S >= 0){
			printf("%d\n", Order);
			Add_Choice_Block_to_State_VF(Table, S);
			Save_Choice_VF(Table);
			Print_State_VF(Table);
			Remake_Poss_Choice(Table);
			Change_Turn(turn);
		}
		else if (S == -2)
			if (Roll_Back_IF(Table) == -1)
				Change_Turn(turn);
	}
	if (turn == HUMAN)
		printf("\nCumputer Win\n");
	else
		printf("\nHuman Win\n");

	return 0;
}
