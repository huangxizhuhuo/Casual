#include "SnakeApp.h"
#include <sstream>
#include <string>
#include <ctime>
#include <vector>
#include <functional>

#pragma comment(lib,"Core_d.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"strmiids.lib")

const int NUM_SOUNDS=4;
WCHAR* strSoundFileName[]=
{
	{L"../res/disappear.wav"},
	{L"../res/error.wav"},
	{L"../res/win.wav"},
	{L"../res/lose.wav"}

};
CSnakeApp::CSnakeApp()
{
	m_bGameOver=false;
	m_bBomb=false;
	m_Width  = 800;//40*40 20*15
	m_Height = 625;
	m_nFPS=0;
	m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	wcscpy_s(m_Class, L"SnakeClass");
	wcscpy_s(m_Caption, L"贪吃蛇");
}

CSnakeApp::~CSnakeApp()
{
}

BOOL CSnakeApp::Init()
{
	srand((unsigned)time(NULL));
	MoveWindow(GethWnd(),(1024-m_Width)/2,(768-m_Height)/2,m_Width,m_Height,TRUE);		
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
	m_Input.Init(GethWnd(),GethInst());
	m_Keyboard.Create(&m_Input,KEYBOARD,1);
	//m_Mouse.Create(&m_Input,MOUSE,1);
	m_Font.Create(&m_Graphics,L"Arial");
	m_Sound.Init(GethWnd(), 22050, 1, 16);
	m_SoundChannel.Create(&m_Sound, 22050, 1, 16);
	m_texSnake.Load(&m_Graphics,L"../res/snake.png");
	m_texRed.Load(&m_Graphics,L"../res/red1.bmp",0xFFFFFFFF,D3DFMT_A8R8G8B8);
	m_texBomb.Load(&m_Graphics,L"../res/bomb.png",0xFFFF00FF,D3DFMT_A8R8G8B8);

	InitFood();
	SnakeUnit t;
	t.x=10;
	t.y=6;
	m_nDirection=1;
	m_Snake.push_back(t);
	return TRUE;
}

BOOL CSnakeApp::Frame()
{
	static int nPlayerScore=0;
	static DWORD UpdateTimer = timeGetTime();
    if(timeGetTime() < UpdateTimer + 150)
        return TRUE;
    UpdateTimer = timeGetTime();
	static DWORD dwOldTime=timeGetTime();
	   
	m_Keyboard.Read();
	//m_Mouse.Read();

	static int nFrame=0;

	if(m_Keyboard.GetKeyState(KEY_F1)==TRUE)
	{
		InitFood();
		m_Snake.clear();

		SnakeUnit t;
		t.x=10;
		t.y=6;
		m_nDirection=1;
		m_Snake.push_back(t);

		m_bGameOver=false;
		m_bBomb=false;
		nPlayerScore=0;
		
		nFrame=0;

		m_Keyboard.SetLock(KEY_F1,TRUE);
	}

	if(m_Keyboard.GetKeyState(KEY_W)==TRUE&&m_nDirection!=0)
	{
		m_nDirection=1;
		m_Keyboard.SetLock(KEY_W,TRUE);
	}
	if(m_Keyboard.GetKeyState(KEY_S)==TRUE&&m_nDirection!=0)
	{
		m_nDirection=2;
		m_Keyboard.SetLock(KEY_S,TRUE);
	}
	if(m_Keyboard.GetKeyState(KEY_A)==TRUE&&m_nDirection!=0)
	{
		m_nDirection=3;
		m_Keyboard.SetLock(KEY_A,TRUE);
	}
	if(m_Keyboard.GetKeyState(KEY_D)==TRUE&&m_nDirection!=0)
	{
		m_nDirection=4;
		m_Keyboard.SetLock(KEY_D,TRUE);
	}

	if((m_Snake[0].x<0||m_Snake[0].x>19||m_Snake[0].y<0||m_Snake[0].y>14)&&!m_bGameOver)
	{
		m_bGameOver=true;
		m_Scores.push_back(nPlayerScore);
		m_Scores.sort(std::greater<int>());
		if(nPlayerScore==m_Scores.front())
			PlaySound(2);
		else
			PlaySound(3);
	}

	
	for(int i=0;i<5;i++)
	{
		if(m_Snake[0].x==m_Food[i].x&&m_Snake[0].y==m_Food[i].y)
		{
			if(m_Food[i].type==0)
			{
			    m_Food[i].x=-100;
		    	m_Food[i].y=-100;
		
			    InitFood();
		
		    	nPlayerScore+=10;
			
			    SnakeUnit t;
			    m_Snake.push_back(t);
			    PlaySound(0);
			}
			else
			{
				PlaySound(1);
				m_bBomb=true;
				m_nBombPosX=m_Food[i].x;
				m_nBombPosY=m_Food[i].y;
				m_bGameOver=true;
				m_Scores.push_back(nPlayerScore);
	        	m_Scores.sort(std::greater<int>());
		        if(nPlayerScore==m_Scores.front())
			        PlaySound(2);
		        else
			        PlaySound(3);

			}
		
		}	
	}

	m_Graphics.Clear();
	m_Graphics.BeginScene(); 
	m_Graphics.BeginSprite();
	
	Draw();

	if(m_bBomb)
	{
		nFrame++;
		m_texBomb.Blit(m_nBombPosX*40-40,m_nBombPosY*40-40,80*nFrame,40,80,128);
	}

	m_Graphics.EndSprite();

	if(m_bGameOver)
	{
		std::wstringstream ss;
		ss<<L"Game Over\nPress F1\nTo Continue";
		m_Font.Print(ss.str().c_str(),700,250);
		m_nDirection=0;
	}

	std::wstringstream ssScores, ssScore;
	ssScores<<L"积分榜:\n";
	int index=1;
	for(std::list<int>::iterator i=m_Scores.begin();i!=m_Scores.end();i++,++index)
	{
		ssScores<<index<<L": "<<*i<<L"\n";
		if(index>9)//仅列出前10位
			break;
	}
	m_Font.Print(ssScores.str().c_str(),700,50);

	ssScore<<L"得分: "<<nPlayerScore;
	m_Font.Print(ssScore.str().c_str(),700,440);
	   
#ifdef DEBUG
	stringstream ssCursorPos;
	ssCursorPos<<L"( "<<m_Mouse.GetXPos()<<L","<<m_Mouse.GetYPos()<<L" )";
	m_Font.Print(ssCursorPos.str(),0,0);
	m_nFPS++;
	static string strFPS;
	stringstream ssFPS;
	if(timeGetTime()-dwOldTime>1000)
	{
		ssFPS<<m_nFPS;
		strFPS=ssFPS.str();
		m_nFPS=0;
		dwOldTime=timeGetTime();
	}
	m_Font.Print(strFPS,0,40);    
#endif
	
	m_Graphics.EndScene();	
    m_Graphics.Display();
	return TRUE;
}

