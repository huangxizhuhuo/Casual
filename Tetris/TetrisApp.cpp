#include "TetrisApp.h"
#include <sstream>
#include <ctime>

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

CTetrisApp::CTetrisApp()
{
	m_bGameOver=false;
	m_Width  = 800;//40*40 20*15
    m_Height = 685;
	m_nFPS=0;
    m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;                                  
    wcscpy_s(m_Class, L"TetrisClass");
	wcscpy_s(m_Caption, L"俄罗斯方块");
}
CTetrisApp::~CTetrisApp()
{
}
BOOL CTetrisApp::Init()
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
	m_texRect.Load(&m_Graphics,L"../res/rect.png");
	m_bGameOver=false;
	m_bTouch=false;
	m_nNextType=rand()%7;
	m_nColor=rand()%7;
	GenerateBlock();
	memset(a,0,sizeof(int)*24*10);
	return TRUE;
}

BOOL CTetrisApp::Frame()
{
	static int nPlayerScore=0;
	//static DWORD dwOldTime=timeGetTime();
	static DWORD UpdateTimer = timeGetTime();
	if (timeGetTime() - UpdateTimer < 17)
		return TRUE;
	UpdateTimer = timeGetTime();

	m_Keyboard.Read();
	//m_Mouse.Read(); 
	if(m_Keyboard.GetKeyState(KEY_F1))
	{
		m_bGameOver=false;
		memset(a,0,sizeof(int)*24*10);
		nPlayerScore=0;
		m_nNextType=rand()%7;
	    GenerateBlock();
		m_Keyboard.SetLock(KEY_F1,TRUE);
	}
	if(m_Keyboard.GetKeyState(KEY_A)&&!m_bGameOver)
	{
		bool bLeft=true;
		for(int i=0;i<4;i++)				
		{					
			for(int j=0;j<4;j++)					
			{						
				if(r[m_nType][m_nDirection][i][j]==1)					
				{							
					if(m_nStartX+i<=0)
						bLeft=false;							
					if(a[m_nStartX+i-1][m_nStartY+j]==1)			
						bLeft=false;							
				}					
			}					
		}				
		if(bLeft)
			m_nStartX--;
		m_Keyboard.SetLock(KEY_A,TRUE);
	}
	if(m_Keyboard.GetKeyState(KEY_D)&&!m_bGameOver)
	{
		bool bRight=true;
		for(int i=0;i<4;i++)				
		{					
			for(int j=0;j<4;j++)					
			{						
				if(r[m_nType][m_nDirection][i][j]==1)//						
				{							
					if(m_nStartX+i>=9)
						bRight=false;							
					if(a[m_nStartX+i+1][m_nStartY+j]==1)			
						bRight=false;							
				}					
			}					
		}				
		if(bRight)
			m_nStartX++;
		m_Keyboard.SetLock(KEY_D,TRUE);
	}
	if(m_Keyboard.GetKeyState(KEY_S)&&!m_bGameOver)
	{
		bool bDown=true;
		for(int i=0;i<4;i++)				
		{					
			for(int j=0;j<4;j++)					
			{						
				if(r[m_nType][m_nDirection][i][j]==1)//						
				{							
					if(m_nStartY+j>=23)
						bDown=false;							
					if(a[m_nStartX+i][m_nStartY+j+1]==1)			
						bDown=false;							
				}					
			}					
		}				
		if(bDown)
			m_nStartY+=1;
		//m_Keyboard.SetLock(KEY_S,TRUE);
	}
	if(m_Keyboard.GetKeyState(KEY_ENTER)&&!m_bGameOver)
	{
		if(++m_nDirection>3)
			m_nDirection=0;
		bool m_bRotate=true;
		for(int i=0;i<4;i++)				
		{					
			for(int j=0;j<4;j++)					
			{						
				if(r[m_nType][m_nDirection][i][j]==1)					
				{							
					if(m_nStartY+j>=23||m_nStartY+j<0||m_nStartX+i>=9||m_nStartX+i<0)
						m_bRotate=false;							
					if(a[m_nStartX+i][m_nStartY+j]==1)			
						m_bRotate=false;							
				}					
			}					
		}
		if(!m_bRotate&&m_nDirection!=0)
			--m_nDirection;
		else if(!m_bRotate&&m_nDirection==0)
			m_nDirection=3;

		m_Keyboard.SetLock(KEY_ENTER,TRUE);
	}
	bool bFalling=true;
	static int nFrame=0;
	nFrame++;
	if(nFrame%15==0)
	{
		for(int i=0;i<4;i++)						
		{							
			for(int j=0;j<4;j++)								
			{									
				if(r[m_nType][m_nDirection][i][j]==1)								
				{											
					if(m_nStartY+j>=23)					
						bFalling=false;											
					if(a[m_nStartX+i][m_nStartY+j+1]==1)								
						bFalling=false;									
				}							
			}						
		}

		if(bFalling&&!m_bGameOver)
		{
			m_nStartY++;
			m_bTouch=false;
		}
	}
	if(!bFalling&&!m_bTouch&&!m_bGameOver)
	{
		m_bTouch=true;
		PlaySound(1);		
		for(int i=0;i<4;i++)			    
			for(int j=0;j<4;j++)		
				if(r[m_nType][m_nDirection][i][j]==1)			
					a[m_nStartX+i][m_nStartY+j] = r[m_nType][m_nDirection][i][j];
					
		GenerateBlock();
	}	
	int num[24]={0};//扫描出排成一行的方块
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<24;j++)
		{
			if(a[i][j]==1)
			{
				num[j]++;
			}
		}
	}
	for(int i=0;i<24;i++)//消掉方块并下移
	{
		if(num[i]==10)
		{
			PlaySound(0);
			nPlayerScore+=10;
			for(int j=0;j<10;j++)
			{
				a[j][i]=0;
			}
			for(int k=i;k>0;k--)
			{
				for(int l=0;l<10;l++)
				{
					a[l][k]=a[l][k-1];
				}
			}
		}
	}
	for(int i=0;i<10;i++)//最上方一行有一个方块GameOver
	{
		if(a[i][4]==1&&!m_bGameOver)
		{
			m_bGameOver=true;	
			PlaySound(3);
		}
	}

	m_Graphics.Clear();
	m_Graphics.BeginScene(); 	
	m_Graphics.BeginSprite();
	DrawContainer();
	DrawContainer(640,760,90,210);
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<24;j++)
		{
			if(a[i][j]==1)	
				m_texRect.Blit(250+i*30,-120+j*30,2*30,0,30,30);	
		}
	}
	DrawBlock(13,7,m_nNextType,0,m_nColor);	
	if(!m_bGameOver)  
		DrawBlock(m_nStartX,m_nStartY,m_nType,m_nDirection,m_nLastColor);	
	m_Graphics.EndSprite();
	if(m_bGameOver)
	{
		std::wstringstream ss;
		ss<<L"Game Over\nPress F1\nTo Continue";
		m_Font.Print(ss.str().c_str(),60,250);
	}
	std::wstringstream ssScore,ssNext;
	ssScore<<L"得分: "<<nPlayerScore;
	m_Font.Print(ssScore.str().c_str(),60,440);
	ssNext<<L"下一个";
	m_Font.Print(ssNext.str().c_str(),680,60);   
	m_Graphics.EndScene();	
    m_Graphics.Display();
	return TRUE;
}

