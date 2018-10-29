//FillBlank2D
#include<stdio.h>
#include<stdlib.h>
#include<Time.h>
#define HUMAN 0
#define COMPUTER 1
#define MAX 10000
#define W 10
#define H 10
#define TimeOver -1
#define ChangeTurn(x) ((x) = ((x+1)%2))
#define AND1(s,p) ((*(s)&*(p)) == 0)
#define AND2(s,p) ((*(s)&*(p)) == 0 && AND1((s)+1,(p)+1))
#define AND3(s,p) ((*(s)&*(p)) == 0 && AND2((s)+1,(p)+1))
#define AND4(s,p) ((*(s)&*(p)) == 0 && AND3((s)+1,(p)+1))
#define AND5(s,p) ((*(s)&*(p)) == 0 && AND4((s)+1,(p)+1))
#define AND6(s,p) ((*(s)&*(p)) == 0 && AND5((s)+1,(p)+1))
#define AND7(s,p) ((*(s)&*(p)) == 0 && AND6((s)+1,(p)+1))
#define AND8(s,p) ((*(s)&*(p)) == 0 && AND7((s)+1,(p)+1))
#define XOR1(s,p) (*(s)^=*(p))
#define XOR2(s,p) (*(s)^=*(p) , XOR1((s)+1,(p)+1))
#define XOR3(s,p) (*(s)^=*(p) , XOR2((s)+1,(p)+1))
#define XOR4(s,p) (*(s)^=*(p) , XOR3((s)+1,(p)+1))
#define XOR5(s,p) (*(s)^=*(p) , XOR4((s)+1,(p)+1))
#define XOR6(s,p) (*(s)^=*(p) , XOR5((s)+1,(p)+1))
#define XOR7(s,p) (*(s)^=*(p) , XOR6((s)+1,(p)+1))
#define XOR8(s,p) (*(s)^=*(p) , XOR7((s)+1,(p)+1))
#define MaxBit 8
#define BitLen 32
#define MassStartNum 1
#define Undetectable -1
typedef int StateType[H][W];
typedef unsigned  BitStateType[MaxBit];
int StateSize = W*H;
typedef struct{
	int Col;
	int Row;
}Point;

StateType BaseBlock[1024] = { 0 };
StateType Block[] = {
	{ { 1, 1 }, { 0, 1, 1 } },
	{ { 1, 1, 1, 1 } },
	{ { 1, 1 }, { 1, 1 } },
	{ { 0, 1, 1 }, { 1, 1 } },
	{ { 1 }, { 1, 1, 1 } },
	{ { 0, 0, 1 }, { 1, 1, 1 } },
	{ { 0, 1 }, { 1, 1, 1 } },
};
StateType FullChoice[MAX];
BitStateType BitFullChoice[MAX];
int PossChoice[MAX];
StateType ChoiceOrder[W*H];
int PresentI;
int Order = 1;
int MassMaxNum = 0;
int NumBlk = sizeof(Block) / sizeof(*Block);
int NumPossCho = MAX;
int NumFullCho, NumBaseCho;
char* Message[10] = {
	"This choice is not possible.......\n",
	"Input X Failed......\n",
	"Invalid X ranges......\n",
	"Input Y Failed ......\n",
	"Invalid Y ranges ......\n",
	"\n\t\t\tThis turn is first........\n\n",
	"\n\t\t\tTabie is RollBack.........\n\n",
};

