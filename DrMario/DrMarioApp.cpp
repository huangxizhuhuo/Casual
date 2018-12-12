#include "DrMarioApp.h"
#include <sstream>
#include <ctime>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "Core_d.lib")

int g_CurrentMusic = -1;
WCHAR* strSoundFileName[] =
{
	{ L"../res/disappear.wav" }
};
WCHAR* strMusicFileName[] =
{
	{ L"../res/drm_music_fever.mid" },
	{ L"../res/victory.mid" }
};


CDrMarioApp::CDrMarioApp()
{
	m_Width  = 640;
    m_Height = 480;
    m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;                                  
    wcscpy_s(m_Class, L"BlockClass");
	wcscpy_s(m_Caption, L"玛丽医生");
	m_Virus  = NULL;
}

BOOL CDrMarioApp::Init()
{
	srand((unsigned)time(NULL));
	MoveWindow(GethWnd(),(1024-m_Width)/2,(768-m_Height)/2,m_Width,m_Height,TRUE);		
	m_Graphics.Init();
	m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
	m_Input.Init(GethWnd(),GethInst());
	m_Keyboard.Create(&m_Input,KEYBOARD,1);
	m_Font.Create(&m_Graphics,L"Arial");
	m_Sound.Init(GethWnd(), 22050, 1, SIDELENGTH);
	m_SoundChannel.Create(&m_Sound, 22050, 1, SIDELENGTH);
	//m_MusicChannel.Create(&m_Sound);

	m_texRect.Load(&m_Graphics,L"../res/rect.png");
	m_texDrMario.Load(&m_Graphics,L"../res/DrMario.bmp",0xFF000000,D3DFMT_A1R5G5B5);
	
	m_nLevel=0;
	m_nTopScore=10000;
	m_nPlayerScore=0;

	int num=0;
	for(int i=0;i<NUM_ROWS;i++)
	{
		for(int j=0;j<NUM_COLUMNS-3;j++)
		{	
			std::vector<Pos> vec;
			for(int k=0;k<4;k++)
			{
			     a[i][j+k][num]=true;
				 vec.push_back(Pos(i,j+k));   	 
			}
			m_Vectors.push_back(vec);			
			num++;
		}
	}
	NewLevel();
	return TRUE;
}

