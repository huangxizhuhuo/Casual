#pragma once

const int GRIDLENGTH=48;
const int GRIDNUM=10;
const int GRIDSTARTX=83;
const int GRIDSTARTY=14;

struct Grid
{
	int x;
	int y;
	int key;	
};

struct ChessBoard
{
	Grid grid1[GRIDNUM][GRIDNUM];
	ChessBoard();
	void Init();
	void Place(int x,int y);
	void ComputerTurn();
	int FindRandEmptyGrid() const;
	int  CursorOnWhichGrid(int x,int y) const;
	bool IsOver() {return over;}
	int  Winner() {return winner;}
private:
	bool ptab[10][10][192];//player��ʤ��
	bool ctab[10][10][192];//computer��ʤ��
	int win[2][192];//192����ʤ���,ÿ����������
	int num[2];
	bool over;
	int winner;
};