//PrintStateVF: 주어진 게임 테이블을 출력
void PrintStateVF(StateType st){
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
				else if (st[Row][Col] == 1)
					printf("■");
			}
			else
				printf("□");
		}
		printf("\n");
	}
}
// PrintBlockVF : 주어진 블록을 출력 
//
//			디버깅용
//
void PrintBlockVF(StateType blk){
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
void CopyVF(StateType dst, StateType src){
	int count = 0;
	int Col, Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (src[Row][Col] == 1 || dst[Row][Col] == 1)
				dst[Row][Col] = src[Row][Col];

}

int NumOfOneIF(StateType st){
	int Col, Row, check = 0;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (st[Row][Col] != 0)
				check++;
	return check;
}
int BitOverlapOneBF(BitStateType b1, BitStateType b2){
	int Row;
	for (Row = 0; Row < H; Row++)
		if ((b1[Row] & b2[Row]) != 0)
			return 1;
	return 0;
}
int OverlapOneBF(StateType s1, StateType s2){
	int Col, Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if ((s1[Row][Col] != 0)&& (s2[Row][Col] != 0))
				return 1;
	return 0;
}
void RemakePossChoice(StateType st){
	int bi;
	NumPossCho = 0;
	for (bi = 0; bi < NumFullCho; bi++)
		if (!OverlapOneBF(st, FullChoice[bi]))
			PossChoice[NumPossCho++] = bi;
}

int RollBackIF(StateType st){
	if (Order >= 2){
		Order -= 3;
		CopyVF(st, ChoiceOrder[Order++]);
		RemakePossChoice(st);
		PrintStateVF(st);
		printf("%s", Message[6]);
	}
	else
		printf("%s", Message[5]);

	if (Order == 0){
		Order = 1;
		return TimeOver;
	}
	return 1;
}

void SaveChoiceVF(StateType st){
	int Col, Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			ChoiceOrder[Order][Row][Col] = st[Row][Col];
	Order++;
}

void ArrayToBitVF(StateType s1, BitStateType b1){
	int Col, Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++){
			b1[(Row*W + Col) / BitLen] |= (s1[Row][Col] << ((Row*W + Col) % BitLen));
		}
}
void BitOrVF(BitStateType b1, BitStateType b2,BitStateType result){
	int BitIndex = 0;
	for (BitIndex = 0; BitIndex<= StateSize/BitLen; BitIndex++)
		result[BitIndex] = b1[BitIndex] | b2[BitIndex];
}
void OrVF(StateType s1, StateType s2, StateType Result){
	int Col, Row = 0;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (s1[Row][Col] != 0 || s2[Row][Col] != 0)
				Result[Row][Col] = 1;
			else
				Result[Row][Col] = 0;
}
//WinIF : 주어진 게임테이블의 승리여부를 판단.
int BitWin1IF(BitStateType st, time_t StartTime){

	int RetValue=0, i;
	if (time(NULL) - StartTime >= 10)
		return TimeOver; // Timeover --> unknown
	for (i = 0; i < NumPossCho; i++)
		if (AND1(st, BitFullChoice[PossChoice[i]])){
			XOR1(st, BitFullChoice[PossChoice[i]]);
			RetValue = BitWin1IF(st, StartTime);
			XOR1(st, BitFullChoice[PossChoice[i]]);
			if (RetValue == TimeOver)
				return TimeOver; // Timeover --> unknown
			else if (RetValue == 0)
				return 1; // true
		}
	
	return 0; // false
}
int BitWin2IF(BitStateType st, time_t StartTime){
	int RetValue = 0, i;
	if (time(NULL) - StartTime >= 10)
		return TimeOver; // Timeover --> unknown
	for (i = 0; i < NumPossCho; i++){
		if (AND2(st, BitFullChoice[PossChoice[i]])){
			XOR2(st, BitFullChoice[PossChoice[i]]);
			RetValue = BitWin2IF(st, StartTime);
			XOR2(st, BitFullChoice[PossChoice[i]]);
			if (RetValue == TimeOver)
				return TimeOver; // Timeover --> unknown
			else if (RetValue == 0)
				return 1; // true
		}
	}
	return 0; // false
}
int BitWin3IF(BitStateType st, time_t StartTime){
	int RetValue = 0, i;
	BitStateType Tmp = { 0 };
	if (time(NULL) - StartTime >= 10)
		return TimeOver; // Timeover --> unknown
	for (i = 0; i < NumPossCho; i++){
		if (AND3(st, BitFullChoice[PossChoice[i]])){
			XOR3(st, BitFullChoice[PossChoice[i]]);
			RetValue = BitWin3IF(st, StartTime);
			XOR3(st, BitFullChoice[PossChoice[i]]);
			if (RetValue == TimeOver)
				return TimeOver; // Timeover --> unknown
			else if (RetValue == 0)
				return 1; // true
		}
	}
	return 0; // false
}
int BitWin4IF(BitStateType st, time_t StartTime){
	int RetValue = 0, i;
	BitStateType Tmp = { 0 };
	if (time(NULL) - StartTime >= 10)
		return TimeOver; // Timeover --> unknown
	for (i = 0; i < NumPossCho; i++){
		if (AND4(st, BitFullChoice[PossChoice[i]])){
			XOR4(st, BitFullChoice[PossChoice[i]]);
			RetValue = BitWin4IF(st, StartTime);
			XOR4(st, BitFullChoice[PossChoice[i]]);
			if (RetValue == TimeOver)
				return TimeOver; // Timeover --> unknown
			else if (RetValue == 0)
				return 1; // true
		}
	}
	return 0; // false
}
int BitWin5IF(BitStateType st, time_t StartTime){
	int RetValue = 0, i;
	BitStateType Tmp = { 0 };
	if (time(NULL) - StartTime >= 10)
		return TimeOver; // Timeover --> unknown
	for (i = 0; i < NumPossCho; i++){
		if (AND5(st, BitFullChoice[PossChoice[i]])){
			XOR5(st, BitFullChoice[PossChoice[i]]);
			RetValue = BitWin5IF(st, StartTime);
			XOR5(st, BitFullChoice[PossChoice[i]]);
			if (RetValue == TimeOver)
				return TimeOver; // Timeover --> unknown
			else if (RetValue == 0)
				return 1; // true
		}
	}
	return 0; // false
}
int BitWin6IF(BitStateType st, time_t StartTime){
	int RetValue = 0, i;
	BitStateType Tmp = { 0 };
	if (time(NULL) - StartTime >= 10)
		return TimeOver; // Timeover --> unknown
	for (i = 0; i < NumPossCho; i++){
		if (AND6(st, BitFullChoice[PossChoice[i]])){
			XOR6(st, BitFullChoice[PossChoice[i]]);
			RetValue = BitWin6IF(st, StartTime);
			XOR6(st, BitFullChoice[PossChoice[i]]);
			if (RetValue == TimeOver)
				return TimeOver; // Timeover --> unknown
			else if (RetValue == 0)
				return 1; // true
		}
	}
	return 0; // false
}
int BitWin7IF(BitStateType st, time_t StartTime){
	int RetValue = 0, i;
	if (time(NULL) - StartTime >= 10)
		return TimeOver; // Timeover --> unknown
	for (i = 0; i < NumPossCho; i++)
		if (AND7(st, BitFullChoice[PossChoice[i]])){
			XOR7(st, BitFullChoice[PossChoice[i]]);
			RetValue = BitWin1IF(st, StartTime);
			XOR7(st, BitFullChoice[PossChoice[i]]);
			if (RetValue == TimeOver)
				return TimeOver; // Timeover --> unknown
			else if (RetValue == 0)
				return 1; // true
		}

	return 0; // false
}
int BitWin8IF(BitStateType st, time_t StartTime){
	int RetValue = 0, i;
	if (time(NULL) - StartTime >= 10)
		return TimeOver; // Timeover --> unknown
	for (i = 0; i < NumPossCho; i++)
		if (AND8(st, BitFullChoice[PossChoice[i]])){
			XOR8(st, BitFullChoice[PossChoice[i]]);
			RetValue = BitWin1IF(st, StartTime);
			XOR8(st, BitFullChoice[PossChoice[i]]);
			if (RetValue == TimeOver)
				return TimeOver; // Timeover --> unknown
			else if (RetValue == 0)
				return 1; // true
		}

	return 0; // false
}
int BitBestChoiceIF(BitStateType st){
	int RetValue, i;
	time_t StartTime = time(NULL);
	BitStateType Tmp = { 0 };
	int(*WinFunc[8])(BitStateType, time_t) = { BitWin1IF, BitWin2IF, BitWin3IF, BitWin4IF, BitWin5IF, BitWin6IF, BitWin7IF, BitWin8IF };
	for (i = 0; i < NumPossCho; i++){
		BitOrVF(st, BitFullChoice[PossChoice[i]],Tmp);
		RetValue = WinFunc[(StateSize-1)/BitLen](Tmp, StartTime);
		if (RetValue == TimeOver)
			return TimeOver; // Timeover --> unknown
		else if (RetValue == 0){
			printf("BitWin\n");
			return PossChoice[i]; // return FullChoiceid
		}
	}
	return TimeOver;
}
// ComputerChoiceIF : 컴퓨터가 최선의 방법을 선택 반환
int BitComputerChoiceIF(StateType st){
	int bestc;
	BitStateType BitSt = { 0 };
	ArrayToBitVF(st, BitSt);
	bestc = BitBestChoiceIF(BitSt);
	if (bestc == TimeOver){
		printf("BitRandom\n");
		return PossChoice[rand() % NumPossCho];
	}
	else
		return bestc;
	
}
int CalculateBlankIF(StateType st,int Row,int Col){
	
	if (Row < 0 || Col<0 || Row >= H || Col>=W)
		return 0;

	if (st[Row][Col] == 0){
		st[Row][Col] = 1;
		return CalculateBlankIF(st, Row - 1, Col)
			+ CalculateBlankIF(st, Row, Col - 1)
			+ CalculateBlankIF(st, Row, Col + 1)
			+ CalculateBlankIF(st, Row + 1, Col)
			+ 1;
	}
	else
		return 0;
}

