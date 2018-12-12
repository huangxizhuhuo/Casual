#pragma once

#include "ChessBoard.h"
#include "../Core/Export.h"
class CGobangApp : public CApplication
{
public:
	CGobangApp();
	~CGobangApp();
	BOOL Init();
	BOOL Shutdown();
	BOOL Frame();
	void DrawChessBoard();
	ChessBoard m_ChessBoard;
protected:
	CGraphics m_Graphics;
	CTexture m_texChessboard,m_texRed,m_texBlue;
	CGameFont m_Font;
	CInput m_Input;
	//CInputDevice m_Mouse;
	CInputDevice m_Keyboard;
};
