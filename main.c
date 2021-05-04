#include  "ChessPlay.h"

int main()
{
	//int ActionList[3][4] = { 1,2,3,4,5,6,7,8,9,10,11,12};
	//int* p = (int*)malloc(4 * sizeof(int));
	//p = *ActionList;
	//printf("%d",p[3]);
	//printf("%d",INF);
	InitBoard();
	int Mode=ModeSelect();
	BeginGame(Mode);
	
}