void CTetrisApp::GenerateBlock()
{
	m_nType=m_nNextType;
	m_nNextType=rand()%7;
	m_nDirection=0;
	m_nLastColor=m_nColor;
	m_nColor=rand()%7;
	m_nStartX=3;
	m_nStartY=0;
	m_bTouch=false;
}
void CTetrisApp::DrawContainer(int x1,int x2,int y1,int y2)
{
	for(int i=0;i<(y2-y1)/30;i++)
	{
		m_texRect.Blit(x1-30,y1+i*30,7*30,0,30,30);	
		m_texRect.Blit(x2,y1+i*30,7*30,0,30,30);
	}
	for(int i=0;i<(x2-x1)/30+2;i++)
	{
		m_texRect.Blit(x1+(i-1)*30,y2,7*30,0,30,30);
	}
}
void CTetrisApp::DrawBlock(int x,int y,int type,int direction,int color)
{
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(r[type][direction][i][j]!=0)
			{
			    m_texRect.Blit(250+(x+i)*30,-120+(y+j)*30,color*30,0,30,30);				
			}	
		}	
	}		
}
bool CTetrisApp::PlaySound(long Num)
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
BOOL CTetrisApp::Shutdown()
{
	m_Graphics.Shutdown();
	return true;
}
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)                           
{	  
	CTetrisApp App; 
	return App.Run();
}