void MakeMassVF(StateType st,int MassAr[]){
	int Col, Row;
	MassMaxNum = MassStartNum;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++){
			if (st[Row][Col] == 0){
				//Point FL = { Col, Row };
				MassAr[MassMaxNum] = CalculateBlankIF(st, Row, Col) / 4;
				MassMaxNum++;
			}
		}
}
void QuickSort(int ar[], int begin, int end){
	int i, j, tmp;
	int border = begin;
	if (begin < end){
		i = begin;
		j = end;
		while (i<j){
			for (; i <= end&& !(ar[i]>ar[border]); i++);
			for (; j > begin && !(ar[j] < ar[border]); j--);
			if (i<j){
				tmp = ar[i];
				ar[i] = ar[j];
				ar[j] = tmp;
			}
		}
		tmp = ar[border];
		ar[border] = ar[j];
		ar[j] = tmp;
		QuickSort(ar,  begin, j - 1);
		QuickSort(ar,  j + 1, end);
		return;
	}
}
int MakePairBF(int MassAr[]){
	int I;
	if (MassMaxNum % 2 == 0 || MassMaxNum <= 1)
		return 0;
	QuickSort(MassAr, 0, MassMaxNum - 1);
	for (I = MassStartNum; I < MassMaxNum; I += 2)
		if (MassAr[I] != 0 && (MassAr[I] != MassAr[I + 1]))
			return 0;
	return 1;
}
int BalanceSplitBF(StateType st){
	int i,MassArray[100] = { 0 };
	StateType Tmp;

	for (i = 0; i < NumPossCho; ++i){
		if (!OverlapOneBF(st, FullChoice[PossChoice[i]])){
			OrVF(st, FullChoice[PossChoice[i]], Tmp);
			MakeMassVF(Tmp, MassArray);
			if (MakePairBF(MassArray))
				return PossChoice[i];
		}
	}
	return Undetectable;
}
int CanPreventSplitBF(StateType st, time_t _time){
	int i,RetV;
	StateType Tmp;
	StateType Tmp1;
	if (time(NULL) - _time >= 4){
		printf("시간초과\n");
		return TimeOver;
	}
	if (BalanceSplitBF(st) != Undetectable)
		return 0;
	for (i = 0; i < NumPossCho; i++){ 
		if (!OverlapOneBF(st, FullChoice[PossChoice[i]])){
			OrVF(st, FullChoice[PossChoice[i]], Tmp);			
			RetV = CanPreventSplitBF(Tmp, _time);
			if (RetV == TimeOver)
				return TimeOver;
			else if (RetV == 1)
				return 0;
		}
	}
	return 1;
}
int PreventBalanceSplitIF(StateType st){
	int i,RetV;
	time_t Start=time(NULL);
	for (i = PresentI - 1; i < NumPossCho; i++){
		StateType Tmp;
		OrVF(st, FullChoice[PossChoice[i]], Tmp);
		RetV = CanPreventSplitBF(Tmp, Start);
		if (RetV == 1){
			printf("Blocking\n");
			return PossChoice[i];
		}
		if (RetV == TimeOver)
			return TimeOver;
	}
	printf("Undetectable\n");
	return Undetectable;
}

