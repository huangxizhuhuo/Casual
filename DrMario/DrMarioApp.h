#pragma once
#include "../Core/Export.h"
#include <vector>

const int CONTAINERX1=130;
const int CONTAINERX2=258;
const int CONTAINERY1=194;//50;
const int NEXTCAPSULEX=300;
const int NEXTCAPSULEY=240;
const int SIDELENGTH=16;
const int NUM_ROWS=8;
const int NUM_COLUMNS=16;//25;
const int NUM_SOUNDS=1;
const int NUM_MUSIC=2;
const int NUM_POSSIBILITY=NUM_ROWS*(NUM_COLUMNS-3);
const int NUM_LEVELS=20;

struct Virus
{
	int  x;
	int  y;
	int  color;
	bool died;//是否被消灭
};
struct Pos
{
	int x;
	int y;
	int color;//记住要消去的位置的颜色，仅用于画消去的动画
	Pos(int xx,int yy):x(xx),y(yy){}
	Pos(int xx,int yy,int c):x(xx),y(yy),color(c){}
	bool operator==(const Pos& pos)
	{
		if(x==pos.x&&y==pos.y)
			return true;
		return false;
	}
};
struct Grid
{
	int color;//占有该位置的颜色
	int direction;//上，下，左，右，独(4)
	int num;//占有该位置胶囊的编号，一个胶囊对应一个编号
	Grid():color(0),direction(0),num(0) {}
	Grid(int c,int d,int n):color(c),direction(d),num(n){}
	bool operator=(const Grid& grid)
	{
		color=grid.color;
		direction=grid.direction;
		//num=grid.direction;////////
		num=grid.num;
		return true;
	}
};
struct Capsule
{
	int x;
	int y;
	int color1;
	int color2;
	int direction;//左，上，右，下
	bool operator=(const Capsule& capsule)
	{
		x=capsule.x;
		y=capsule.y;
		color1=capsule.color1;
		color2=capsule.color2;
		direction=capsule.direction;
		return true;
	}
};
Grid Container[NUM_ROWS][NUM_COLUMNS];//用于保存当前容器的状态，以便画图
bool a[NUM_ROWS][NUM_COLUMNS][NUM_POSSIBILITY]={false};//排列组合，在玛丽医生问题中为NUM_ROWS*(NUM_COLUMNS-3)种
int  b[NUM_POSSIBILITY]={0};//每种组合的激活数目

class CDrMarioApp:public  CApplication
{
public:
	CDrMarioApp();
	~CDrMarioApp();
	BOOL Init();  
    BOOL Shutdown(); 
    BOOL Frame();
	bool PlaySound(long Num);
	bool PlayMusic(long Num);
	bool StopMusic();
protected:
	CGraphics     m_Graphics;	
	CGameFont     m_Font;
	CInput        m_Input;
	CInputDevice  m_Keyboard;
	CSound        m_Sound;
	CSoundChannel m_SoundChannel;
	CSoundData    m_SoundData;
	//CMusicChannel m_MusicChannel;
	CTexture      m_texRect;
	CTexture      m_texDrMario;
	
	Virus*  m_Virus;
	bool    m_bGameOver;
	Capsule m_Capsule;
	Capsule m_NextCapsule;
	bool    m_bTouch;
	bool    m_bSingleTouch;
	bool    m_bNextLevel;
	int     m_nCapsuleNum;
	int     m_nPlayerScore;
	int     m_nLevel;
	int     m_nTopScore;
	int     m_nVirusNum;
	int     m_nSpeed;
	
	std::vector<std::vector<Pos>> m_Vectors;
	std::vector<Pos>              m_VirusPos;//保存细菌的位置

	bool IsTheSameColor(std::vector<Pos>& vec);
	bool IsNotInVector(Pos p, std::vector<Pos>& vec);
	bool IsNotInVector(int n, std::vector<int>& vec);
	
	void InitVirus();
	void GenerateNextCapsule();
	void NewLevel();
	
	void DrawContainer(int x1,int x2);
	void DrawVirus();
	void DrawCapsule(const Capsule& capsule);

	void PrintInformation();
	void ProcessInput();
	
	std::vector<Pos> ProcessCounteract();
};
