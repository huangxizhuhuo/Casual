#include "ChessBoard.h"

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

	std::vector<int> ints;
	for(int i=0;i<5;i++)//5个不同的随机数
	{
		int r;
		while(1)
		{
		    r=rand()%(GRIDNUM*GRIDNUM);
			int counter=0;
			for(int j=0;j<(int)ints.size();j++)
		    {
			    if(ints[j]==r)
					break;
				else
					counter++;
		    }
			if(counter==ints.size())
				break;
		}
		ints.push_back(r);
	}

	for(int i=0;i<3;i++)
	{
		 grid2[i].x=272+i*GRIDLENGTH;
		 grid2[i].y=12;
		 grid2[i].key=rand()%7+1;//1--7
	}

	for(int i=0;i<GRIDNUM;i++)
	{
		for(int j=0;j<GRIDNUM;j++)
		{	
			for(int k=0;k<140;k++)
			{
			    m_Tab[i][j][k]=false;
			}
		}
	}
	//初始化5连珠排列组合
	int num=0;
	
	for(int i=0;i<GRIDNUM;i++)//9*5=45
	{
		for(int j=0;j<5;j++)
		{
			std::vector<Pos> vecPos;	
			for(int k=0;k<5;k++)
			{		
			    m_Tab[i][j+k][num]=true;
				vecPos.push_back(Pos(i,j+k));
			}
			m_Vectors.push_back(vecPos);
			num++;
		}
	}
	for(int i=0;i<GRIDNUM;i++)//9*5=45
	{
		for(int j=0;j<5;j++)
		{
			std::vector<Pos> vecPos;
			for(int k=0;k<5;k++)
			{  		
				m_Tab[j+k][i][num]=true;
				vecPos.push_back(Pos(j+k,i));
			}
			m_Vectors.push_back(vecPos);
			num++;
		}
	}
	for(int i=0;i<5;i++)//斜线 5*5=25
	{
		for(int j=0;j<5;j++)
		{
			std::vector<Pos> vecPos;
			for(int k=0;k<5;k++)
			{	
				m_Tab[j+k][i+k][num]=true;
				vecPos.push_back(Pos(j+k,i+k));
			}
			m_Vectors.push_back(vecPos);
			num++;
		}
	}
	for(int i=0;i<5;i++)//反斜线 5*5=25
	{
		for(int j=8;j>=4;j--)
		{
			std::vector<Pos> vecPos;
			for(int k=0;k<5;k++)
			{
				m_Tab[j-k][i+k][num]=true;
				vecPos.push_back(Pos(j-k,i+k));
			}
			m_Vectors.push_back(vecPos);
			num++;
		}
	}
	for(int i=0;i<140;i++)
		m_Counteract[i]=0;

	for(int i=0;i<(int)ints.size();i++)
	{
		grid1[ints[i]/GRIDNUM][ints[i]%GRIDNUM].key=rand()%7+1;
		JudgeChessBoardAdd(ints[i]/GRIDNUM,ints[i]%GRIDNUM);
	}
}

bool ChessBoard::IsNotInVector(Pos p,std::vector<Pos> vec)
{
	for(int i=0;i<(int)vec.size();i++)
	{
		if(p==vec[i])
			return false;
	}
	return true;
}

bool ChessBoard::IsTheSameColor(std::vector<Pos> vec)
{
	int k[5];
	 k[0]=grid1[vec[0].x][vec[0].y].key;
	 k[1]=grid1[vec[1].x][vec[1].y].key;
	 k[2]=grid1[vec[2].x][vec[2].y].key;
	 k[3]=grid1[vec[3].x][vec[3].y].key;
	 k[4]=grid1[vec[4].x][vec[4].y].key;
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			if(k[j]!=k[i])
				return false;
		}
	}
	return true;
}

int ChessBoard::JudgeChessBoardAdd(int x,int y)
{
	std::vector<int> ints;
	for(int i=0;i<140;i++)			
	{					
		if(m_Tab[x][y][i])//&&m_Counteract[i]<5)					
		{						
			m_Counteract[i]++;	
			if(m_Counteract[i]==5)						
				ints.push_back(i);								
		}			
	}	
	std::vector<Pos> vecPos;//被删除的棋子
	for(int i=0;i<(int)ints.size();i++)				
	{
		if(IsTheSameColor(m_Vectors[ints[i]]))
		{
		for(int j=0;j<5;j++)				
		{										
			int x=m_Vectors[ints[i]][j].x;					
			int y=m_Vectors[ints[i]][j].y;	
			if(IsNotInVector(Pos(x,y),vecPos))
			{
				vecPos.push_back(Pos(x,y));
			    JudgeChessBoardRemove(x,y);	//被删过则不能又删一次			
			    //grid1[x][y].key=0;	
			}
		}
		}	
	}
	for(int i=0;i<(int)vecPos.size();i++)
		grid1[vecPos[i].x][vecPos[i].y].key=0;

	return (int)vecPos.size();
}

void ChessBoard::JudgeChessBoardRemove(int x,int y)
{
	for(int i=0;i<140;i++)	
	{
		if(m_Tab[x][y][i])//&&m_Counteract[i]>0)							
		   m_Counteract[i]--;	
	}		
}

void ChessBoard::GenerateThreeChesses()
{
	for(int i=0;i<3;i++)
		 grid2[i].key=rand()%7+1;//1--7
}

int ChessBoard::FindRandEmptyGrid() const
{
	int n=0;
	for(int i=0;i<GRIDNUM;i++)
	{
		for(int j=0;j<GRIDNUM;j++)
		{
			if(grid1[i][j].key==0)
			{
				n++;
				break;
			}
		}
	}
	if(n==0)
		return -1;//找不到一个空格
	int r;
	while(1)
	{
	    r=rand()%81;
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