int WinIF(StateType st, time_t StartTime){
	int RetValue, i;
	StateType Tmp = { 0 };
	if (time(NULL) - StartTime >= 5)
		return -1; // Timeover --> unknown
	for (i = 0; i < NumPossCho; i++){
		if (!OverlapOneBF(st, FullChoice[PossChoice[i]])){
			OrVF(st, FullChoice[PossChoice[i]], Tmp);
			RetValue = WinIF(Tmp, StartTime);
			if (RetValue == -1)
				return -1; // Timeover --> unknown
			else if (RetValue == 0)
				return 1; // true
		}
	}
	return 0; // false
}
//BestChoiceIF : 주어진 게임테이블에서 승리방법 반환, 승리방법 없을 경우 GPOS반환
int BestChoiceIF(StateType st){
	int RetValue, i;
	StateType Tmp = { 0 };
	time_t StartTime = time(NULL);
	for (i = 0; i < NumPossCho; i++){
		OrVF(st, FullChoice[PossChoice[i]], Tmp);
		RetValue = WinIF(Tmp, StartTime);
		if (RetValue == TimeOver)
			return TimeOver; // Timeover --> unknown
		else if (RetValue == 0){
			printf("WINChoice\n");
			return PossChoice[i]; // return FullChoiceid
		}
	}
	return -2;
}
int HeuComputerChoiceIF(StateType st){
	int bestc = BestChoiceIF(st);
	if (bestc == TimeOver){ 
		bestc = PreventBalanceSplitIF(st);
		if (bestc == TimeOver || bestc == Undetectable)
			return PossChoice[rand() % (NumPossCho - PresentI) + PresentI];
		return bestc;
	}
	else if (bestc == -2)
		return PossChoice[rand() % (NumPossCho - PresentI) + PresentI];
	else
		return bestc;
}

