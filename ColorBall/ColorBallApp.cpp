#include "ColorBallApp.h"
#include <sstream>
#include <string>
#include <ctime>
#include <vector>
#include <functional>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "Core_d.lib")

wchar_t* strChessFileName[]=
{
	{L"../res/red.bmp"},
	{L"../res/pink.bmp" },
	{L"../res/skyblue.bmp" },
	{L"../res/blue.bmp" },
	{L"../res/orange.bmp" },
	{L"../res/brown.bmp" },
	{L"../res/green.bmp" }
};
const int NUM_SOUNDS=5;
wchar_t* strSoundFileName[] =
{
	{L"../res/error.wav" },
	{L"../res/press.wav" },
	{L"../res/disappear.wav" },
	{L"../res/win.wav" },
	{L"../res/lose.wav" }
};
int a[GRIDNUM][GRIDNUM];

int AS_Valid(_asNode* parent, _asNode* node, int data) 
{
	int x = node->x, y = node->y;
	if (x < 0 || y < 0 || x >= GRIDNUM || y >= GRIDNUM || a[x][y]!=0)
		return 0;
	return 1;
}

int AS_Cost(_asNode* parent, _asNode* node, int data) 
{
	int x = node->x, y = node->y;
	int cost = a[x][y]+1;	// Ensure always cost > 1
	return cost;
}

CColorBallApp::CColorBallApp()
{
	m_nCurPickChess=-1;
	//m_bMoving=false;//
	m_bGameOver=false;
	m_Width  = 680; 
    m_Height = 530;
	m_nFPS=0;
    m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;                                  
    wcscpy_s(m_Class, L"ColorBallClass");
	wcscpy_s(m_Caption, L"七色彩珠");
}

CColorBallApp::~CColorBallApp()
{
}

BOOL CColorBallApp::Init()
{
	//获得系统分辨率
	MoveWindow(GethWnd(),(1024-m_Width)/2,(768-m_Height)/2,m_Width,m_Height,TRUE);
	//ShowCursor(TRUE);
	srand((unsigned)time(NULL));
	
	m_ChessBoard.Init();//
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
	m_Input.Init(GethWnd(),GethInst());
	//m_Mouse.Create(&m_Input,MOUSE,1);
	m_Keyboard.Create(&m_Input,KEYBOARD,1);
	m_Font.Create(&m_Graphics,L"Arial");
	m_Sound.Init(GethWnd(), 22050, 1, 16);
    m_SoundChannel.Create(&m_Sound, 22050, 1, 16);

	m_texChessboard.Load(&m_Graphics,L"../res/Chessboard.bmp");
	m_texBlack.Load(&m_Graphics,L"../res/black.bmp",0xFF000000,D3DFMT_A1R5G5B5);//
	for(int i=0;i<7;i++)
	{
		m_texChesses[i].Load(&m_Graphics,strChessFileName[i],0xFF000000,D3DFMT_A1R5G5B5);//,D3DFMT_A8R8G8B8);
	}

	m_AStar.SetRows(GRIDNUM);//	
	m_AStar.udValid = AS_Valid;
	m_AStar.udCost  = AS_Cost ;
	return TRUE;
}