BOOL CDrMarioApp::Frame()
{
	static DWORD UpdateTimer=timeGetTime();
	if(timeGetTime()-UpdateTimer<17)//60FPS
		return TRUE;
	UpdateTimer=timeGetTime();
	if(m_bSingleTouch&&!m_bNextLevel&&!m_bGameOver)
	    ProcessInput();

	static int nFrame=0;
	nFrame++;
	bool bFalling=true;
	if(nFrame%m_nSpeed==0&&m_bSingleTouch&&!m_bNextLevel&&!m_bGameOver)
	{
		if((m_Capsule.direction==0||m_Capsule.direction==2)&&m_Capsule.y<NUM_COLUMNS-1)
		{
			if(Container[m_Capsule.x][m_Capsule.y+1].color!=0||\
				Container[m_Capsule.x+1][m_Capsule.y+1].color!=0)	
				bFalling=false;
		}
		else if((m_Capsule.direction==1||m_Capsule.direction==3)&&m_Capsule.y<NUM_COLUMNS-2)
		{
			if(Container[m_Capsule.x][m_Capsule.y+2].color!=0)
					bFalling=false;
		}
		else//////////
			bFalling=false;

		if(bFalling)
		{
			m_Capsule.y++;
			m_bTouch=false;
		}	
	}
	std::vector<Pos> vecPos1,vecPos2;
	
	
	if(!bFalling&&!m_bTouch)//&&m_bSingleTouch)
	{
		m_bTouch=true;
		int x1,y1,x2,y2;
		switch(m_Capsule.direction)//拷贝至Container数组
		{
		case 0:
			Container[m_Capsule.x][m_Capsule.y].color=m_Capsule.color1;
			Container[m_Capsule.x][m_Capsule.y].direction=2;
			Container[m_Capsule.x][m_Capsule.y].num=m_nCapsuleNum;
			Container[m_Capsule.x+1][m_Capsule.y].color=m_Capsule.color2;
			Container[m_Capsule.x+1][m_Capsule.y].direction=3;
			Container[m_Capsule.x+1][m_Capsule.y].num=m_nCapsuleNum;
			x1=m_Capsule.x;
			y1=m_Capsule.y;
			x2=m_Capsule.x+1;
			y2=m_Capsule.y;
			break;
		case 2:
			Container[m_Capsule.x+1][m_Capsule.y].color=m_Capsule.color1;
			Container[m_Capsule.x+1][m_Capsule.y].direction=3;
			Container[m_Capsule.x+1][m_Capsule.y].num=m_nCapsuleNum;
			Container[m_Capsule.x][m_Capsule.y].color=m_Capsule.color2;
			Container[m_Capsule.x][m_Capsule.y].direction=2;
			Container[m_Capsule.x][m_Capsule.y].num=m_nCapsuleNum;
			x1=m_Capsule.x+1;
			y1=m_Capsule.y;
			x2=m_Capsule.x;
			y2=m_Capsule.y;
			break;
		case 1:
			Container[m_Capsule.x][m_Capsule.y].color=m_Capsule.color1;
			Container[m_Capsule.x][m_Capsule.y].direction=0;
			Container[m_Capsule.x][m_Capsule.y].num=m_nCapsuleNum;
			Container[m_Capsule.x][m_Capsule.y+1].color=m_Capsule.color2;
			Container[m_Capsule.x][m_Capsule.y+1].direction=1;
			Container[m_Capsule.x][m_Capsule.y+1].num=m_nCapsuleNum;
			x1=m_Capsule.x;
			y1=m_Capsule.y;
			x2=m_Capsule.x;
			y2=m_Capsule.y+1;
			break;//
		case 3:
			Container[m_Capsule.x][m_Capsule.y].color=m_Capsule.color2;
			Container[m_Capsule.x][m_Capsule.y].direction=0;
			Container[m_Capsule.x][m_Capsule.y].num=m_nCapsuleNum;
			Container[m_Capsule.x][m_Capsule.y+1].color=m_Capsule.color1;
			Container[m_Capsule.x][m_Capsule.y+1].direction=1;	
			Container[m_Capsule.x][m_Capsule.y+1].num=m_nCapsuleNum;
			x1=m_Capsule.x;
			y1=m_Capsule.y;
			x2=m_Capsule.x;
			y2=m_Capsule.y+1;
			break;
		
		}
		for(int m=0;m<NUM_POSSIBILITY;m++)										
			if(a[x1][y1][m])													
            	b[m]++;
		for(int m=0;m<NUM_POSSIBILITY;m++)										
            if(a[x2][y2][m])													
            	b[m]++;	
		vecPos1=ProcessCounteract();
																		
		m_Capsule=m_NextCapsule;		
		m_Capsule.x=3;		
		m_Capsule.y=-1;		
		GenerateNextCapsule();			
	}

	static int nNumFrame=0;
	nNumFrame++;
	//如果一个单独方块，架在其上的一切都要跟着下移，直到下降遇到阻碍为止，整体停止
	static bool bEnter=false;
	if(nNumFrame%10==0)
	{
		static int num=0;	
		
		for(int j=NUM_COLUMNS-1;j>=0;j--)
		{		
			for(int i=NUM_ROWS-1;i>=0;i--)//单独方块的下降					
			{		
				int k=j;
		    	
				if( Container[i][k].direction==4&&Container[i][k+1].color==0&&k<NUM_COLUMNS-1)
				{//独					
					num++;	
					bEnter=true;
					Container[i][k+1]=Container[i][k];	
					for(int m=0;m<NUM_POSSIBILITY;m++)										
						if(a[i][k+1][m])													
	            			b[m]++;
					Container[i][k]=Grid();	
					for(int m=0;m<NUM_POSSIBILITY;m++)										
						if(a[i][k][m])															
							b[m]--;				
				}
				if(k<NUM_COLUMNS-2&&Container[i][k].direction==0&&Container[i][k+1].direction==1&&\
					Container[i][k+2].color==0)
				{//上下
					num++;
					bEnter=true;
					Container[i][k+2]=Container[i][k+1];	
					for(int m=0;m<NUM_POSSIBILITY;m++)										
						if(a[i][k+2][m])													
	            			b[m]++;
					Container[i][k+1]=Container[i][k];	
					Container[i][k]=Grid();
					for(int m=0;m<NUM_POSSIBILITY;m++)										
						if(a[i][k][m])													
	            			b[m]--;
				}		
				if(i>0&&k<NUM_COLUMNS-1&&Container[i-1][k].direction==2&&Container[i][k].direction==3&&\
					Container[i-1][k+1].color==0&&Container[i][k+1].color==0)
				{//左右
					num++;
					bEnter=true;
					Container[i][k+1]=Container[i][k];
					for(int m=0;m<NUM_POSSIBILITY;m++)										
						if(a[i][k+1][m])													
            				b[m]++;
					Container[i-1][k+1]=Container[i-1][k];
					for(int m=0;m<NUM_POSSIBILITY;m++)										
						if(a[i-1][k+1][m])													
            				b[m]++;
  	
					Container[i][k]=Grid();
					for(int m=0;m<NUM_POSSIBILITY;m++)										
						if(a[i][k][m])													
            				b[m]--;
					Container[i-1][k]=Grid();
					for(int m=0;m<NUM_POSSIBILITY;m++)										
						if(a[i-1][k][m])													
							b[m]--;
				}		
			}	
		}	
		if(num!=0)
		{
			m_bSingleTouch=false;
			num=0;
		}
		else
		{
			m_bSingleTouch=true;
			vecPos2=ProcessCounteract();
		}
	}
	
	m_Graphics.Clear();
	m_Graphics.BeginScene(); 	
	m_Graphics.BeginSprite();
	DrawContainer(CONTAINERX1,CONTAINERX2);//画容器
	
	DrawCapsule(m_Capsule);//画移动中的胶囊
	DrawCapsule(m_NextCapsule);//画下一个胶囊
	DrawVirus();//画病毒
	for(int i=0;i<NUM_ROWS;i++)//画已落下的胶囊
	{
		for(int j=0;j<NUM_COLUMNS;j++)
		{
			if(Container[i][j].color!=0&&IsNotInVector(Pos(i,j),m_VirusPos))//不是病毒
				m_texDrMario.Blit(CONTAINERX1+i*SIDELENGTH,CONTAINERY1+j*SIDELENGTH,\
				(Container[i][j].color-1)*SIDELENGTH,Container[i][j].direction*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		}
	}
	
	static std::vector<Pos> temp;
	static DWORD theCurrentTime;
	if(vecPos1.size()!=0)
	{
		temp=vecPos1;
		theCurrentTime=timeGetTime();	
	}
	if(vecPos2.size()!=0)
	{
		temp=vecPos2;
		theCurrentTime=timeGetTime();
	}
	if(temp.size()!=0)
	{
		for(int i=0;i<(int)temp.size();i++)//画一下消去
		{
			m_texDrMario.Blit(CONTAINERX1+temp[i].x*SIDELENGTH,CONTAINERY1+temp[i].y*SIDELENGTH,
				(temp[i].color-1)*SIDELENGTH,5*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		}
		if(timeGetTime()-theCurrentTime>50)
		{
			temp=vecPos1;
			theCurrentTime=timeGetTime();
		}
	}	
	m_Graphics.EndSprite();
	PrintInformation();

	if(m_VirusPos.size()==0)
	{
		std::wstring str=L"StageClear\n\nTry Next\n\nPress Space";
		m_Font.Print(str.c_str(),CONTAINERX1+20,CONTAINERY1+40,0,0,D3DCOLOR_RGBA(0,0,255,255));
		m_bNextLevel=true;	
	}
	if(m_bNextLevel)
	{
		PlayMusic(1);
	    m_Keyboard.Read();
	    if(m_Keyboard.GetKeyState(KEY_SPACE))		
		{		
			m_nLevel++;		
			NewLevel();	
			m_Keyboard.SetLock(KEY_SPACE,TRUE);
		}
	}
	for(int i=3;i<5;i++)
	{	
		if(Container[i][0].color==1)
		{
			std::wstring str=L"Game Over";
		    m_Font.Print(str.c_str(),CONTAINERX1+20,CONTAINERY1+80,0,0,D3DCOLOR_RGBA(0,0,255,255));
			m_bGameOver=true;
		}
	}
	if(m_bGameOver)
	{
	}

	m_Graphics.EndScene();	
    m_Graphics.Display();
	return TRUE;
}
void CDrMarioApp::DrawContainer(int x1,int x2)
{
	for(int i=0;i<(int)(m_Height-30-CONTAINERY1)/30+1;i++)
	{
		m_texRect.Blit(x1-5,CONTAINERY1+i*30,7*30,0,5,30);	
		m_texRect.Blit(x2,  CONTAINERY1+i*30,7*30,0,5,30);	
	}
	m_texRect.Blit(x1+30,CONTAINERY1-35,7*30,0,5,30);	
	m_texRect.Blit(x2-35,CONTAINERY1-35,7*30,0,5,30);

	for(int i=0;i<(CONTAINERX2-CONTAINERX1)/30;i++)
		m_texRect.Blit(x1+5+i*30,448,  7*30,0,30,5);	
	m_texRect.Blit(x1,   CONTAINERY1-5,7*30,0,30,5);	
	m_texRect.Blit(x2-30,CONTAINERY1-5,7*30,0,30,5);
}
void CDrMarioApp::NewLevel()
{
	memset(b,0,sizeof(int)*NUM_POSSIBILITY);
	memset(Container,0,sizeof(Grid)*NUM_ROWS*NUM_COLUMNS);
	m_nCapsuleNum=0;

	m_Capsule.color1=rand()%3+1;
	m_Capsule.color2=rand()%3+1;
	m_Capsule.x=3;
	m_Capsule.y=-1;
	m_Capsule.direction=0;
	GenerateNextCapsule();

	m_bSingleTouch=true;
	m_bGameOver=false;
	m_bNextLevel=false;
	
	m_nVirusNum=(m_nLevel+1)*4;
	m_nSpeed=20;//中速,10快速,30慢速
	InitVirus();
	for(int i=0;i<m_nVirusNum;i++)
	{
		Container[m_Virus[i].x][m_Virus[i].y].color=m_Virus[i].color;
		m_VirusPos.push_back(Pos(m_Virus[i].x,m_Virus[i].y));
		for(int j=0;j<NUM_POSSIBILITY;j++)											
			if(a[m_Virus[i].x][m_Virus[i].y][j])															
				b[j]++;//四个细菌同色并且同列叠加的几率很小														
	}
	PlayMusic(0);
}
void CDrMarioApp::InitVirus()
{
	if(m_Virus!=NULL)
		delete m_Virus;
	m_Virus=new Virus[m_nVirusNum];
	std::vector<int> ints;
	for(int i=0;i<m_nVirusNum;i++)
	{
		int r;
		while(1)
		{
		    //r=rand()%(NUM_ROWS*20)+(NUM_COLUMNS-20)*NUM_ROWS;//下20行有病毒，上5行没有
			r=rand()%(NUM_ROWS*(NUM_COLUMNS-4))+4*NUM_ROWS;//下20行有病毒，上5行没有
			int counter=0;
			for(size_t j=0;j<ints.size();j++)
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
	for(int i=0;i<m_nVirusNum;i++)
	{
		m_Virus[i].x=ints[i]%NUM_ROWS;
		m_Virus[i].y=ints[i]/NUM_ROWS;
		m_Virus[i].color=rand()%3+1;
		m_Virus[i].died=false;
	}
}
void CDrMarioApp::DrawVirus()
{
	static int nFrame=0;
	nFrame++;
	for(int i=0;i<m_nVirusNum;i++)
	{
		if(!m_Virus[i].died)
		{
			if(nFrame<30)
			{
				m_texDrMario.Blit(CONTAINERX1+m_Virus[i].x*SIDELENGTH,CONTAINERY1+m_Virus[i].y*SIDELENGTH,\
				    (m_Virus[i].color-1)*SIDELENGTH,6*SIDELENGTH,SIDELENGTH,SIDELENGTH);
			}
			else
			{
				m_texDrMario.Blit(CONTAINERX1+m_Virus[i].x*SIDELENGTH,CONTAINERY1+m_Virus[i].y*SIDELENGTH,\
					(m_Virus[i].color-1)*SIDELENGTH,7*SIDELENGTH,SIDELENGTH,SIDELENGTH);
				if(nFrame>60)
					nFrame=0;
			}
		}
	}
	for(int i=0;i<m_nVirusNum;i++)
	{
		if(!m_Virus[i].died)
		{
			if(m_Virus[i].color==1)
			{
				if(nFrame<30)
				    m_texDrMario.Blit(CONTAINERX1-60,CONTAINERY1+200,\
				        0,6*SIDELENGTH,SIDELENGTH,SIDELENGTH);
				else
				{
					m_texDrMario.Blit(CONTAINERX1-60,CONTAINERY1+200,\
				        0,7*SIDELENGTH,SIDELENGTH,SIDELENGTH);
					if(nFrame>60)
						nFrame=0;
				}
			}
			else if(m_Virus[i].color==2)
			{
				if(nFrame<30)
				    m_texDrMario.Blit(CONTAINERX1-60,CONTAINERY1+220,\
				        SIDELENGTH,6*SIDELENGTH,SIDELENGTH,SIDELENGTH);
				else
				{
					m_texDrMario.Blit(CONTAINERX1-60,CONTAINERY1+220,\
				        SIDELENGTH,7*SIDELENGTH,SIDELENGTH,SIDELENGTH);
					if(nFrame>60)
						nFrame=0;
				}
			}
			else if(m_Virus[i].color==3)
			{
				if(nFrame<30)
				    m_texDrMario.Blit(CONTAINERX1-60,CONTAINERY1+240,\
				        2*SIDELENGTH,6*SIDELENGTH,SIDELENGTH,SIDELENGTH);
				else
				{
					m_texDrMario.Blit(CONTAINERX1-60,CONTAINERY1+240,\
				        2*SIDELENGTH,7*SIDELENGTH,SIDELENGTH,SIDELENGTH);
					if(nFrame>60)
						nFrame=0;
				}
				
			}
		}
	}
}
void CDrMarioApp::GenerateNextCapsule()
{
	m_NextCapsule.color1=rand()%3+1;
	m_NextCapsule.color2=rand()%3+1;
	m_NextCapsule.x=12;
	m_NextCapsule.y=5;
	m_NextCapsule.direction=0;//左右
	m_nCapsuleNum++;
}
void CDrMarioApp::DrawCapsule(const Capsule& capsule)
{//color 1,2,3   direction 0,1,2,3
	switch(capsule.direction)
	{
	case 0://←→
	    m_texDrMario.Blit(CONTAINERX1+capsule.x*SIDELENGTH,CONTAINERY1+capsule.y*SIDELENGTH,\
			(capsule.color1-1)*SIDELENGTH,2*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		m_texDrMario.Blit(CONTAINERX1+capsule.x*SIDELENGTH+SIDELENGTH,CONTAINERY1+capsule.y*SIDELENGTH,\
			(capsule.color2-1)*SIDELENGTH,3*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		break;
	case 1://↑↓     
		m_texDrMario.Blit(CONTAINERX1+capsule.x*SIDELENGTH,CONTAINERY1+capsule.y*SIDELENGTH,\
			(capsule.color1-1)*SIDELENGTH,0*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		m_texDrMario.Blit(CONTAINERX1+capsule.x*SIDELENGTH,CONTAINERY1+capsule.y*SIDELENGTH+SIDELENGTH,\
			(capsule.color2-1)*SIDELENGTH,1*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		break;
	case 2://→←
		m_texDrMario.Blit(CONTAINERX1+capsule.x*SIDELENGTH+SIDELENGTH,CONTAINERY1+capsule.y*SIDELENGTH,\
			(capsule.color1-1)*SIDELENGTH,3*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		m_texDrMario.Blit(CONTAINERX1+capsule.x*SIDELENGTH,CONTAINERY1+capsule.y*SIDELENGTH,\
			(capsule.color2-1)*SIDELENGTH,2*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		break;
	case 3://↓↑  
		m_texDrMario.Blit(CONTAINERX1+capsule.x*SIDELENGTH,CONTAINERY1+capsule.y*SIDELENGTH+SIDELENGTH,\
			(capsule.color1-1)*SIDELENGTH,1*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		m_texDrMario.Blit(CONTAINERX1+capsule.x*SIDELENGTH,CONTAINERY1+capsule.y*SIDELENGTH,\
			(capsule.color2-1)*SIDELENGTH,0*SIDELENGTH,SIDELENGTH,SIDELENGTH);
		break;
	}
}
bool CDrMarioApp::IsTheSameColor(std::vector<Pos>& vec)
{
	int k[4];	 
	k[0]=Container[vec[0].x][vec[0].y].color;
	k[1]=Container[vec[1].x][vec[1].y].color;
	k[2]=Container[vec[2].x][vec[2].y].color;
	k[3]=Container[vec[3].x][vec[3].y].color;
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(k[j]!=k[i])
				return false;
		}
	}
	return true;
}
bool CDrMarioApp::IsNotInVector(Pos p, std::vector<Pos>& vec)
{
	for(size_t i=0; i < vec.size(); i++)
	{
		if(p==vec[i])
			return false;
	}
	return true;
}
bool CDrMarioApp::IsNotInVector(int n,std::vector<int>& vec)
{
	for(int i=0;i<(int)vec.size();i++)
	{
		if(n==vec[i])
			return false;
	}
	return true;
}
void CDrMarioApp::ProcessInput()
{
	m_Keyboard.Read();
	if(m_Keyboard.GetKeyState(KEY_F1))//
	{
		m_nPlayerScore=0;
		m_Keyboard.SetLock(KEY_F1,TRUE);
	}
	//------------------------------------------------------------------------------------
	if(m_Keyboard.GetKeyState(KEY_A))//左移
	{
		if((m_Capsule.direction==0||m_Capsule.direction==2)&&\
			Container[m_Capsule.x-1][m_Capsule.y].color==0&&m_Capsule.x>0)
		    m_Capsule.x-=1;
		else if((m_Capsule.direction==1||m_Capsule.direction==3)&&\
			Container[m_Capsule.x-1][m_Capsule.y].color==0&&\
			Container[m_Capsule.x-1][m_Capsule.y+1].color==0&&m_Capsule.x>0)
			m_Capsule.x-=1;
		m_Keyboard.SetLock(KEY_A,TRUE);
	}
	if(m_Keyboard.GetKeyState(KEY_D))//右移
	{
		if((m_Capsule.direction==0||m_Capsule.direction==2)&&\
			Container[m_Capsule.x+2][m_Capsule.y].color==0&&m_Capsule.x<NUM_ROWS-2)
		    m_Capsule.x+=1;
		else if((m_Capsule.direction==1||m_Capsule.direction==3)\
			&&Container[m_Capsule.x+1][m_Capsule.y].color==0&&\
			Container[m_Capsule.x+1][m_Capsule.y+1].color==0&&m_Capsule.x<NUM_ROWS-1)
			m_Capsule.x+=1;
		m_Keyboard.SetLock(KEY_D,TRUE);
	}
	if(m_Keyboard.GetKeyState(KEY_ENTER))//旋转
	{
		if(m_Capsule.direction+1>3)
			m_Capsule.direction=0;
		else
			m_Capsule.direction++;
		bool bRotate=true;
		if(m_Capsule.y<NUM_COLUMNS-1&&m_Capsule.x>0&&m_Capsule.x<NUM_ROWS-1)
		{
			switch(m_Capsule.direction)
			{
			case 0://由下上旋转而来
				if(Container[m_Capsule.x-1][m_Capsule.y].color!=0)
					bRotate=false;
				break;
			case 1:
			case 3:
				if(Container[m_Capsule.x][m_Capsule.y+1].color!=0)
					bRotate=false;
				break;
			case 2:
				if(Container[m_Capsule.x+1][m_Capsule.y].color!=0)
					bRotate=false;
				break;
			}
		}
		else
			bRotate=false;
		if(!bRotate)
		{
			if(m_Capsule.direction==0)
				m_Capsule.direction=3;
			else
				m_Capsule.direction--;
		}
		m_Keyboard.SetLock(KEY_ENTER,TRUE);
	}

	if(m_Keyboard.GetKeyState(KEY_S))
	{	
		bool bFalling=true;
		if((m_Capsule.direction==0||m_Capsule.direction==2)&&m_Capsule.y<NUM_COLUMNS-1)
		{
			if(Container[m_Capsule.x][m_Capsule.y+1].color!=0||\
				Container[m_Capsule.x+1][m_Capsule.y+1].color!=0)	
				bFalling=false;
		}
		else if((m_Capsule.direction==1||m_Capsule.direction==3)&&m_Capsule.y<NUM_COLUMNS-2)
		{
			if(Container[m_Capsule.x][m_Capsule.y+2].color!=0)
					bFalling=false;
		}
		else//////////
			bFalling=false;
		if(bFalling)			
		{			
			m_Capsule.y++;	
			m_bTouch=false;
		}
	}
	//------------------------------------------------------------------------------------
}

std::vector<Pos> CDrMarioApp::ProcessCounteract()
{
	std::vector<Pos> vecPos;
	std::vector<int> ints;//一列有4个以上消掉
	for(int i=0;i<NUM_POSSIBILITY;i++)									
		if(b[i]==4)
			ints.push_back(i);
	//每次调用ProcessCounteract()，都可能会篡改m_vecPos，
	for(int i=0;i<(int)ints.size();i++)				
	{
		if(IsTheSameColor(m_Vectors[ints[i]]))
		{
			PlaySound(0);
			
		    for(int j=0;j<4;j++)				
		    {										
			    int x=m_Vectors[ints[i]][j].x;					
			    int y=m_Vectors[ints[i]][j].y;	
				
				if(IsNotInVector(Pos(x,y),vecPos))		
				{
					vecPos.push_back(Pos(x,y,Container[x][y].color));		
				}
		    }
		}	
	}
	for(int i=0;i<(int)vecPos.size();i++)//处理消去的小块
	{    
		for(int j=0;j<NUM_POSSIBILITY;j++)										
			if(a[vecPos[i].x][vecPos[i].y][j])											
				b[j]--;
		if(!IsNotInVector(Pos(vecPos[i].x,vecPos[i].y),m_VirusPos))
		{
			for(std::vector<Pos>::iterator it=m_VirusPos.begin();it!=m_VirusPos.end();it++)
			{
				if((*it).x==vecPos[i].x&&(*it).y==vecPos[i].y)
				{
					
					for(int j=0;j<m_nVirusNum;j++)
					{
						if(m_Virus[j].x==(*it).x&&m_Virus[j].y==(*it).y)
						{
							m_Virus[j].died=true;
							m_nPlayerScore+=100;//杀掉一个细菌+100分
							break;
						}
					}
					m_VirusPos.erase(it);
					break;
				}
			}
		}
		Container[vecPos[i].x][vecPos[i].y]=Grid();
	}
	//另一半被消去，是独，在相邻位置（左右上下）找不到其同类
	for(int i=0;i<NUM_ROWS;i++)
	{
		for(int j=0;j<NUM_COLUMNS;j++)
		{
			if(i>0&&i<NUM_ROWS-1&&j<NUM_COLUMNS-1&&j>0)
			{
				if( Container[i][j].num!=Container[i-1][j].num&&\
					Container[i][j].num!=Container[i+1][j].num&&\
					Container[i][j].num!=Container[i][j+1].num&&\
					Container[i][j].num!=Container[i][j-1].num)	
					Container[i][j].direction=4;
			}
			else if(i==0)
			{
				if( Container[i][j].num!=Container[i+1][j].num&&\
					Container[i][j].num!=Container[i][j+1].num&&\
					Container[i][j].num!=Container[i][j-1].num)	
					Container[i][j].direction=4;
			}
			else if(i==NUM_ROWS-1)
			{
				if( Container[i][j].num!=Container[i-1][j].num&&\
					Container[i][j].num!=Container[i][j+1].num&&\
					Container[i][j].num!=Container[i][j-1].num)	
					Container[i][j].direction=4;
			}
			else if(j==0)
			{
				if( Container[i][j].num!=Container[i-1][j].num&&\
					Container[i][j].num!=Container[i+1][j].num&&\
					Container[i][j].num!=Container[i][j+1].num)	
					Container[i][j].direction=4;
			}
			else if(j==NUM_COLUMNS-1)
			{
				if( Container[i][j].num!=Container[i-1][j].num&&\
					Container[i][j].num!=Container[i+1][j].num&&\
					Container[i][j].num!=Container[i][j-1].num)	
					Container[i][j].direction=4;
			}

		}
	}
	return vecPos;
}
bool CDrMarioApp::PlayMusic(long Num)
{
  /*  if(g_CurrentMusic == Num)
        return TRUE;
    m_MusicChannel.Stop();
    m_MusicChannel.Free();
    //DWORD Timer = timeGetTime() + 700;
    //while(timeGetTime() < Timer)//足够的时间逐渐减小音量
    //{
    //    DWORD Level = (Timer - timeGetTime())/10;
    //    m_MusicChannel.SetVolume(Level);
    //}
    m_MusicChannel.Load(strMusicFileName[Num]);
    m_MusicChannel.Play(70,0);
    g_CurrentMusic = Num;*/
    return TRUE;
}

bool CDrMarioApp::StopMusic()
{
   // m_MusicChannel.Stop();
    //m_MusicChannel.Free();
    g_CurrentMusic = -1;
    return TRUE;
}

CDrMarioApp::~CDrMarioApp()
{
}
BOOL CDrMarioApp::Shutdown()
{
	m_Graphics.Shutdown();
	return true;
}
bool CDrMarioApp::PlaySound(long Num)
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
void CDrMarioApp::PrintInformation()
{
	std::wstringstream ssTopScore;
	ssTopScore<<L"TOP\n"<<m_nTopScore;
	m_Font.Print(ssTopScore.str().c_str(),16,CONTAINERY1+30,0,0,D3DCOLOR_RGBA(255,0,0,255));
	std::wstringstream ssScore;
	ssScore<<L"SCORE\n"<<m_nPlayerScore;
	m_Font.Print(ssScore.str().c_str(),   16,CONTAINERY1+80,0,0,D3DCOLOR_RGBA(255,0,0,255));
	std::wstringstream ssLevel;
	ssLevel<<L"LEVEL\n"<<m_nLevel;
	m_Font.Print(ssLevel.str().c_str(),   CONTAINERX2+48,CONTAINERY1+120,0,0,D3DCOLOR_RGBA(255,0,0,255));
	std::wstringstream ssSpeed;
	
	std::wstring str;
	switch(m_nSpeed)
	{
	case 10:
		str=L"HI";
		break;
	case 20:
		str=L"MED";
		break;
	case 30:
		str=L"LOW";
		break;
	}
	ssSpeed<<L"SPEED\n"<<str;
	m_Font.Print(ssSpeed.str().c_str(),   CONTAINERX2+48,CONTAINERY1+170,0,0,D3DCOLOR_RGBA(255,0,0,255));
	std::wstringstream ssVirus;
	ssVirus<<L"VIRUS\n"<<m_VirusPos.size();
	m_Font.Print(ssVirus.str().c_str(),   CONTAINERX2+48,CONTAINERY1+220,0,0,D3DCOLOR_RGBA(255,0,0,255));
}
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)                           
{	  
	CDrMarioApp App; 
	return App.Run();
}