int ComputerChoiceIF(StateType st){
	int bestc = BestChoiceIF(st);

	if (bestc == -1)
		return PossChoice[rand() % NumPossCho];
	else
		return bestc;
}

void MakeBlockbyPointsVF(Point* p, int PN, StateType Result){
	int i = 0;
	for (i = 0; i < PN; i++)
		Result[p[i].Row][p[i].Col] = 1;
}
int EqualBF(StateType s1, StateType s2){
	int Col, Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (s1[Row][Col] != s2[Row][Col])
				return 0;
	return 1;
}
int FindPossChoiceIF(StateType A){
	int i;
	for (i = 0; i <= NumPossCho; i++)
		if (EqualBF(A, FullChoice[PossChoice[i]]))
			return PossChoice[i];

	return TimeOver;
}
int ValidIDIF(StateType st, int ID){
	if (0<= ID && ID<NumFullCho && !OverlapOneBF(st, FullChoice[ID]))
		return ID;
	else
		return -10;
	
}

int ValidIF(StateType st, Point* p, int PN){
	int ci;
	StateType Tmp = { 0 };
	MakeBlockbyPointsVF(p, PN, Tmp);
	if ((ci = FindPossChoiceIF(Tmp)) == TimeOver)
		return -10;
	else
		return ci;
}
int GetChoiceIF(StateType st, Point* p){

	char buffer[MAX],third;
	int first, second;
	int i = 0, n1 = 0, n2 = 0, PointNum = 0;
	gets(buffer);
	for (i = 0; !(sscanf(buffer + n1, "%d%d%n", &first, &second, &n2) != 2); i++){
		p[i].Col = first;
		p[i].Row = second;
		if (!(0 <= p[i].Col&&p[i].Col < W))
			return -8;
		if (!(0 <= p[i].Row&&p[i].Row < H))
			return -6;
		n1 += n2;
	}
	if (sscanf(buffer, "%c%d", &third, &second) == 2 && third == '#' )
			return 	ValidIDIF(st, second);
	if (first == TimeOver)
		return TimeOver;
	else if (first == -2)
		return -2;

	PointNum = i;
	return ValidIF(st, p, PointNum);
}
//HumanChoiceIF : 사용자로부터 선택을 입력 받음. 잘못된 선택일 경우 다시 입력,
//				 TimeOver이면 게임을 포기
int HumanChoiceIF(StateType st){

	int V;
	Point points[W*H];
	do{
		int n = 0, size = 0;
		printf("Input 4 Point ( RollBack : -2 /// Give up : TimeOver ///  Type Ex: Col Row || #ID ) \n");
		V = GetChoiceIF(st, points);
		if (-2 <= V &&V <= TimeOver)
			return V;
		else if (V < -2)
			printf("%s", Message[V + 10]);

	} while (!(V >= 0));

	return V;
}

//SetTurnIF : 사용자로부터 누가 먼저 시작할지 여부를 입력받아 그 순서를 반환.
int SetTurnIF(){
	int turn = TimeOver;
	while (!(turn == 0 || turn == 1)){
		printf("Turm(0-human or 1-computer) >> ");
		scanf("%d", &turn);
	}
	getchar();
	return turn;
}
void RotationBlock90VF(StateType dst){
	int Col, Row, i = 0, tmp;
	int MaxL;

	MaxL = W > H ? H : W;
	for (Row = 0; Row < MaxL; Row++)
		for (Col = Row + 1; Col < MaxL; Col++){
			tmp = dst[Row][Col];
			dst[Row][Col] = dst[Col][Row];
			dst[Col][Row] = tmp;
		}
	for (Row = 0; Row < MaxL; Row++)
		for (Col = 0; Col < MaxL / 2; Col++){
			tmp = dst[Row][Col];
			dst[Row][Col] = dst[Row][MaxL - 1 - Col];
			dst[Row][MaxL - 1 - Col] = tmp;
		}
}
int DuplicationinFullChoiceBF(StateType A){
	int i;
	if (NumFullCho == TimeOver)
		return 0;
	for (i = 0; i <= NumFullCho; i++){
		if (EqualBF(A, FullChoice[i]))
			return 1;
	}
	return 0;
}

