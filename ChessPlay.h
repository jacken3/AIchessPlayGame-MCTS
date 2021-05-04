#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define selected 0
#define unselected 1
#define childMax 1024
extern int Size;
extern bool** Board;
extern int ActionList[childMax][4];
typedef bool** board;//bool�Ͷ�ά����
extern int ActionNum;

typedef struct Tree
{
	struct Tree* parent;
	board Board;
	struct Tree* child[childMax];
	float V;
	float N;

}BoardTree;//������

void InitBoard(void);
void ShowBoard(void);
int ModeSelect(void);
void BeginGame(int Mode);
bool GameOver(board* BoardNow);
void reneBoard(int* Action, board* BoardNow);
void AI_Action();
int* randomAction(board* BoardNow);//����һ�������������
int ActionViable(int* Action,board* BoardNow);
board MCTS(board StateInit);
BoardTree* InitTree(board rootBoard);
void ExpandTree(BoardTree* StateNow);
int findUCBMax(BoardTree* StateNow);
void Iterate(board bestBoard);
board GchildBoard(int* Action,board BoardNow);




