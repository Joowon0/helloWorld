//FillBlank2

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define HUMAN 0
#define COMPUTER 1
#define GIVEUP	-1
#define MAX 1024
int sz_state = 0;
char* Message[5] = {
	"Input BID Failed......\n",
	"Invalid BID ranges......\n",
	"Input POS Failed ......\n",
	"Invalid POS ranges ......\n",
	""
};
typedef int state_type[32];

state_type block[] = {
	{0,0,1,1},						//1001 1000 0000 0000
	{1,0,1},						//1101 0000 0000 0000
	{0,1,0,1},						//1010 1000 0000 0000
};
int num_blk = sizeof(block) / sizeof(*block);
state_type choice[MAX];
int num_cho;
int Num_Of_One_IF(state_type st){
	int i, check = 0;
	for (i = 0; i < sz_state; i++)
		if (st[i] == 1)
			check++;
	return check;
}

void Insert_VF(state_type A, state_type B){
	for (int i = 0; i < sz_state; i++)
		A[i] = B[i];
}
int Overlap_One_BF(state_type A, state_type B){
	int i = 0;
	for (i = 0; i < sz_state; i++)
		if ((A[i] * B[i]) == 1)
			return 1;
	return 0;
}
void Or_VF(state_type A, state_type B, state_type result){
	int i = 0;
	for (i = 0; i < sz_state; i++)
		if (A[i] == 1 || B[i] == 1)
			result[i] = 1;
		else
			result[i] = 0;
}

void L_Shift_VF(state_type A, int n,state_type result){
	int i, j;
	Insert_VF(result, A);
	for (j = n, i = 0; j < sz_state; i++, j++)
		result[i] = result[j];
	for (i = 1; i <= n;i++)
		result[sz_state - i] = 0;
}

void R_Shift_VF(state_type A, int n, state_type result){
	int i, j;
	Insert_VF(result, A);
	for (i = sz_state , j = i - n; i >= 0; i--, j--)
		result[i] = result[j];
	for (i = 0; i < n; i++)
		result[i] = 0;

}

int Valid_IF(state_type st, int BID, int POS)
{
	state_type tmp;
	int One;
	if (BID == -1)
		return -1;
	One = Num_Of_One_IF(block[BID]);
	if ((0 <= POS&&POS < sz_state)){
		R_Shift_VF(block[BID], POS, tmp);
		if (Overlap_One_BF(st, tmp) == 0){
			R_Shift_VF(block[BID], POS, tmp);
			if (Num_Of_One_IF(tmp) == One)
			return POS;
		}
	}
	return -1;
}	

//Print_State_VF: 주어진 게임 테이블을 출력
void Print_State_VF(state_type st)
{
	int i = 0;
	printf("\nCurrent state: ");
	for (i = 0; i < sz_state; i++){
		if (st[i] == 1)
			printf("#");
		else
			printf(".");
	}
	printf("\n");
}

//Win_IF : 주어진 게임테이블의 승리여부를 판단.
int Win_IF(state_type st, time_t start_time)
{
	int retv, i;
	state_type tmp;
	if (time(NULL) - start_time >= 5)
		return -1; // timeover --> unknown
	for (i = 0; i < num_cho; i++){
		if (Overlap_One_BF(st,choice[i]) == 0){
			Or_VF(st, choice[i], tmp);
			retv = Win_IF(tmp, start_time);
			if (retv == -1)
				return -1; // timeover --> unknown
			else if (retv == 0)
				return 1; // true
		}
	}
	return 0; // false
}

//Best_Choice_IF : 주어진 게임테이블에서 승리방법 반환, 승리방법 없을 경우 G_POS반환
int Best_Choice_IF(state_type st)
{
	int retv, i;
	state_type tmp;
	time_t start_time = time(NULL);
	for (i = 0; i < num_cho; i++){
		if (Overlap_One_BF(st,choice[i]) == 0){
			Or_VF(st, choice[i], tmp);
			retv = Win_IF(tmp, start_time);
			if (retv == -1)
				return -1; // timeover --> unknown
			else if (retv == 0)
				return i; // return choice_id
		}
	}
	return -1;
}
int Find_RandomChoice_IF(state_type st)
{
	state_type tmp;
	int i = 0, N_PC = 0;
	int PC_list[1024];
	for (i = 0; i < num_cho; i++)
		if (Overlap_One_BF(st,choice[i]) == 0)
			PC_list[N_PC++] = i;
	
	return PC_list[rand() % N_PC];
}
// Computer_Choice_IF : 컴퓨터가 최선의 방법을 선택 반환
int Computer_Choice_IF(state_type st)
{
	int best_c = Best_Choice_IF(st);
	if (best_c == -1)
		return Find_RandomChoice_IF(st);
	else
		return best_c;
}