void MakeBaseBlockVF(){
	int bi, count, NumofOne;
	NumBaseCho = 0;
	for (bi = 0; bi < NumBlk; bi++){
		StateType Tmp = { 0 };
		NumofOne = NumOfOneIF(Block[bi]);
		CopyVF(BaseBlock[NumBaseCho], Block[bi]);
		CopyVF(Tmp, Block[bi]);
		for (count = 1; count <= 3; count++){
			RotationBlock90VF(Tmp);
			CopyVF(BaseBlock[++NumBaseCho], Tmp);
		}
		NumBaseCho++;
	}
}

void PasteVF(StateType dst, int Drow, int Dcol, StateType src){
	int Srow, Scol, col;
	for (Srow = 0; Srow < H; Drow++, Srow++)
		if (0 <= Drow&&Drow < H)
			for (col = Dcol, Scol = 0; Scol<W; col++, Scol++)
				if (0 <= col&&col < W)
					dst[Drow][col] = src[Srow][Scol];
}

// MakeFullChoiceVF : 주어진 블록과 게임테이블 크기로 가능한 모든 선택을 만들어 배열 FullChoice 에 저장.

void MakeFullChoiceVF(){
	int bi, NofState, row, col;
	NumFullCho = TimeOver;
	NumPossCho = 0;
	MakeBaseBlockVF();

	for (bi = 0; bi < NumBaseCho; bi++){
		NofState = NumOfOneIF(BaseBlock[bi]);
		for (row = -H + 1; row < H; row++)
			for (col = -W + 1; col < W; col++){
				StateType Tmp = { 0 };
				PasteVF(Tmp, row, col, BaseBlock[bi]);
				if (NumOfOneIF(Tmp) == NofState)
					if (!DuplicationinFullChoiceBF(Tmp)){
						PossChoice[NumPossCho++] = ++NumFullCho;
						CopyVF(FullChoice[NumFullCho], Tmp);
					}
			}
	}
	NumFullCho++;
	for (bi = 0; bi < NumFullCho; bi++)
		ArrayToBitVF(FullChoice[bi], BitFullChoice[bi]);
}
void AddChoiceBlocktoStateVF(StateType st, int S){
	int Col, Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (FullChoice[S][Row][Col] == 1)
				st[Row][Col] = 2;
}
void ModifyStateVF(StateType st){
	int Col, Row;
	for (Row = 0; Row < H; Row++)
		for (Col = 0; Col < W; Col++)
			if (st[Row][Col] == 2)
				st[Row][Col] = 1;
}

int main()
{
	int Exit = 1;
	srand((unsigned)time(NULL));
	while (Exit){
		int turn, i = 0, S = 0;
		int Bit = 0, Array = 0;
		int CExit, CASE = 0;
		StateType	Table = { 0 };
		turn = SetTurnIF();
		MakeFullChoiceVF();
		PrintStateVF(Table);
		RemakePossChoice(Table);
		Order = 1;
		S = 0;
		while (NumPossCho != 0 && S != TimeOver){
			ModifyStateVF(Table);
			if (turn == HUMAN) {
				//S = HumanChoiceIF(Table);
				S = BitComputerChoiceIF(Table);
			}
			else if (turn == COMPUTER)
				S = HeuComputerChoiceIF(Table);

			if (S >= 0){
				AddChoiceBlocktoStateVF(Table, S);
				SaveChoiceVF(Table);
				PrintStateVF(Table);
				RemakePossChoice(Table); 
				ChangeTurn(turn);
			}
			else if (S == -2)
				if (RollBackIF(Table) == TimeOver)
					ChangeTurn(turn);
		}
		if (turn == HUMAN)
			printf("BitComputer Win!!\n");
	
		else
			printf("Human Win!!\n");
		printf("Do you want Retry ? : ( 0 : Exit || 1 : Retry )\n");
		scanf("%d", &CExit);
		getchar();
		if (CExit == 1)
			Exit = 1;
		else if (CExit == 0)
			Exit = 0;
	}
	return 0;
}