void CSnakeApp::Draw()
{
	for(int i=0;i<20;i++)
	{
		for(int j=0;j<15;j++)
		{
			m_texSnake.Blit(i*40,j*40,0,0,40,40);
		}
	}
	for(int i=0;i<5;i++)
	{
		if(m_Food[i].type==0)
		    m_texSnake.Blit(m_Food[i].x*40,m_Food[i].y*40,80,40);//Food
		else
			m_texSnake.Blit(m_Food[i].x*40,m_Food[i].y*40,0,80,40,40);//Bomb
	}
	

	if(m_nDirection)
	{
	    for(int i=(int)m_Snake.size()-1;i>0;i--)
	    {
		    m_Snake[i].x=m_Snake[i-1].x;
		    m_Snake[i].y=m_Snake[i-1].y;
			m_texRed.Blit(m_Snake[i].x*40,m_Snake[i].y*40);
		    /*m_texSnake.Blit(m_Snake[i].x*40,m_Snake[i].y*40,40,0,80,40);*/
	    }
	    switch(m_nDirection)
	    {
	    case 1:
		    m_Snake[0].y-=1;
		    break;
	    case 2:
		    m_Snake[0].y+=1;
		    break;
	    case 3:
		    m_Snake[0].x-=1;
		    break;
	    case 4:
	    	m_Snake[0].x+=1;
		    break;
	    }
	   /* m_texSnake.Blit(m_Snake[0].x*40,m_Snake[0].y*40,40,0,80,40);*/
		m_texRed.Blit(m_Snake[0].x*40,m_Snake[0].y*40);
	}
	else
	{
		 for(int i=0;i<(int)m_Snake.size();i++)	 
		    /*m_texSnake.Blit(m_Snake[i].x*40,m_Snake[i].y*40,40,0,80,40);*/
			m_texRed.Blit(m_Snake[i].x*40,m_Snake[i].y*40);
	}
}
void CSnakeApp::InitFood()
{
	std::vector<int> ints;
	for(int i=0;i<5;i++)//5个不同的随机数
	{
		int r;
		while(1)
		{
		    r=rand()%300;
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
	for(int i=0;i<(int)ints.size();i++)
	{
		m_Food[i].x=ints[i]%20;
		m_Food[i].y=ints[i]/20;
		if(rand()%2==0)
			m_Food[i].type=0;
		else
			m_Food[i].type=1;
	}
}
bool CSnakeApp::PlaySound(long Num)
{
	if(Num >=0 && Num < NUM_SOUNDS)
	{
		m_SoundData.Free();
		if(m_SoundData.LoadWAV(strSoundFileName[Num]) == TRUE)
			m_SoundChannel.Play(&m_SoundData);
		return true; 
	}
    return false;
}

BOOL CSnakeApp::Shutdown()
{
	m_Graphics.Shutdown();
	return true;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)                           
{
	CSnakeApp App; 
	return App.Run();
}

