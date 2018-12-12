#pragma once

#include <vector>

const int GRIDLENGTH=48;//each grid's length,they are same
const int GRIDNUM=9;//if row=column,grid in each row
const int GRIDSTARTX=127;//107;
const int GRIDSTARTY=62;//64;

struct Pos
{
	Pos(int xx,int yy) : x(xx),y(yy) {}
	int x;
	int y;
	bool operator ==(const Pos& p)
	{
		return (p.x==x)&&(p.y==y);
	}
};

struct Grid
{
	int x;
	int y;
	int key;	
};

struct ChessBoard
{
	Grid grid1[GRIDNUM][GRIDNUM];//in which the chess can be picked; 1d or 2d ?
	Grid grid2[3];//the nex three chesses that the computer will add	
	ChessBoard();
	void Init();
	void GenerateThreeChesses();
	int JudgeChessBoardAdd(int x,int y);
	void JudgeChessBoardRemove(int x,int y);
	int  FindRandEmptyGrid() const;
	int  CursorOnWhichGrid(int x,int y) const;
	
protected:	
	bool m_Tab[GRIDNUM][GRIDNUM][140];//5◊”œ‡¡¨
	int  m_Counteract[140];
	std::vector<std::vector<Pos>> m_Vectors;
	bool IsTheSameColor(std::vector<Pos> vec);
	bool IsNotInVector(Pos p,std::vector<Pos> vec);
};
