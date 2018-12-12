#pragma once
#include "../Core/Export.h"
#include "ChessBoard.h"
#include "AStar.h"

class CColorBallApp : public  CApplication
{
public:
	CColorBallApp();
	~CColorBallApp();
	virtual BOOL Init();
	virtual BOOL Shutdown();
	virtual BOOL Frame();
	void DrawChessBoard();
	ChessBoard m_ChessBoard;
	bool PlaySound(long Num);
protected:
	CGraphics m_Graphics;
	CTexture m_texChessboard;
	CTexture m_texChesses[7];
	CTexture m_texBlack;
	CGameFont m_Font;
	CInput m_Input;
	//CInputDevice m_Mouse;
	CInputDevice m_Keyboard;
	CAStar m_AStar;
	CSound m_Sound;
	CSoundChannel m_SoundChannel;
	CSoundData    m_SoundData;
	int m_nFPS;
	int m_nCurPickChess;
	//bool m_bMoving;
	bool m_bGameOver;
	_asNode* m_bestPath;
	std::list<int> m_Scores;
};
