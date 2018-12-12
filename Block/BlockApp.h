#pragma once

#include "../Core/Export.h"

static const int ROWS = 5;
static const int COLUMNS = 20;
static const int WIDTH = 30;
static const int HEIGHT = 30;
static const int GAP = 10;

struct Block
{
	int x;
	int y;
	int color;
	bool destroy;
};

struct Board
{
	int x;
	int y;
	int length;
	int color;
};

struct Ball
{
	int x;
	int y;
	int vx;
	int vy;
};

class CBlockApp:public CApplication
{
public:
	CBlockApp();
	~CBlockApp();
	BOOL Init();
	BOOL Shutdown();
	BOOL Frame();
	bool PlaySound(long Num);

protected:
	CGraphics		m_Graphics;
	CGameFont		m_Font;
	CInput			m_Input;
	CInputDevice	m_Keyboard;
	CSound			m_Sound;
	CSoundChannel	m_SoundChannel;
	CSoundData		m_SoundData;
	CTexture		m_texRect;
	CTexture		m_texBall;
	int		m_StartX;
	int		m_StartY;
	Block	m_Block[ROWS][COLUMNS];
	Board	m_Board;
	Ball	m_Ball;
	int		m_GameState;

private:
	void InitBlock();
	void DrawBlock();
};

