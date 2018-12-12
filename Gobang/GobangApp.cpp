#include "GobangApp.h"
#include <sstream>
#include <string>

#pragma comment (lib,"Core_d.lib")
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")
#pragma comment (lib,"dinput8.lib")
#pragma comment (lib,"dxguid.lib")
#pragma comment (lib,"winmm.lib")

CGobangApp::CGobangApp()
{
	m_Width  = 640;
	m_Height = 530;
	m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	wcscpy_s(m_Class, L"FiveAppClass");
	wcscpy_s(m_Caption, L"Îå×ÓÆå");
}

CGobangApp::~CGobangApp()
{
}

BOOL CGobangApp::Init()
{
	MoveWindow(GethWnd(),(1024-m_Width)/2,(768-m_Height)/2,m_Width,m_Height,TRUE);
	m_ChessBoard.Init();
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
	m_Input.Init(GethWnd(),GethInst());
	//m_Mouse.Create(&m_Input,MOUSE,1);
	m_Keyboard.Create(&m_Input,KEYBOARD,1);
	m_Font.Create(&m_Graphics,L"Arial");
	m_texChessboard.Load(&m_Graphics,L"../res/Chessboard1.bmp");
	m_texRed.Load(&m_Graphics, L"../res/chessb.png");
	m_texBlue.Load(&m_Graphics, L"../res/chessw.png");
	return TRUE;
}

BOOL CGobangApp::Frame()
{
	static DWORD dwOldTime=timeGetTime();
	std::stringstream ss;
	//m_Mouse.Read();
	m_Keyboard.Read();
	if(m_Keyboard.GetKeyState(KEY_F1)==TRUE)
	{
		m_ChessBoard.Init();
		m_Keyboard.SetLock(KEY_F1,TRUE);
	}
	
	m_Graphics.Clear();
	m_Graphics.BeginScene();
	std::wstringstream ssCursorPos;
	//ssCursorPos<<L"( "<<m_Mouse.GetXPos()<<L","<<m_Mouse.GetYPos()<<L" )";
	//m_Font.Print(ssCursorPos.str(),0,0);
	//if(m_Mouse.GetButtonState(0)==TRUE)
	if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(GethWnd(), &pt);	
		int pos=m_ChessBoard.CursorOnWhichGrid(pt.x, pt.y);
		int x,y;
		x=pos/GRIDNUM;
		y=pos%GRIDNUM;
		if(m_ChessBoard.grid1[x][y].key==0&&!m_ChessBoard.IsOver())
		{
			m_ChessBoard.Place(x,y);
			m_ChessBoard.ComputerTurn();
		}
		//m_Mouse.SetLock(0,TRUE);
	}

	DrawChessBoard();

	std::wstringstream ssWinner;
	if(m_ChessBoard.IsOver())
	{
		int n=m_ChessBoard.Winner();
		if(n==0)
			ssWinner<<L"You Win,\nPress F1 To Continue";
		else if(n==1)
			ssWinner<<L"You Failed,\n Press F1 To Continue";
		else
			ssWinner<<L"Y&C,\nPress F1 To Continue";

	}
	m_Font.Print(ssWinner.str().c_str(),0,100);
	
	m_Graphics.EndScene();
	m_Graphics.Display();
	return TRUE;
}

void CGobangApp::DrawChessBoard()
{
	m_Graphics.BeginSprite();
	m_texChessboard.Blit((m_Width-478)/2,10);
	m_Graphics.EndSprite();
	int k;
	for(int i=0;i<GRIDNUM;i++)
	{
		for(int j=0;j<GRIDNUM;j++)
		{
			if((k=m_ChessBoard.grid1[i][j].key)!=0)
			{
				m_Graphics.BeginSprite();
				if(k==1)
					m_texRed.Blit(m_ChessBoard.grid1[i][j].x,m_ChessBoard.grid1[i][j].y);
				else if(k==2)
					m_texBlue.Blit(m_ChessBoard.grid1[i][j].x,m_ChessBoard.grid1[i][j].y);
				m_Graphics.EndSprite();
			}
		}
	}
}

BOOL CGobangApp::Shutdown()
{
	m_texChessboard.Free();
	m_Graphics.Shutdown();
	return true;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	CGobangApp App;
	return App.Run();
}

