#include  "ChessPlay.h"

int Size;
board Board;


void InitBoard()
{
	printf("请输入棋盘大小：");
	scanf_s("%d", &Size);
	Board = (bool**)malloc(sizeof(bool*) * Size);
	for(int i=0;i<Size;i++)
		Board[i] = (bool*)malloc(sizeof(bool) * Size);
	for (int i = 0; i < Size; i++)
		for (int j = 0; j < Size; j++)
			Board[i][j] = unselected;
	ShowBoard();
}

void ShowBoard()
{
	printf("\n");
	for (int i = 0; i < Size; i++)
	{
		for (int j = 0; j < Size; j++)
			printf("%d\t", Board[i][j]);
		printf("\n");
	}
}

int ModeSelect()
{
	int Mode;
	printf("请选择游戏模式(0--人机 1--人人)：");
	scanf_s("%d", &Mode);
	return Mode;
}

bool GameOver(board BoardNow)
{
	for (int i = 0; i < Size; i++)
		for (int j = 0; j < Size; j++)
			if (BoardNow[i][j])
				return 0;
	return 1;
}

void reneBoard(int* Action,board BoardNow)
{
	if (Action[0] == Action[2])//对行操作
	{
		for (int i = Action[1] - 1; i < Action[3]; i++)
		{
			BoardNow[Action[0]-1][i] = selected;
		}
	}
	else if(Action[1] == Action[3])//对列操作
	{
		for (int i = Action[0] - 1; i < Action[2]; i++)
		{
			BoardNow[i][Action[1]-1] = selected;
		}
	}
}

void AI_Action()
{
	board boardBest = MCTS(Board);
	Iterate(boardBest);
	ShowBoard();
}

int* randomAction(board BoardNow)
{
	int ActionDim, ActionNum, Idx_1, Idx_2;
	int* Action = (int*)malloc(4 * sizeof(int));
	do
	{
		ActionDim = rand() % 2;//随机行动维度（行或列）1为行 0为列
		ActionNum = rand() % Size;
		Idx_1 = rand() % Size;
		Idx_2 = rand() % Size;
		if (ActionDim)
		{
			Action[0] = ActionNum+1;
			Action[1] = ((Idx_1 < Idx_2) ? Idx_1 : Idx_2)+1;
			Action[2] = Action[0];
			Action[3] = ((Idx_1 > Idx_2) ? Idx_1 : Idx_2)+1;
		}
		else
		{
			Action[0] = ((Idx_1 < Idx_2) ? Idx_1 : Idx_2)+1;
			Action[1] = ActionNum+1;
			Action[2] = ((Idx_1 > Idx_2) ? Idx_1 : Idx_2)+1;
			Action[3] = Action[1];
		}
	} while (!ActionViable(Action,BoardNow));
	return Action;
}


int ActionViable(int* Action,board BoardNow)
{
	if (Action[0] == 0)
		return 0;
	if (Action[0] == Action[2])//对行操作
	{
		for (int i = Action[1]-1 ; i <Action[3]; i++)
		{
			if (BoardNow[Action[0]-1][i] == selected)
				return 0;
		}
	}
	else if (Action[1] == Action[3])//对列操作
	{
		for (int i = Action[0]-1 ; i <Action[2]; i++)
		{
			if (BoardNow[i][Action[1]-1] == selected)
				return 0;
		}
	}
	return 1;
}//对当前棋面判断行动的可行性

board MCTS(board BoardNow)
{
	BoardTree* root;
	root=InitTree(BoardNow);
	BoardTree* SNow =NULL;
	int Iterations= 10000;
	int childIdx = 0;
	int VVM = 0;//访问次数最大值
	ExpandTree(root);
	for (int It = 0; It < Iterations; It++)//迭代循环
	{
		SNow = root;
		while (!GameOver(SNow->Board))//每次到游戏结束
		{
			while (SNow->child[0] != NULL)
			{
				childIdx = findUCBMax(SNow);
				SNow = SNow->child[childIdx];
			}
			if (SNow->N == 0)
			{
				int v=Simulation(SNow);
				do
				{
					SNow->N++;
					SNow->V += v;
					SNow = SNow->parent;
					v = -v;
				} while ((SNow->parent != NULL));
				SNow->N++;
				break;
			}
			else if (SNow->N != 0)
			{
				if (GameOver(SNow->Board))//当前棋面已经结束
				{
					int v = -1;
					do
					{
						SNow->N++;
						SNow->V += v;
						SNow = SNow->parent;
						v = -v;
					} while ((SNow->parent != NULL));
					SNow->N++;
					break;
				}
				ExpandTree(SNow);
			}
		}
	}
	for (int i = 0; (i < childMax)&&(root->child[i]!=NULL); i++)
	{
		for (int k = 0; k < Size; k++)
		{
			for (int j = 0; j < Size; j++)
			{
				printf("%d\t", root->child[i]->Board[k][j]);
			}
			printf("\n");
		}
		printf("\t%f %f", root->child[i]->V, root->child[i]->N);
		printf("\n");
		if (root->child[i]->N > VVM)
		{
			VVM = root->child[i]->N;
			childIdx = i;
		}
	}
	return root->child[childIdx]->Board;
}

BoardTree* InitTree(board rootBoard)
{
	BoardTree* root = (BoardTree*)malloc(sizeof(BoardTree));
	root->parent = NULL;
	for (int i = 0; i < childMax; i++)
		root->child[i] = NULL;
	root->N = 0;
	root->V = 0;
	root->Board = rootBoard;
	return root;
}