BOOL CColorBallApp::Frame()
{
	static int nPlayerScore=0;
	static DWORD UpdateTimer = timeGetTime();
    // Limit all frame updates to 30 fps
    if(timeGetTime() < UpdateTimer + 33)
        return TRUE;
    UpdateTimer = timeGetTime();

	for(int i=0;i<GRIDNUM;i++)//
		for(int j=0;j<GRIDNUM;j++)
			a[i][j]=m_ChessBoard.grid1[i][j].key;

	static DWORD dwOldTime=timeGetTime();
	
	//棋子移动时屏蔽输入    
	//m_Mouse.Read();    
	m_Keyboard.Read();

	if(m_Keyboard.GetKeyState(KEY_F1)==TRUE)////
	{
		m_bGameOver=false;
		m_ChessBoard.Init();
		nPlayerScore=0;
		PlaySound(3);
		m_Keyboard.SetLock(KEY_F1,TRUE);
	}
	//if(m_Mouse.GetButtonState(0)==TRUE)////
	if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{	
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(GethWnd(), &pt);
		int pos=m_ChessBoard.CursorOnWhichGrid(pt.x, pt.y);
		int key;
		if(m_ChessBoard.grid1[pos/GRIDNUM][pos%GRIDNUM].key!=0)
			m_nCurPickChess=pos;
		else if(m_nCurPickChess!=-1)//pos为空,算玩家的一次行动
		{
			//寻路
			key=m_ChessBoard.grid1[m_nCurPickChess/GRIDNUM][m_nCurPickChess%GRIDNUM].key;
			int x1,y1,x2,y2;
			x1=m_nCurPickChess/GRIDNUM;
			y1=m_nCurPickChess%GRIDNUM;
			x2=pos/GRIDNUM;
			y2=pos%GRIDNUM;
			m_AStar.GeneratePath(x1,y1,x2,y2,&m_ChessBoard);
            m_bestPath=m_AStar.GetBestNode();
			if(m_bestPath)
			{
				PlaySound(1);
			    m_ChessBoard.grid1[x2][y2].key=m_ChessBoard.grid1[x1][y1].key;
			    m_ChessBoard.grid1[x1][y1].key=0;//
			    m_nCurPickChess=-1;//无此行的奇怪现象 
				//判断棋盘形势，消掉五连以上棋子
				m_ChessBoard.JudgeChessBoardRemove(x1,y1);
				int nCounteractNum=m_ChessBoard.JudgeChessBoardAdd(x2,y2);
				if(nCounteractNum<=6)
				    nPlayerScore+=nCounteractNum*2;
				else if(nCounteractNum<=8)
					nPlayerScore+=nCounteractNum*3;
				else if(nCounteractNum<=10)
					nPlayerScore+=nCounteractNum*4;
				else
					nPlayerScore+=nCounteractNum*5;
					
				//是否消掉,是则玩家继续行动,否则添加三个棋子到随机位置	
				if(nCounteractNum==0)//这三个棋子的位置应一次性选出，
				{//还是每次选一个，如果电脑放置一个棋子导致抵消，这两者就不同了
				    for(int i=0;i<3;i++)
		            {		    
					    int key=m_ChessBoard.grid2[i].key;		    
					    int pos=m_ChessBoard.FindRandEmptyGrid();	
						if(pos==-1)//
						{
							//m_Scores.push_back(nPlayerScore);
							//m_bGameOver=true;
							break;
						}
					    m_ChessBoard.grid1[pos/GRIDNUM][pos%GRIDNUM].key=m_ChessBoard.grid2[i].key;	
						m_ChessBoard.JudgeChessBoardAdd(pos/GRIDNUM,pos%GRIDNUM);	
						PlaySound(1);
				    }
				    m_ChessBoard.GenerateThreeChesses(); 
				}//
				else
				{
					PlaySound(2);
				}
			}
			else
			{
				PlaySound(0);
			}

		}
		//m_Mouse.SetLock(0,TRUE);
	}	
	if((m_ChessBoard.FindRandEmptyGrid())==-1&&!m_bGameOver)
	{
		m_Scores.push_back(nPlayerScore);
		m_Scores.sort(std::greater<int>());
		if(nPlayerScore==m_Scores.front())
			PlaySound(3);
		else
			PlaySound(4);
		m_bGameOver=true;
		
	}
	m_Graphics.Clear();
	m_Graphics.BeginScene(); 
	m_Graphics.BeginSprite();
	DrawChessBoard();
	m_Graphics.EndSprite();
	std::wstringstream ssScroe, ssGameOver;
	if(m_bGameOver)
	{
		m_Font.Print(L"Game Over\nPress F1\nTo Continue",10,200);
	}
	
	std::wstringstream ssScores;
	ssScores<<L"积分榜:\n";
	int index=1;
	for(std::list<int>::iterator i=m_Scores.begin();i!=m_Scores.end();i++,++index)
	{
		ssScores<<index<<L": "<<*i<<L"\n";
		if(index>9)//仅列出前10位
			break;
	}
	m_Font.Print(ssScores.str().c_str(),570,50);
	
	ssScroe<<L"得分: "<<nPlayerScore;
	m_Font.Print(ssScroe.str().c_str(),570,440);
	   
#ifdef DEBUG
	stringstream ssCursorPos;
	ssCursorPos<<"( "<<m_Mouse.GetXPos()<<","<<m_Mouse.GetYPos()<<" )";
	m_Font.Print(ssCursorPos.str(),0,0);

	stringstream ssChess;
	for(int i=0;i<9;i++)//
	{
		for(int j=0;j<9;j++)
		{
			ssChess<<a[i][j]<<" ";
		}
		ssChess<<"\n";
	}
	m_Font.Print(ssChess.str(),0,100);
	//计算FPS
	m_nFPS++;
	static string strFPS;
	stringstream ssFPS;//
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

bool CColorBallApp::PlaySound(long Num)
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

void CColorBallApp::DrawChessBoard()
{
	//m_Graphics.BeginSprite();
	m_texChessboard.Blit((m_Width-430)/2,10);
	//m_Graphics.EndSprite();
	static int nTotalFrame=0;
	int k;
 	for(int i=0;i<GRIDNUM;i++)
	{
		for(int j=0;j<GRIDNUM;j++)
		{
		    if(m_nCurPickChess==(i*GRIDNUM+j))//当前Pick的棋子
		    {
			    k=m_ChessBoard.grid1[i][j].key;
				//m_Graphics.BeginSprite();
			    
				//m_Graphics.EndSprite();
				nTotalFrame++;
				if(nTotalFrame<=60&&nTotalFrame>30)
				{
					m_texChesses[k-1].Blit(m_ChessBoard.grid1[i][j].x,m_ChessBoard.grid1[i][j].y);
					if(nTotalFrame==60)
					    nTotalFrame=0;
				}
				else
				{
					m_texBlack.Blit(m_ChessBoard.grid1[i][j].x,m_ChessBoard.grid1[i][j].y);
				}
		    }
		    else if((k=m_ChessBoard.grid1[i][j].key)!=0)
	    	{
				//m_Graphics.BeginSprite();
			    m_texChesses[k-1].Blit(m_ChessBoard.grid1[i][j].x,m_ChessBoard.grid1[i][j].y);
				//m_Graphics.EndSprite();
		    }
		}
	}
	//m_Graphics.BeginSprite();
	for(int i=0;i<3;i++)
	{
		if((k=m_ChessBoard.grid2[i].key)!=0)
		{
			m_texChesses[k-1].Blit(m_ChessBoard.grid2[i].x,m_ChessBoard.grid2[i].y);
		}
	}
	//m_Graphics.EndSprite();
}
BOOL CColorBallApp::Shutdown()
{
	for(int i=0;i<7;i++)
		m_texChesses[i].Free();
	m_texBlack.Free();
	m_texChessboard.Free();
	m_Graphics.Shutdown();
	return true;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)                           
{	  
	CColorBallApp App; 
	return App.Run();
}
