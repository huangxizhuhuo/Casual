#include "ChessBoard.h"
#include <cstdlib>
#include <ctime>
using namespace std;

ChessBoard::ChessBoard()
{
	for(int i=0;i<GRIDNUM;i++)
	{
		for(int j=0;j<GRIDNUM;j++)
		{
		    grid1[i][j].x=GRIDSTARTX+j*GRIDLENGTH;
			grid1[i][j].y=GRIDSTARTY+i*GRIDLENGTH;
			grid1[i][j].key=0;
		}
	}
	

}
void ChessBoard::Init()
{
	for(int i=0;i<GRIDNUM;i++)
		for(int j=0;j<GRIDNUM;j++)
			grid1[i][j].key=0;

	int count=0;
	winner=2;
	over=false;
	num[0]=num[1]=0;//初始双方下子数皆为0个

	for(int i=0;i<GRIDNUM;i++)//水平方向的获胜组合(10*6=60种)
	{
		for(int j=0;j<GRIDNUM;j++)
		{
			for(int k=0;k<192;k++)
			{
				ptab[i][j][k]=false;//Debug默认是true
				ctab[i][j][k]=false;
			}
		}
	}

	for(int i=0;i<192;i++)//双方获胜组合的棋子数初始化
	{
		win[0][i]=0;
		win[1][i]=0;
	}

	for(int i=0;i<GRIDNUM;i++)//水平方向的获胜组合(10*6=60种)
	{
		for(int j=0;j<=GRIDNUM-5;j++)
		{
			for(int k=0;k<5;k++)
			{
				ptab[i][j+k][count]=true;
				ctab[i][j+k][count]=true;
			}
			count++;
		}
	}
	for(int i=0;i<GRIDNUM;i++)
	{
		for(int j=0;j<=GRIDNUM-5;j++)
		{
			for(int k=0;k<5;k++)
			{
				ptab[j+k][i][count]=true;
				ctab[j+k][i][count]=true;
			}
			count++;
		}
	}
    for(int i=0;i<=GRIDNUM-5;i++)
	{
		for(int j=0;j<=GRIDNUM-5;j++)
		{
			for(int k=0;k<5;k++)
			{
				ptab[j+k][i+k][count]=true;
				ctab[j+k][i+k][count]=true;
			}
			count++;
		}
	}
	for(int i=0;i<=GRIDNUM-5;i++)
	{
		for(int j=9;j>=4;j--)
		{
			for(int k=0;k<5;k++)
			{
				ptab[j-k][i+k][count]=true;
				ctab[j-k][i+k][count]=true;
			}
			count++;
		}
	}

	/*srand(unsigned(time(NULL)));
	if(rand()%2==0)
		turn=true;
	else
		turn=false;*/
}

void ChessBoard::Place(int x,int y)
{ 
	grid1[x][y].key=1;
	num[0]++;//0代表Player	
	if(num[0]==50&&num[1]==50)	
	{		
		over=true;			
		winner=2;//平		
	}	
	else	
	{		
		for(int i=0;i<192;i++)		
		{			
			if(ptab[x][y][i])			
			{			
				win[0][i]++;				
				ctab[x][y][i]=false;				
				win[1][i]=7;				
				if(win[0][i]==5)//				
				{					
					over=true;					
					winner=0;				
				}				
			}		
		}						
	}
}

void ChessBoard::ComputerTurn()
{
	int grades[2][10][10];
	int max=0;
	int u,v;
	for(int m=0;m<10;m++)
	{
		for(int n=0;n<10;n++)
		{
			grades[0][m][n]=0;
			grades[1][m][n]=0;
			if(grid1[m][n].key==0)//计算每个空位置的得分
			{
				for(int i=0;i<192;i++)
				{		
					if(ptab[m][n][i]&&win[0][i]!=7)
					{//Player的
						switch(win[0][i])
						{
						case 0:
							grades[0][m][n]+=1;
							break;
						case 1:
							grades[0][m][n]+=200;
							break;
						case 2:
							grades[0][m][n]+=400;
							break;
						case 3:
							grades[0][m][n]+=2000;
							break;
						case 4:
							grades[0][m][n]+=10000;
							break;
						}
					}
					if(ctab[m][n][i]&&win[1][i]!=7)
					{
						switch(win[1][i])
						{
						case 0:
							grades[1][m][n]+=1;
							break;
						case 1:
							grades[1][m][n]+=220;
							break;
						case 2:
							grades[1][m][n]+=420;
							break;
						case 3:
							grades[1][m][n]+=2100;
							break;
						case 4:
							grades[1][m][n]+=20000;
							break;
						}
					}
				}
				//计算最佳的落子位置，根据每个空位的得分
				if(max==0)//双方棋格分数的最大值
				{
					u=m;
					v=n;
				}
				if(grades[0][m][n]>max)//最佳防守位置
				{
					max=grades[0][m][n];
					u=m;
					v=n;
				}
				else if(grades[0][m][n]==max)//最佳防守位置中的最佳攻击位置
				{
					if(grades[1][m][n]>grades[1][u][v])
					{
						u=m;
						v=n;
					}
				}

				if(grades[1][m][n]>max)
				{
					max=grades[1][m][n];
					u=m;
					v=n;
				}
				else if(grades[1][m][n]==max)
				{
					if(grades[0][m][n]>grades[0][u][v])
					{
						u=m;
						v=n;
					}
				}

			}
		}
	}
	grid1[u][v].key=2;//Computer落子
	num[1]++;
	if(num[0]==50&&num[1]==50)
	{
		winner=2;
		over=true;
	}
	else
	{
		for(int i=0;i<192;i++)
		{
			if(ctab[u][v][i])
			{
				win[1][i]++;
				ptab[u][v][i]=false;//更新Player表的情况
				win[0][i]=7;
				if(win[1][i]==5)//五连情况产生
				{
					winner=1;//Computer win
					over=true;
				}
			}
		}

	}
}

int ChessBoard::FindRandEmptyGrid() const
{
	int r=0;
	while(1)
	{
		r=rand()%(GRIDNUM*GRIDNUM);
		if(grid1[r/GRIDNUM][r%GRIDNUM].key==0)
			break;
	}
	return r;
}

int ChessBoard::CursorOnWhichGrid(int x,int y) const
{
	for(int i=0;i<GRIDNUM;i++)//遍历
	{
		for(int j=0;j<GRIDNUM;j++)
		{	
			if(x>grid1[i][j].x&&x<grid1[i][j].x+GRIDLENGTH&&y>grid1[i][j].y&&y<grid1[i][j].y+GRIDLENGTH)	
			{		
				return (i*GRIDNUM+j);		
			}
		}
	}
	return -1;
}