void ExpandTree(BoardTree* StateNow)
{
	int ActionList[childMax][4] = { 0 };
	int count = 0;
	int childcount = 0;
	for (int i = 0; i < Size; i++)
	{
		for (int j = 0; j < Size; j++)
		{
			for (int k = j; k < Size; k++)
			{
				ActionList[count][0] = i + 1;
				ActionList[count][1] = j + 1;
				ActionList[count][2] = i + 1;
				ActionList[count][3] = k + 1;
				count++;
			}
		}
	}
	for (int i = 0; i < Size; i++)
	{
		for (int j = 0; j < Size; j++)
		{
			for (int k = j+1; k < Size; k++)
			{
				ActionList[count][1] = i + 1;
				ActionList[count][0] = j + 1;
				ActionList[count][3] = i + 1;
				ActionList[count][2] = k + 1;
				count++;
			}
		}
	}
	for (int i = 0; i < count; i++)
	{
		if (ActionViable(*(ActionList + i), StateNow->Board))
		{
			board childBoard =GchildBoard(*(ActionList + i),StateNow->Board);
			BoardTree* child = (BoardTree*)malloc(sizeof(BoardTree));
			child->Board = childBoard;
			child->parent = StateNow;
			for (int i = 0; i < childMax; i++)
				child->child[i] = NULL;
			child->N = 0;
			child->V = 0;
			StateNow->child[childcount] = child;
			childcount++;
		}
	}
}

void BeginGame(int Mode)
{
	printf("游戏开始！");
	switch (Mode)
	{
	case 0://人机模式 重难点
	{
		int GameTurnCount = 0;//记录游戏轮次
		int AI_First = 0;
		printf("请选择是否AI先手（1--AI先手 0--AI后手）：");
		scanf_s("%d", &AI_First);
		while (!GameOver(Board))
		{
			int Action[4] = { 0 };
			if ((AI_First&& GameTurnCount % 2==0)|| (!AI_First && GameTurnCount % 2 == 1))
			{
				AI_Action();
			}
			else
			{
				printf("请玩家行动\n");
				scanf_s("%d %d %d %d", &Action[0], &Action[1], &Action[2], &Action[3]);
				reneBoard(Action,Board);
				ShowBoard();
			}
			GameTurnCount++;
		}
		if (AI_First ^ (GameTurnCount%2) )
			printf("你失败了，下次加油！");
		else
			printf("恭喜你获胜了！！！"); 
		break;
	}
	case 1://人人模式 构建博弈顺序框架
	{
		int GameTurnCount = 0;//记录游戏轮次
		while (!GameOver(Board))//游戏未结束
		{
			int Action[4] = { 0 };
			printf("请玩家%d行动\n", (GameTurnCount % 2 + 1));
			scanf_s("%d %d %d %d", &Action[0], &Action[1], &Action[2], &Action[3]);
			reneBoard(Action,Board);
			ShowBoard();
			GameTurnCount++;
		}
		printf("玩家%d获胜！！！", (GameTurnCount % 2 + 1));
		break;
	}
	default:
		printf("Somethiing is going wrong!");
		break;
	}

}

int findUCBMax(BoardTree* StateNow)
{
	int childIdx = 0;
	float UCBMax = 0;
	for (int i = 0; (i<childMax)&& (StateNow->child[i]!=NULL); i++)
	{
		if (StateNow->child[i]->N == 0)
			return i;
		float UCB = StateNow->child[i]->V / StateNow->child[i]->N + sqrt(2) * sqrt(log(StateNow->N) / StateNow->child[i]->N);
		if (UCB > UCBMax)
		{
			UCBMax = UCB;
			childIdx = i;
		}
	}
	return childIdx;
}

void Iterate(board bestBoard)
{
	for(int i=0;i<Size;i++)
		for (int j = 0; j < Size; j++)
		{
			Board[i][j] = bestBoard[i][j];
		}
}

board GchildBoard(int* Action, board BoardNow)
{
	board childBoard = (bool**)malloc(sizeof(bool*) * Size);
	for (int i = 0; i < Size; i++)
		childBoard[i] = (bool*)malloc(sizeof(bool) * Size);
	for (int i = 0; i < Size; i++)
		for (int j = 0; j < Size; j++)
			childBoard[i][j] = BoardNow[i][j];

	if (Action[0] == Action[2])//对行操作
	{
		for (int i = Action[1] - 1; i < Action[3]; i++)
		{
			childBoard[Action[0] - 1][i] = selected;
		}
	}
	else if (Action[1] == Action[3])//对列操作
	{
		for (int i = Action[0] - 1; i < Action[2]; i++)
		{
			childBoard[i][Action[1] - 1] = selected;
		}
	}
	return childBoard;
}

int Simulation(BoardTree* StateNow)
{
	board simulationBoard = (bool**)malloc(sizeof(bool*) * Size);
	for (int i = 0; i < Size; i++)
		simulationBoard[i] = (bool*)malloc(sizeof(bool) * Size);
	for (int i = 0; i < Size; i++)
		for (int j = 0; j < Size; j++)
			simulationBoard[i][j] = StateNow->Board[i][j];

	int count = 0;
	while (!GameOver(simulationBoard))
	{
		int* Action = (int*)malloc(4 * sizeof(int));
		Action = randomAction(simulationBoard);
		reneBoard(Action, simulationBoard);
		count++;
	}
	free(simulationBoard);
		return pow(-1,count);
}

