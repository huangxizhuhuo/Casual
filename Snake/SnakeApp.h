#pragma once
#include "../Core/Export.h"
#include <vector>
#include <list>

struct Food
{
	int x;
	int y;
	int type;
};

struct SnakeUnit
{
	int x;
	int y;
};

typedef std::vector<SnakeUnit> Snake;

class CSnakeApp:public  CApplication
{
public:
	CSnakeApp();
	~CSnakeApp();
	BOOL Init();  
	BOOL Shutdown(); 
	BOOL Frame();
	bool PlaySound(long Num);

protected:
	CGraphics m_Graphics;	
	CGameFont m_Font;
	CInput m_Input;
	CInputDevice m_Keyboard;
	CSound m_Sound;
	CSoundChannel m_SoundChannel;
	CSoundData    m_SoundData;
	int m_nFPS;
	
	CTexture m_texSnake,m_texRed,m_texBomb;
	bool m_bGameOver;
	std::list<int> m_Scores;
	Food m_Food[5];
	Snake m_Snake;
	void Draw();
	void InitFood();
	int m_nDirection;
	bool m_bBomb;
	int m_nBombPosX,m_nBombPosY;
};