// Print_Block_VF : 주어진 블록을 출력
void Print_Block_VF(state_type blk)
{
	int i;
	for (i = 0; i < sz_state; i++){
		if (blk[i]==1)
			printf("#");
		else
			printf(".");
	}
	printf("\n");
}
int Get_Choice_IF(state_type st, int*BID, int* POS){
	char buffer[MAX];
	int n, retV;
	gets(buffer);

	if (sscanf(buffer, "%d%n", BID, &n) != 1)
		return -5;

	if (*BID == -1)
		return -1;
	else if (!(0 <= *BID && *BID <= num_blk))
		return -4;

	if (sscanf(buffer + n, "%d", POS) != 1)
		return -3;

	if (!((retV = Valid_IF(st, *BID, *POS)) >= 0))
		return -2;
	else
		return retV;
}
//Human_Choice_IF : 사용자로부터 선택을 입력 받음. 잘못된 선택일 경우 다시 입력,
//				 -1이면 게임을 포기
int Human_Choice_IF(state_type st){

	int BID , POS , choice_index, i, V;
	do{
		printf("BID: ##(0), #.#(1), #.##(2)\n");
		printf("Input BID(0~2, -1:give up) and POS(0~%d) : ", sz_state);
		int n = 0, size = 0;
		V = Get_Choice_IF(st, &BID, &POS);
		if (V == -1)
			return BID;
		else if (V < -1)
			printf("%s", Message[V + 5]);

	} while (!(0 <= BID&&BID < num_blk && (V >= 0)));

	return V;
}

//Get_Turn_IF : 사용자로부터 누가 먼저 시작할지 여부를 입력받아 그 순서를 반환.
int Get_Turn_IF(void)
{
	int turn = -1;
	while (!(turn == 0 || turn == 1)){
		printf("Turm(0-human or 1-computer) >> ");
		scanf("%d", &turn);
	}
	return turn;
}

//Get_State_Size_IF : 사용자로부터 게임테이블의 길이를 입력받아 그 값을 반환.
int Get_State_Size_IF(void)
{
	int sz = -1;
	while (!(12 <= sz&&sz <= 32)){
		printf("State Size(12~32) >> ");
		scanf("%d", &sz);
	}
	getchar();
	return sz - 1;
}

//Is_Over_BF : 게임이 끝났는지 여부 판단
int Is_Over_BF(state_type st)
{
	state_type tmp;
	int i = 0;
	for (i = 0; i < num_cho; i++)
		if (Overlap_One_BF(choice[i],st) == 0)
			return 0;
	return 1;
}
int Find_First_One_IF(state_type _s){
	int i = 0;
	for (i = 0; i < sz_state; i++){
		if (_s[i] == 1)
			return i;
	}
	return -1;
}
void ReMake_Block_VF(){
	int i ,j;
	for (i = 0; i < num_blk; i++){
		j=Find_First_One_IF(block[i]);
		if (j != -1)
			L_Shift_VF(block[i], j, block[i]);
	}
}

// Make_Choice_VF : 주어진 블록과 게임테이블 크기로 가능한 모든 선택을 만들어 배열 choice 에 저장.
void Make_Choice_VF()
{
	int m, b_i, NofState,j;
	num_cho = 0;
	for (b_i = 0; b_i < num_blk; b_i++){
		j = num_cho;
		NofState = Num_Of_One_IF(block[b_i]);
		Insert_VF(choice[num_cho], block[b_i]);
		for (m = 1; m < sz_state; m++){
			R_Shift_VF(choice[j], m, choice[++num_cho]);
			if (Num_Of_One_IF(choice[num_cho]) != NofState){
				num_cho -= 1;
				break;
			}
		}
		for (m = 1; m < sz_state; m++){
			L_Shift_VF(choice[j], m, choice[++num_cho]);
			if (Num_Of_One_IF(choice[num_cho]) != NofState)
				break;
		}
		NofState = 0;
	}
	for (b_i = 0; b_i < num_cho; b_i++)
		Print_Block_VF(choice[b_i]);
}


int main()
{
	int turn;
	state_type	Table = { 0 };
	int S;

	srand((unsigned)time(NULL));

	turn = Get_Turn_IF();
	sz_state = Get_State_Size_IF();
	Make_Choice_VF();
	Print_State_VF(Table);

	while (!Is_Over_BF(Table)){

		if (turn == HUMAN)		S = Human_Choice_IF(Table);

		else					S = Computer_Choice_IF(Table);

		if (S == -1)
			break;

		if (turn == HUMAN){
			turn = COMPUTER;
			printf("Human choice : ");
			Print_Block_VF(choice[S]);
			Or_VF(Table, choice[S],Table);
		}

		else{
			turn = HUMAN;
			printf("Com.  choice : ");
			Print_Block_VF(choice[S]);
			Or_VF(Table, choice[S], Table);
		}

		Print_State_VF(Table);
	}

	if (turn == HUMAN)
		printf("\nCumputer Win\n");
	else
		printf("\nHuman Win\n");

	return 0;
}
