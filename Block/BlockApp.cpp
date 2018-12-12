#include "BlockApp.h"
#include <ctime>
#include <string>
#include <sstream>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "Core_d.lib")

const int NUM_SOUNDS = 4;
WCHAR* strSoundFileName[] =
{
	{ L"../res/disappear.wav" },
	{ L"../res/error.wav" },
	{ L"../res/win.wav" },
	{ L"../res/lose.wav" }
};

CBlockApp::CBlockApp()
{
	m_Width = 640;
	m_Height = 480;
	m_Style = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	wcscpy_s(m_Class, MAX_PATH, L"BlockClass");
	wcscpy_s(m_Caption, MAX_PATH, L"´ò×©¿é");
}

CBlockApp::~CBlockApp()
{
}

BOOL CBlockApp::Init()
{
	srand((unsigned)time(NULL));
	MoveWindow(GethWnd(), (1024 - m_Width) / 2, (768 - m_Height) / 2, m_Width, m_Height, TRUE);
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
	m_Input.Init(GethWnd(), GethInst());
	m_Keyboard.Create(&m_Input, KEYBOARD, 1);
	m_Font.Create(&m_Graphics, L"Arial");
	m_Sound.Init(GethWnd(), 22050, 1, 16);
	m_SoundChannel.Create(&m_Sound, 22050, 1, 16);

	m_texRect.Load(&m_Graphics, L"../res/rect.png");
	m_texBall.Load(&m_Graphics, L"../res/ball.bmp", 0xFF000000, D3DFMT_A8R8G8B8);

	m_StartX = 5;
	m_StartY = 5;
	m_Board.x = (1024 - m_Width) / 2;
	m_Board.y = 400;
	m_Board.length = 8;
	m_Board.color = 7;

	m_Ball.x = m_Board.x + 40;
	m_Ball.y = m_Board.y - 15;
	m_Ball.vx = -5;
	m_Ball.vy = -5;
	InitBlock();
	m_GameState = 0;
	return TRUE;
}

BOOL CBlockApp::Frame()
{
	static int nPlayerScore = 0;
	static DWORD UpdateTimer = timeGetTime();
	if (timeGetTime() - UpdateTimer < 17)
		return TRUE;
	UpdateTimer = timeGetTime();

	m_Keyboard.Read();
	if (m_Keyboard.GetKeyState(KEY_F1)) {
		nPlayerScore = 0;
		m_Ball.x = m_Board.x + 40;
		m_Ball.y = m_Board.y - 15;
		m_Ball.vx = -5;
		m_Ball.vy = -5;
		InitBlock();
		m_GameState = 0;
		m_Keyboard.SetLock(KEY_F1, TRUE);
	}
	if (m_Ball.x<0 || m_Ball.x + 16>int(m_Width - 5))
	{
		PlaySound(1);
		m_Ball.vx = -m_Ball.vx;
	}

	if (m_Ball.y<0)
	{
		PlaySound(1);
		m_Ball.vy = -m_Ball.vy;
	}
	if (m_Ball.y + 16>int(m_Height - 30) && m_GameState == 0)
	{
		m_Ball.y = m_Height;
		m_GameState = 1;
		PlaySound(3);
	}

	if (m_Ball.y + 16 >= m_Board.y && m_Ball.y < int(m_Height - 30) && 
		m_Ball.x>m_Board.x - 16 && m_Ball.x < m_Board.x + m_Board.length*WIDTH)//m_Ball.y<m_Height-30
		//m_Ball.x>m_Board.x&&m_Ball.x+16<m_Board.x+m_Board.length*WIDTH
	{
		PlaySound(1);
		m_Ball.y = m_Board.y - 16;
		m_Ball.vy = -m_Ball.vy;
	}

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			if (m_Ball.x > m_Block[i][j].x && m_Ball.x<m_Block[i][j].x + WIDTH &&
				m_Ball.y>m_Block[i][j].y && m_Ball.y < m_Block[i][j].y + HEIGHT)
			{
				if (!m_Block[i][j].destroy)
				{
					PlaySound(1);
					m_Block[i][j].destroy = true;
					nPlayerScore += 10;
					m_Ball.vy = -m_Ball.vy;
				}
			}
		}
	}

	int nNoDestroyNum = 0;
	for (int i = 0; i < ROWS; i++)
		for (int j = 0; j < COLUMNS; j++)
			if (!m_Block[i][j].destroy)
				nNoDestroyNum++;

	if (nNoDestroyNum == 0 && m_GameState == 0)
	{
		m_GameState = 2;
		PlaySound(2);
	}

	m_Ball.x += m_Ball.vx;
	m_Ball.y += m_Ball.vy;

	if (m_Keyboard.GetKeyState(KEY_A) && m_Board.x >= 10)
		m_Board.x -= 10;
	if (m_Keyboard.GetKeyState(KEY_D) && m_Board.x <= 625 - m_Board.length*WIDTH)
		m_Board.x += 10;

	m_Graphics.Clear();
	m_Graphics.BeginScene();
	m_Graphics.BeginSprite();

	DrawBlock();
	for (int i = 0; i < m_Board.length; i++)
		m_texRect.Blit(m_Board.x + i*WIDTH, m_Board.y, m_Board.color * 30, 0, 30, 30);

	m_texBall.Blit(m_Ball.x, m_Ball.y);

	m_Graphics.EndSprite();
	if (m_GameState != 0)
	{
		m_Ball.vx = 0;
		m_Ball.vy = 0;
		std::wstringstream ss;
		ss << L"Game Over\nPress F1\nTo Continue";
		m_Font.Print(ss.str().c_str(), 30, 200);
	}
	std::wstringstream ssScore;
	ssScore << L"µÃ·Ö: " << nPlayerScore;
	m_Font.Print(ssScore.str().c_str(), 30, 400);
	m_Graphics.EndScene();
	m_Graphics.Display();
	return TRUE;
}

void CBlockApp::InitBlock()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			m_Block[i][j].x = m_StartX + WIDTH*j + GAP;
			m_Block[i][j].y = m_StartY + HEIGHT*i + GAP;
			m_Block[i][j].color = rand() % 7;
			m_Block[i][j].destroy = false;
		}
	}
}
void CBlockApp::DrawBlock()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			if (!m_Block[i][j].destroy)
				m_texRect.Blit(m_Block[i][j].x, m_Block[i][j].y, m_Block[i][j].color * 30, 0, 30, 30);
		}
	}
}

bool CBlockApp::PlaySound(long Num)
{
	if (Num >= 0 && Num < NUM_SOUNDS)
	{
		m_SoundData.Free();
		if (m_SoundData.LoadWAV(strSoundFileName[Num]) == TRUE)
			m_SoundChannel.Play(&m_SoundData);
		return true;
	}
	return false;
}

BOOL CBlockApp::Shutdown()
{
	m_Graphics.Shutdown();
	return true;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
{
	CBlockApp App;
	return App.Run();
}
