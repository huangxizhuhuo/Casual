/*-------------------------------------------------------
GameCore Component
Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
2007-2008 SCY modified
-------------------------------------------------------*/
#include "Stdafx.h"
#include "Graphics.h"

CGraphics::CGraphics()
{
	m_hWnd = NULL;
    m_pD3D = NULL;
    m_pD3DDevice = NULL;
    m_pSprite = NULL;
    m_AmbientRed = m_AmbientGreen = m_AmbientBlue = (char)255;
    m_Width  = 0;
    m_Height = 0;
    m_BPP    = 0;
    m_Windowed = TRUE;
    m_ZBuffer  = FALSE;
    m_HAL      = FALSE;
}

CGraphics::~CGraphics()
{
	Shutdown();
}

BOOL CGraphics::Init()
{
    Shutdown();
    if((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
      return FALSE;
    return TRUE;
}

BOOL CGraphics::SetMode(HWND hWnd, BOOL Windowed, BOOL UseZBuffer, long Width, long Height, char BPP)
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DFORMAT             Format, AltFormat;
	RECT                  WndRect, ClientRect;
	long                  WndWidth, WndHeight;
	float                 Aspect;  
	if((m_hWnd = hWnd) == NULL)// Error checking  
		return FALSE;
	if(m_pD3D == NULL) 
		return FALSE;
	// Get the current display format
	if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3ddm)))   
		return FALSE;	
	if(!Width)// Configure width  
	{		   
		if(Windowed == FALSE)// Default to screen width if fullscreen 
		{  
			m_Width = m_d3ddm.Width; 
		} 
		else // Otherwise grab from client size
		{     		 
			GetClientRect(m_hWnd, &ClientRect);     
			m_Width = ClientRect.right;  
		}
	} 
	else 
	{ 
		m_Width = Width;
	}
	if(!Height)// Configure height 
	{	
		if(Windowed == FALSE)// Default to screen height if fullscreen 	
		{   
			m_Height = m_d3ddm.Height; 
		} 
		else// Otherwise grab from client size 	
		{    			    
			GetClientRect(m_hWnd, &ClientRect);   
			m_Height = ClientRect.bottom;  
		}
	}  
	else
	{ 
		m_Height = Height;
	}
	if(!(m_BPP = BPP) || Windowed == TRUE)// Configure BPP 
	{  
		if(!(m_BPP = GetFormatBPP(m_d3ddm.Format)))   
			return FALSE;
	}
	if(Windowed == TRUE)// Resize client window if using windowed mode  
	{ 
		GetWindowRect(m_hWnd, &WndRect);
		GetClientRect(m_hWnd, &ClientRect);   
		WndWidth  = (WndRect.right  - (ClientRect.right  - m_Width))  - WndRect.left;   
		WndHeight = (WndRect.bottom - (ClientRect.bottom - m_Height)) - WndRect.top;  
		MoveWindow(m_hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);
	}
	// Clear presentation structure
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	// Default to no hardware acceleration detected
	m_HAL = FALSE;
	// Setup Windowed or fullscreen usage
	if((m_Windowed = Windowed) == TRUE)
	{  
		d3dpp.Windowed         = TRUE;  
		d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;//后备缓冲区翻转的动作 
		d3dpp.BackBufferFormat = m_d3ddm.Format;//窗口模式无法选择颜色模式，只能用桌面的颜色模式
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		if(CheckFormat(m_d3ddm.Format, TRUE, TRUE) == TRUE)// See if card supports HAL	  
			m_HAL = TRUE;  	
		else
		{    			
			if(CheckFormat(m_d3ddm.Format, TRUE, FALSE) == FALSE)// Return error if not emulated      
				return FALSE;  
		}
	}  
	else//全屏模式下 
	{
		d3dpp.Windowed   = FALSE;  
		d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;//与窗口模式有所不同  
		d3dpp.BackBufferWidth  = m_Width;  
		d3dpp.BackBufferHeight = m_Height;
		d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;//全屏模式可以设置屏幕刷新频率，此处使用默认值
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;//显示后备缓冲区的速度 
		// or D3DPRESENT_INTERVAL_DEFAULT or D3DPRESENT_INTERVAL_IMMEDIATE	
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		if(m_BPP == 32)// Figure display format to use	
		{      
			Format    = D3DFMT_X8R8G8B8;      
			AltFormat = D3DFMT_X8R8G8B8;
		}  
		if(m_BPP == 24) 	
		{      
			Format    = D3DFMT_R8G8B8;       
			AltFormat = D3DFMT_R8G8B8; 
		}
		if(m_BPP == 16)	
		{     
			Format    = D3DFMT_R5G6B5;    
			AltFormat = D3DFMT_X1R5G5B5;  
		}
		if(m_BPP == 8) 	
		{      
			Format    = D3DFMT_P8;     
			AltFormat = D3DFMT_P8;
		}	
		if(CheckFormat(Format, FALSE, TRUE) == TRUE)// Check for HAL device       
			m_HAL = TRUE;
		else
		{     			     
			if(CheckFormat(AltFormat, FALSE, TRUE) == TRUE)// Check for HAL device in alternate format	   
			{         
				m_HAL = TRUE;          
				Format = AltFormat;      
			} 	   
			else//软件模拟   
			{         
				// Check for Emulation device          
				if(CheckFormat(Format, FALSE, FALSE) == FALSE) 		   
				{ 			   
					// Check for Emulation device in alternate format               
					if(CheckFormat(AltFormat, FALSE, FALSE) == FALSE)                  
						return FALSE;              
					else                  
						Format = AltFormat;        		   
				}    		
			}  	
		}   
		d3dpp.BackBufferFormat = Format;
	}
	// Setup Zbuffer format - 16 bit 
	if((m_ZBuffer = UseZBuffer) == TRUE)
 
	{
        d3dpp.EnableAutoDepthStencil = TRUE;
        d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    } 
    else 
    {
        d3dpp.EnableAutoDepthStencil = FALSE;
    }
    // Create the Direct3D Device object
    if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		(m_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,hWnd, 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp, &m_pD3DDevice)))                                            
    {  
		// Try to create Direct3D without ZBuffer support if selected and first call failed. 
		if(m_ZBuffer == TRUE)	
		{    
			m_ZBuffer = FALSE;   
			d3dpp.EnableAutoDepthStencil = FALSE;    
			if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				(m_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,hWnd, 
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp, &m_pD3DDevice)))//顶点处理采用纯软件?                                                            
				return FALSE;
		}	
		else                                      
			return FALSE;
	}
    // Set default rendering states
    EnableLighting(FALSE);
    EnableZBuffer(m_ZBuffer);
    EnableAlphaBlending(FALSE);
    EnableAlphaTesting(FALSE);
    // Enable texture rendering stages and filter types
    m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    // Set default ambient color to white
    SetAmbientLight((char)255,(char)255,(char)255);
    // Calculate the aspect ratio based on window size
    Aspect = (float)m_Height / (float)m_Width;
    SetPerspective(D3DX_PI/4, Aspect, 1.0f, 10000.0f);
    // Create a sprite interface
    if(FAILED(D3DXCreateSprite(m_pD3DDevice, &m_pSprite)))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::Shutdown()
{
    ReleaseCOM(m_pSprite);
    ReleaseCOM(m_pD3DDevice);
    ReleaseCOM(m_pD3D);
    return TRUE;
}

IDirect3D9 *CGraphics::GetDirect3DCOM()
{
    return m_pD3D;
}

IDirect3DDevice9 *CGraphics::GetDeviceCOM()
{
    return m_pD3DDevice;
}

ID3DXSprite *CGraphics::GetSpriteCOM()
{
    return m_pSprite;
}

long CGraphics::GetNumDisplayModes(D3DFORMAT Format)
{
    if(m_pD3D == NULL)
        return 0;
    return (long)m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, Format);
}

BOOL CGraphics::GetDisplayModeInfo(long Num, D3DDISPLAYMODE *Mode, D3DFORMAT Format)
{
    long Max;
    if(m_pD3D == NULL)
        return FALSE;
    Max = GetNumDisplayModes(Format);
    if(Num >= Max)
        return FALSE;
    if(FAILED(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT,Format, Num, Mode)))            
        return FALSE;
    return TRUE;
}

char CGraphics::GetFormatBPP(D3DFORMAT Format)
{
  
	switch(Format) 
	{		
	case D3DFMT_A8R8G8B8:// 32 bit modes
    case D3DFMT_X8R8G8B8:     
		return 32;     
		break;   
    case D3DFMT_R8G8B8:// 24 bit modes
        return 24;
        break;  
    case D3DFMT_R5G6B5:// 16 bit modes
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
        return 16;
        break;   
    case D3DFMT_A8P8:// 8 bit modes
    case D3DFMT_P8:
        return 8;
        break;
    default:
        return 0; 
	}
}

BOOL CGraphics::CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL)
{  
	if(FAILED(m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,	 
		(HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,Format, Format, Windowed))) 
		return FALSE;	  		                           		
	return TRUE;
}

BOOL CGraphics::BeginScene()
{
 	if(m_pD3DDevice == NULL)   
		return FALSE;
	if(FAILED(m_pD3DDevice->BeginScene()))  
		return FALSE; 
	return TRUE;
}

BOOL CGraphics::EndScene()
{
	short i;
	if(m_pD3DDevice == NULL)// Error checking 
		return FALSE;
	for(i=0;i<8;i++)// Release all textures   
		m_pD3DDevice->SetTexture(i, NULL);
	if(FAILED(m_pD3DDevice->EndScene()))// End the scene 
		return FALSE;
	return TRUE;
}

BOOL CGraphics::BeginSprite()
{
    if(m_pSprite == NULL)
        return FALSE;
    if(FAILED(m_pSprite->Begin(D3DXSPRITE_ALPHABLEND)))//D3DXSPRITE_DONOTSAVESTATE
        return FALSE;
    return TRUE;
}

BOOL CGraphics::EndSprite()
{
    if(m_pSprite == NULL)
        return FALSE;
    if(FAILED(m_pSprite->End()))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::Display()
{
    if(m_pD3DDevice == NULL)
        return FALSE;
    if(FAILED(m_pD3DDevice->Present(NULL, NULL, NULL, NULL)))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::Clear(long Color, float ZBuffer)
{
    if(m_pD3DDevice == NULL)
        return FALSE;  
    if(m_ZBuffer == FALSE)// Only clear screen if no zbuffer
        return ClearDisplay(Color);
    // Clear display and zbuffer
    if(FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Color, ZBuffer, 0)))
        return FALSE;
    return TRUE; 
}

BOOL CGraphics::ClearDisplay(long Color)
{
    if(m_pD3DDevice == NULL)
        return FALSE;
    if(FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, Color, 1.0f, 0)))
        return FALSE;
    return TRUE; 
}  

BOOL CGraphics::ClearZBuffer(float ZBuffer)
{
    if(m_pD3DDevice == NULL || m_ZBuffer == FALSE)
        return FALSE;
    if(FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, ZBuffer, 0)))
        return FALSE;
    return TRUE;
}

long CGraphics::GetWidth()
{
    return m_Width;
}

long CGraphics::GetHeight()
{
    return m_Height;
}

char CGraphics::GetBPP()
{
    return m_BPP;
}

BOOL CGraphics::GetHAL()
{
    return m_HAL;
}
    
BOOL CGraphics::GetZBuffer()
{
    return m_ZBuffer;
}

//BOOL CGraphics::SetCamera(CCamera *Camera)
//{
//    if(m_pD3DDevice == NULL || Camera == NULL)
//        return FALSE;
//    if(FAILED(m_pD3DDevice->SetTransform(D3DTS_VIEW, &Camera->GetMatrix())))
//        return FALSE;
//    return TRUE;
//}

BOOL CGraphics::SetWorldPosition(CWorldPosition *WorldPos)
{
    if(WorldPos == NULL || m_pD3DDevice == NULL)
        return FALSE;
    if(FAILED(m_pD3DDevice->SetTransform(D3DTS_WORLD, WorldPos->GetMatrix(this))))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::SetLight(long Num, CLight *Light)
{
    if(Light == NULL)
        return FALSE;
    if(m_pD3DDevice == NULL)
        return FALSE;
    if(FAILED(m_pD3DDevice->SetLight(Num, Light->GetLight())))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::SetAmbientLight(char Red, char Green, char Blue)
{
    D3DCOLOR Color;
    if(m_pD3DDevice == NULL)
        return FALSE;
    Color = D3DCOLOR_XRGB((m_AmbientRed = Red), (m_AmbientGreen = Green), (m_AmbientBlue = Blue));
    if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, Color)))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::GetAmbientLight(char *Red, char *Green, char *Blue)
{
    if(Red != NULL)
        *Red = m_AmbientRed;
    if(Green != NULL)
        *Green = m_AmbientGreen;
    if(Blue != NULL)
        *Blue = m_AmbientBlue;
    return TRUE;
}

BOOL CGraphics::SetMaterial(CMaterial *Material)
{
    if(m_pD3DDevice == NULL)
        return FALSE;
    if(Material != NULL) 
	{   
		if(FAILED(m_pD3DDevice->SetMaterial(Material->GetMaterial())))    
			return FALSE;
	}
	return TRUE;
}

BOOL CGraphics::SetTexture(short Num, CTexture *Texture)
{  
    if(m_pD3DDevice == NULL || Num < 0 || Num > 7)// Error checking
        return FALSE;
    if(Texture == NULL) 
	{ 
		if(FAILED(m_pD3DDevice->SetTexture(Num, NULL)))// Clear the texture     
			return NULL; 
	} 
	else
	{ 		  
		if(FAILED(m_pD3DDevice->SetTexture(Num, Texture->GetTextureCOM())))// Set the texture   
			return FALSE;
	}
	return TRUE;
}

BOOL CGraphics::SetPerspective(float FOV, float Aspect, float Near, float Far)
{
    D3DXMATRIX matProjection;
    if(m_pD3DDevice == NULL)
        return FALSE;
    D3DXMatrixPerspectiveFovLH(&matProjection, FOV, Aspect, Near, Far);
    if(FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection)))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::EnableLight(long Num, BOOL Enable)
{
    if(m_pD3DDevice == NULL)
        return FALSE;
    if(FAILED(m_pD3DDevice->LightEnable(Num, Enable)))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::EnableLighting(BOOL Enable)
{
    if(m_pD3DDevice == NULL)
        return FALSE;
    if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, Enable)))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::EnableZBuffer(BOOL Enable)
{
    if(m_pD3DDevice == NULL || m_ZBuffer == FALSE)
        return FALSE;
    if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, (Enable == TRUE) ? D3DZB_TRUE : D3DZB_FALSE)))
        return FALSE;
    return TRUE;
}

BOOL CGraphics::EnableAlphaBlending(BOOL Enable, DWORD Src, DWORD Dest)
{
    if(m_pD3DDevice == NULL)
        return FALSE;
    // Enable or disable
    if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, Enable)))
        return FALSE;
    // Set blend type
    if(Enable == TRUE) 
	{    
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  Src);  
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, Dest);
	}
	return TRUE;
}

BOOL CGraphics::EnableAlphaTesting(BOOL Enable)
{
    if(m_pD3DDevice == NULL)
        return FALSE;
    if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, Enable)))
        return FALSE;  
    if(Enable == TRUE)// Set test type
	{ 
		m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x08); 
		m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}
	return TRUE;
}

void CGraphics::SetLinearVertexFog(float Start,float End,DWORD color,bool UseRange)
{
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE,true);
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR,color);
	m_pD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_LINEAR);
	m_pD3DDevice->SetRenderState(D3DRS_FOGSTART,*(DWORD*)(&Start));
	m_pD3DDevice->SetRenderState(D3DRS_FOGEND,*(DWORD*)(&End));
	if(UseRange)
		m_pD3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE,true);
}

void CGraphics::SetExpVertexFog(DWORD color,bool UseRange,float Density)
{
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE,true);
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR,color);
	m_pD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_EXP);
	m_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY,*(DWORD*)(&Density));//雾化密度
    if(UseRange)
		m_pD3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE,true);
}

void CGraphics::SetExp2VertexFog(DWORD color,bool UseRange,float Density)
{
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE,true);
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR,color);
	m_pD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_EXP2);
	m_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY,*(DWORD*)(&Density));//雾化密度
    if(UseRange)
		m_pD3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE,true);
}

void CGraphics::SetLinearPixelFog(float Start,float End,DWORD color)
{
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE,true);
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR,color);
	m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_LINEAR);
	m_pD3DDevice->SetRenderState(D3DRS_FOGSTART,*(DWORD*)(&Start));
	m_pD3DDevice->SetRenderState(D3DRS_FOGEND,*(DWORD*)(&End));
}

void CGraphics::SetExp2PixelFog(DWORD color,float Density)
{
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE,true);
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR,color);
	m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_EXP2);
	m_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY,*(DWORD*)(&Density));
}

void CGraphics::SetExpPixelFog(DWORD color,float Density)
{
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE,true);
	m_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR,color);
	m_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE,D3DFOG_EXP);
	m_pD3DDevice->SetRenderState(D3DRS_FOGDENSITY,*(DWORD*)(&Density));
}

CWorldPosition::CWorldPosition()
{
    m_Billboard = FALSE;
    m_matCombine1 = m_matCombine2 = NULL;
    Move(0.0f,0.0f,0.0f);
    Rotate(0.0f,0.0f,0.0f);
    Scale(1.0f, 1.0f, 1.0f);
	D3DXMatrixIdentity(&m_matWorld);
    Update();
}

BOOL CWorldPosition::Copy(CWorldPosition *DestPos)
{
    DestPos->Move(m_XPos, m_YPos, m_ZPos);
    DestPos->Rotate(m_XRotation, m_YRotation, m_ZRotation);
    DestPos->Scale(m_XScale, m_YScale, m_ZScale);
    DestPos->EnableBillboard(m_Billboard); 
    return TRUE;
}

BOOL CWorldPosition::Move(float XPos, float YPos, float ZPos)
{
    m_XPos = XPos;
    m_YPos = YPos;
    m_ZPos = ZPos;
    D3DXMatrixTranslation(&m_matTranslation, m_XPos, m_YPos, m_ZPos);
    return TRUE;
}

BOOL CWorldPosition::MoveRel(float XAdd, float YAdd, float ZAdd)
{
    return Move(m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd);
}

BOOL CWorldPosition::Rotate(float XRot, float YRot, float ZRot)
{
    m_XRotation = XRot;
    m_YRotation = YRot;
    m_ZRotation = ZRot;
    D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_YRotation, m_XRotation, m_ZRotation);
    return TRUE;
}

BOOL CWorldPosition::RotateRel(float XAdd, float YAdd, float ZAdd)
{
    return Rotate(m_XRotation + XAdd, m_YRotation + YAdd, m_ZRotation + ZAdd);
}

BOOL CWorldPosition::Scale(float XScale, float YScale, float ZScale)
{
    m_XScale = XScale;
    m_YScale = YScale;
    m_ZScale = ZScale;
    D3DXMatrixScaling(&m_matScale, XScale, YScale, ZScale);
    return TRUE;
}

BOOL CWorldPosition::ScaleRel(float XAdd, float YAdd, float ZAdd)
{
    return Scale(m_XScale + XAdd, m_YScale + YAdd, m_ZScale + ZAdd);
}

BOOL CWorldPosition::Update(CGraphics *Graphics)
{
    D3DXMATRIX matView, matTransposed;  
    if(m_Billboard == TRUE)// Setup billboarding matrix
	{  
		if(Graphics != NULL && Graphics->GetDeviceCOM() != NULL) 
		{     
			Graphics->GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView);     
			D3DXMatrixTranspose(&matTransposed, &matView);     
			matTransposed._41 = matTransposed._42 = matTransposed._43 = 
				matTransposed._14 = matTransposed._24 = matTransposed._34 = 0.0f; 
		}
		else	    
			D3DXMatrixIdentity(&matTransposed);		
	} 
	// Combine scaling and rotation matrices first
	D3DXMatrixMultiply(&m_matTemp, &m_matScale, &m_matRotation);//
	if(m_Billboard == TRUE)// Apply billboard matrix  
		D3DXMatrixMultiply(&m_matTemp, &m_matTemp, &matTransposed);//  
	// Combine with translation matrix
	D3DXMatrixMultiply(&m_matTemp, &m_matTemp, &m_matTranslation);
	// Combine with combined matrices (if any)
	if(m_matCombine1 != NULL)  
		D3DXMatrixMultiply(&m_matTemp, &m_matTemp, m_matCombine1);
	if(m_matCombine2 != NULL)   
		D3DXMatrixMultiply(&m_matTemp, &m_matTemp, m_matCombine2);
	
	/*D3DXMatrixMultiply(&m_matWorld, &m_matScale, &m_matRotation);//
	if(m_Billboard == TRUE)// Apply billboard matrix  
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &matTransposed);//  
	// Combine with translation matrix
	D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matTranslation);
	if(m_matCombine1 != NULL)  
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine1);
	if(m_matCombine2 != NULL)   
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine2);*/
	D3DXMatrixMultiply(&m_matWorld, &m_matWorld,&m_matTemp);
	//m_matWorld=m_matTemp;

	return TRUE;
}

BOOL CWorldPosition::EnableBillboard(BOOL Enable)
{
    m_Billboard = Enable;
    return TRUE;
}

D3DXMATRIX *CWorldPosition::GetMatrix(CGraphics *Graphics)
{
    Update(Graphics);
    return &m_matWorld;
}

BOOL CWorldPosition::SetCombineMatrix1(D3DXMATRIX *Matrix)
{
    m_matCombine1 = Matrix;
    return TRUE;
}

BOOL CWorldPosition::SetCombineMatrix2(D3DXMATRIX *Matrix)
{
    m_matCombine2 = Matrix;
    return TRUE;
}

float CWorldPosition::GetXPos()
{
    return m_XPos;
}

float CWorldPosition::GetYPos()
{
    return m_YPos;
}

float CWorldPosition::GetZPos()
{
    return m_ZPos;
}

float CWorldPosition::GetXRotation()
{
    return m_XRotation;
}

float CWorldPosition::GetYRotation()
{
    return m_YRotation;
}

float CWorldPosition::GetZRotation()
{
    return m_ZRotation;
}

float CWorldPosition::GetXScale()
{
    return m_XScale;
}

float CWorldPosition::GetYScale()
{
    return m_YScale;
}

float CWorldPosition::GetZScale()
{
    return m_ZScale;
}

//CCamera::CCamera()
//{
//    Move(0.0f,0.0f,0.0f);
//    Rotate(0.0f,0.0f,0.0f);
//    Update();
//}
//
//BOOL CCamera::Move(float XPos, float YPos, float ZPos)
//{
//    m_XPos = XPos;
//    m_YPos = YPos;
//    m_ZPos = ZPos;
//    D3DXMatrixTranslation(&m_matTranslation, -m_XPos, -m_YPos, -m_ZPos);
//    return TRUE;
//}
//
//BOOL CCamera::MoveRel(float XAdd, float YAdd, float ZAdd)
//{
//    return Move(m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd);
//}
//
//BOOL CCamera::Rotate(float XRot, float YRot, float ZRot)
//{
//    D3DXMATRIX matXRotation, matYRotation, matZRotation;
//    m_XRot = XRot;
//    m_YRot = YRot;
//    m_ZRot = ZRot;
//    D3DXMatrixRotationX(&matXRotation, -m_XRot);
//    D3DXMatrixRotationY(&matYRotation, -m_YRot);
//    D3DXMatrixRotationZ(&matZRotation, -m_ZRot);
//    m_matRotation = matZRotation;
//    D3DXMatrixMultiply(&m_matRotation, &m_matRotation, &matYRotation);
//    D3DXMatrixMultiply(&m_matRotation, &m_matRotation, &matXRotation);
//    return TRUE;
//}
//
//BOOL CCamera::RotateRel(float XAdd, float YAdd, float ZAdd)
//{
//    return Rotate(m_XRot + XAdd, m_YRot + YAdd, m_ZRot + ZAdd);
//}
//
//BOOL CCamera::Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt)
//{
//    float XRot, YRot, XDiff, YDiff, ZDiff;
//    // Calculate angles between points
//    XDiff = XAt - XEye;
//    YDiff = YAt - YEye;
//    ZDiff = ZAt - ZEye;
//    XRot = (float)atan2(-YDiff, sqrt(XDiff*XDiff+ZDiff*ZDiff));/*---------*/
//    YRot = (float)atan2(XDiff, ZDiff);
//    Move(XEye, YEye, ZEye);
//    Rotate(XRot, YRot, 0.0f);
//    return TRUE;
//}
//
//BOOL CCamera::SetStartTrack()
//{
//    m_StartXPos = m_XPos;
//    m_StartYPos = m_YPos;
//    m_StartZPos = m_ZPos;
//    m_StartXRot = m_XRot;
//    m_StartYRot = m_YRot;
//    m_StartZRot = m_ZRot;
//    return TRUE;
//}
//
//BOOL CCamera::SetEndTrack()
//{
//    m_EndXPos = m_XPos;
//    m_EndYPos = m_YPos;
//    m_EndZPos = m_ZPos;
//    m_EndXRot = m_XRot;
//    m_EndYRot = m_YRot;
//    m_EndZRot = m_ZRot;
//    return TRUE;
//}
//
//BOOL CCamera::Track(float Time, float Length)
//{
//    float x, y, z;
//    float TimeOffset;
//
//    TimeOffset = Length * Time;
//
//    x = (m_EndXPos - m_StartXPos) / Length * TimeOffset;
//    y = (m_EndYPos - m_StartYPos) / Length * TimeOffset;
//    z = (m_EndZPos - m_StartZPos) / Length * TimeOffset;
//    Move(m_StartXPos + x, m_StartYPos + y, m_StartZPos + z);
//
//    x = (m_EndXRot - m_StartXRot) / Length * TimeOffset;
//    y = (m_EndYRot - m_StartYRot) / Length * TimeOffset;
//    z = (m_EndZRot - m_StartZRot) / Length * TimeOffset;
//    Rotate(m_StartXRot + x, m_StartYRot + y, m_StartZRot + z);
//
//    return TRUE;
//}
//
//BOOL CCamera::Update()
//{
//    D3DXMatrixMultiply(&m_matWorld, &m_matTranslation, &m_matRotation);
//    return TRUE;
//}
//
///*D3DXMATRIX *CCamera::GetMatrix()
//{
//    Update();
//    return &m_matWorld;
//}*/
//
//D3DXMATRIX &CCamera::GetMatrix()
//{
//	Update();
//	return m_matWorld;
//}
//
//float CCamera::GetXPos()
//{
//    return m_XPos;
//}
//
//float CCamera::GetYPos()
//{
//    return m_YPos;
//}
//
//float CCamera::GetZPos()
//{
//    return m_ZPos;
//}
//
//float CCamera::GetXRotation()
//{
//    return m_XRot;
//}
//
//float CCamera::GetYRotation()
//{
//    return m_YRot;
//}
//
//float CCamera::GetZRotation()
//{
//    return m_ZRot;
//}

CGameFont::CGameFont()
{
    m_Font = NULL;
}

CGameFont::~CGameFont()
{
    Free();
}

ID3DXFont *CGameFont::GetFontCOM()
{
    return m_Font;
}

BOOL CGameFont::Create(CGraphics *Graphics, const WCHAR* Name, long Size, BOOL Bold, BOOL Italic, BOOL Underline, BOOL Strikeout)
{
	if(Graphics == NULL || Name == NULL)
        return FALSE;
    if(Graphics->GetDeviceCOM() == NULL)
        return FALSE;
	D3DXFONT_DESC d3dFont;
	memset(&d3dFont,0,sizeof(d3dFont));
	d3dFont.Height=-Size;
	d3dFont.Weight=(Bold==TRUE) ? 600 : 0;
	d3dFont.Italic=Italic;
	d3dFont.CharSet=DEFAULT_CHARSET;
	wcscpy_s(d3dFont.FaceName, 32, Name);
	if(FAILED(D3DXCreateFontIndirect(Graphics->GetDeviceCOM(),&d3dFont,&m_Font)))
		return FALSE;
    return TRUE;
}

BOOL CGameFont::Free()
{
    ReleaseCOM(m_Font);
    return TRUE;
}

BOOL CGameFont::Print(const WCHAR* Text, long XPos, long YPos, long Width, long Height, D3DCOLOR Color, DWORD Format)
{
    RECT Rect;
    if(m_Font == NULL)
		return FALSE;
    if(!Width)
        Width = 65535;
    if(!Height)
        Height = 65536;
    Rect.left   = XPos;
    Rect.top    = YPos;
    Rect.right  = Rect.left + Width;
    Rect.bottom = Rect.top + Height;
    if(FAILED(m_Font->DrawText(NULL,Text, -1, &Rect, Format, Color)))
        return FALSE;
    return TRUE;
}

CTexture::CTexture()
{
    m_Graphics = NULL;
    m_Texture = NULL;
    m_Width = m_Height = 0;
}

CTexture::~CTexture()
{
    Free();
}

BOOL CTexture::Load(CGraphics* Graphics, const WCHAR* Filename, DWORD Transparent, D3DFORMAT Format)
{
    Free();
    if((m_Graphics = Graphics) == NULL)
        return FALSE;
    if(Graphics->GetDeviceCOM() == NULL)     
		return FALSE;
	if (Filename == NULL)
		return FALSE;

	HRESULT hr;
	if (FAILED(hr = D3DXCreateTextureFromFileEx(Graphics->GetDeviceCOM(), Filename, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_FROM_FILE,
		0, Format, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, Transparent, NULL, NULL, &m_Texture)))
		return FALSE;
    m_Width = GetWidth();
    m_Height = GetHeight();
    return TRUE;
}

BOOL CTexture::Create(CGraphics *Graphics, DWORD Width, DWORD Height, D3DFORMAT Format)
{
    Free();
    if((m_Graphics = Graphics) == NULL)
        return FALSE;
    if(FAILED(m_Graphics->GetDeviceCOM()->CreateTexture(Width, Height, 0, 0, Format, D3DPOOL_MANAGED, &m_Texture, NULL)))
        return FALSE;
    return TRUE;
}

BOOL CTexture::Create(CGraphics *Graphics, IDirect3DTexture9 *Texture)
{
    D3DLOCKED_RECT SrcRect, DestRect;
    D3DSURFACE_DESC Desc;
    Free();
    if((m_Graphics = Graphics) == NULL)
        return FALSE;
    if(Texture == NULL)
        return TRUE;  
    // Copy texture over
    Texture->GetLevelDesc(0, &Desc);
    m_Width  = Desc.Width;
    m_Height = Desc.Height;
    m_Graphics->GetDeviceCOM()->CreateTexture(m_Width, m_Height, 0, 0, Desc.Format, D3DPOOL_MANAGED, &m_Texture, NULL);
    Texture->LockRect(0, &SrcRect, NULL, D3DLOCK_READONLY);
    m_Texture->LockRect(0, &DestRect, NULL, 0);
    memcpy(DestRect.pBits, SrcRect.pBits, SrcRect.Pitch * m_Height);
    m_Texture->UnlockRect(0);
    Texture->UnlockRect(0);
    return TRUE;
}

BOOL CTexture::Free()
{
    ReleaseCOM(m_Texture);
    m_Graphics = NULL;
    m_Width = m_Height = 0;
    return TRUE;
}

BOOL CTexture::IsLoaded()
{
    if(m_Texture == NULL)
        return FALSE;
    return TRUE;
}

IDirect3DTexture9 *CTexture::GetTextureCOM()
{
    return m_Texture;
}

long CTexture::GetWidth()
{
    D3DSURFACE_DESC d3dsd;
    if(m_Texture == NULL)
        return 0;
    if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
        return 0;
    return d3dsd.Width;
}

long CTexture::GetHeight()
{
    D3DSURFACE_DESC d3dsd;
    if(m_Texture == NULL)
        return 0;
    if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
        return 0;
    return d3dsd.Height;
}

D3DFORMAT CTexture::GetFormat()
{
    D3DSURFACE_DESC d3dsd;
    if(m_Texture == NULL)
        return D3DFMT_UNKNOWN;
    if(FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
        return D3DFMT_UNKNOWN;
    return d3dsd.Format;
}

BOOL CTexture::Blit(long DestX, long DestY,long SrcX, long SrcY,
					long Width, long Height,float XScale, float YScale,D3DCOLOR Color)                                                                                                                                                    
{
    RECT Rect;
    ID3DXSprite *pSprite;
    D3DXMATRIX matScale,matOld;	//定义一个缩放矩阵
    if(m_Texture == NULL)
        return FALSE;
    if(m_Graphics == NULL)
        return FALSE;
    if((pSprite = m_Graphics->GetSpriteCOM()) == NULL)
        return FALSE;
    if(!Width)
        Width = m_Width;
    if(!Height)
        Height = m_Height;
    Rect.left = SrcX;
    Rect.top  = SrcY;
    Rect.right = Rect.left + Width;
    Rect.bottom = Rect.top + Height;
	pSprite->GetTransform(&matOld);
	D3DXMatrixScaling(&matScale,XScale,YScale,0.0f);
	pSprite->SetTransform(&matScale); 
	if(FAILED(pSprite->Draw(m_Texture,&Rect,NULL,&D3DXVECTOR3(float(DestX),float(DestY),0.0f),Color))) 
		return FALSE;
	return TRUE;
}

CMaterial::CMaterial()
{
    // Set a default material (white)
    ZeroMemory(&m_Material, sizeof(D3DMATERIAL9));
    SetDiffuseColor((unsigned char)255,(unsigned char)255,(unsigned char)255);
    SetAmbientColor((unsigned char)255,(unsigned char)255,(unsigned char)255);
    SetSpecularColor((unsigned char)255,(unsigned char)255,(unsigned char)255);
    SetEmissiveColor(0,0,0);
    SetPower(1.0f);
}

BOOL CMaterial::SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
    m_Material.Diffuse.r = 1.0f / 255.0f * (float)Red;
    m_Material.Diffuse.g = 1.0f / 255.0f * (float)Green;
    m_Material.Diffuse.b = 1.0f / 255.0f * (float)Blue;
	m_Material.Diffuse.a =1.0f;
    return TRUE;
}

BOOL CMaterial::GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
    if(Red != NULL)
        *Red = (unsigned char)(255.0f * m_Material.Diffuse.r);
    if(Green != NULL)
        *Green = (unsigned char)(255.0f * m_Material.Diffuse.g);
    if(Blue != NULL)
        *Blue = (unsigned char)(255.0f * m_Material.Diffuse.b);
    return TRUE;
}

BOOL CMaterial::SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
    m_Material.Ambient.r = 1.0f / 255.0f * (float)Red;
    m_Material.Ambient.g = 1.0f / 255.0f * (float)Green;
    m_Material.Ambient.b = 1.0f / 255.0f * (float)Blue;
	m_Material.Ambient.a =1.0f;
    return TRUE;
}

BOOL CMaterial::GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
    if(Red != NULL)    
		*Red = (unsigned char)(255.0f * m_Material.Ambient.r);
    if(Green != NULL)
        *Green = (unsigned char)(255.0f * m_Material.Ambient.g);
    if(Blue != NULL)
        *Blue = (unsigned char)(255.0f * m_Material.Ambient.b);
    return TRUE;
}

BOOL CMaterial::SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
    m_Material.Specular.r = 1.0f / 255.0f * (float)Red;
    m_Material.Specular.g = 1.0f / 255.0f * (float)Green;
    m_Material.Specular.b = 1.0f / 255.0f * (float)Blue;
	m_Material.Specular.a =1.0f;
    return TRUE;
}

BOOL CMaterial::GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
    if(Red != NULL)
        *Red = (unsigned char)(255.0f * m_Material.Specular.r);
    if(Green != NULL)
        *Green = (unsigned char)(255.0f * m_Material.Specular.g);
    if(Blue != NULL)
        *Blue = (unsigned char)(255.0f * m_Material.Specular.b);
    return TRUE;
}

BOOL CMaterial::SetEmissiveColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
    m_Material.Emissive.r = 1.0f / 255.0f * (float)Red;
    m_Material.Emissive.g = 1.0f / 255.0f * (float)Green;
    m_Material.Emissive.b = 1.0f / 255.0f * (float)Blue;
	m_Material.Emissive.a =1.0f;
    return TRUE;
}

BOOL CMaterial::GetEmissiveColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
    if(Red != NULL)
        *Red = (unsigned char)(255.0f * m_Material.Emissive.r);
    if(Green != NULL)
        *Green = (unsigned char)(255.0f * m_Material.Emissive.g);
    if(Blue != NULL)
        *Blue = (unsigned char)(255.0f * m_Material.Emissive.b);
    return TRUE;
}

BOOL CMaterial::SetPower(float Power)
{
    m_Material.Power = Power;
    return TRUE;
}

float CMaterial::GetPower(float Power)
{
    return m_Material.Power;
}

D3DMATERIAL9 *CMaterial::GetMaterial()
{
    return &m_Material;
}

CLight::CLight()
{
    // Set a default light to point
    ZeroMemory(&m_Light, sizeof(D3DLIGHT9));
    //SetType(D3DLIGHT_POINT);
    //Move(0.0f, 0.0f, 0.0f);
    //SetDiffuseColor((unsigned char)255,(unsigned char)255,(unsigned char)255);
    //SetAmbientColor((unsigned char)255,(unsigned char)255,(unsigned char)255);
    //SetRange(1000.0f);
    //SetAttenuation0(1.0f);
}

BOOL CLight::SetType(D3DLIGHTTYPE Type)
{
    m_Light.Type = Type;
    return TRUE;
}

BOOL CLight::Move(float XPos, float YPos, float ZPos)
{
    m_Light.Position.x = XPos;
    m_Light.Position.y = YPos;
    m_Light.Position.z = ZPos;
    return TRUE;
}

BOOL CLight::MoveRel(float XPos, float YPos, float ZPos)
{
    m_Light.Position.x += XPos;
    m_Light.Position.y += YPos;
    m_Light.Position.z += ZPos;
    return TRUE;
}

BOOL CLight::GetPos(float *XPos, float *YPos, float *ZPos)
{
    if(XPos != NULL)
        *XPos = m_Light.Position.x;
    if(YPos != NULL)
        *YPos = m_Light.Position.y;
    if(ZPos != NULL)
        *ZPos = m_Light.Position.z;
    return TRUE;
}

BOOL CLight::Point(float XFrom, float YFrom, float ZFrom, float XAt, float YAt, float ZAt)                  
{
    D3DXVECTOR3 vecSrc;
    D3DXVECTOR3 vecDest;
    // Move the light
    Move(XFrom, YFrom, ZFrom);
    // Calculate vector between angles
    m_Light.Direction.x = XAt - XFrom;
    m_Light.Direction.y = YAt - YFrom;
    m_Light.Direction.z = ZAt - ZFrom;
    return TRUE;
}

BOOL CLight::GetDirection(float *XDir, float *YDir, float *ZDir)
{
    if(XDir != NULL)
        *XDir = m_Light.Direction.x;
    if(YDir != NULL)
        *YDir = m_Light.Direction.y;
    if(ZDir != NULL)
        *ZDir = m_Light.Direction.z;
    return TRUE;
}

BOOL CLight::SetDirection(float XDir,float YDir,float ZDir)
{
	m_Light.Direction.x=XDir;
	m_Light.Direction.y=YDir;
	m_Light.Direction.z=ZDir;
	return TRUE;
}

BOOL CLight::SetDiffuseColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
    m_Light.Diffuse.r = 1.0f / 255.0f * (float)Red;
    m_Light.Diffuse.g = 1.0f / 255.0f * (float)Green;
    m_Light.Diffuse.b = 1.0f / 255.0f * (float)Blue;
	m_Light.Diffuse.a = 1.0f;
    return TRUE;
}

BOOL CLight::GetDiffuseColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
    if(Red != NULL)
        *Red = (unsigned char)(255.0f * m_Light.Diffuse.r);
    if(Green != NULL)
        *Green = (unsigned char)(255.0f * m_Light.Diffuse.g);
    if(Blue != NULL)
        *Blue = (unsigned char)(255.0f * m_Light.Diffuse.b);
    return TRUE;
}

BOOL CLight::SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
    m_Light.Specular.r = 1.0f / 255.0f * (float)Red;
    m_Light.Specular.g = 1.0f / 255.0f * (float)Green;
    m_Light.Specular.b = 1.0f / 255.0f * (float)Blue;
	m_Light.Specular.a = 1.0f;
    return TRUE;
}

BOOL CLight::GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
    if(Red != NULL)
        *Red = (unsigned char)(255.0f * m_Light.Specular.r);
    if(Green != NULL)
        *Green = (unsigned char)(255.0f * m_Light.Specular.g);
    if(Blue != NULL)
        *Blue = (unsigned char)(255.0f * m_Light.Specular.b);
    return TRUE;
}

BOOL CLight::SetAmbientColor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
    m_Light.Ambient.r = 1.0f / 255.0f * (float)Red;
    m_Light.Ambient.g = 1.0f / 255.0f * (float)Green;
    m_Light.Ambient.b = 1.0f / 255.0f * (float)Blue;
	m_Light.Ambient.a = 1.0f;
    return TRUE;
}

BOOL CLight::GetAmbientColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue)
{
    if(Red != NULL)
        *Red = (unsigned char)(255.0f * m_Light.Ambient.r);
    if(Green != NULL)
        *Green = (unsigned char)(255.0f * m_Light.Ambient.g);
    if(Blue != NULL)
        *Blue = (unsigned char)(255.0f * m_Light.Ambient.b);
    return TRUE;
}

BOOL CLight::SetRange(float Range)
{
    m_Light.Range = Range;
    return TRUE;
}

float CLight::GetRange()
{
    return m_Light.Range;
}

BOOL CLight::SetFalloff(float Falloff)
{
    m_Light.Falloff = Falloff;
    return TRUE;
}

float CLight::GetFalloff()
{
    return m_Light.Falloff;
}

BOOL CLight::SetAttenuation0(float Attenuation)
{
    m_Light.Attenuation0 = Attenuation;
    return TRUE;
}

float CLight::GetAttenuation0()
{
    return m_Light.Attenuation0;
}

BOOL CLight::SetAttenuation1(float Attenuation)
{
    m_Light.Attenuation1 = Attenuation;
    return TRUE;
}

float CLight::GetAttenuation1()
{
    return m_Light.Attenuation1;
}

BOOL CLight::SetAttenuation2(float Attenuation)
{
    m_Light.Attenuation2 = Attenuation;
    return TRUE;
}

float CLight::GetAttenuation2()
{
    return m_Light.Attenuation2;
}

BOOL CLight::SetTheta(float Theta)
{
    m_Light.Theta = Theta;
    return TRUE;
}

float CLight::GetTheta()
{
    return m_Light.Theta;
}

BOOL CLight::SetPhi(float Phi)
{
    m_Light.Phi = Phi;
    return TRUE;
}

float CLight::GetPhi()
{
    return m_Light.Phi;
}

D3DLIGHT9 *CLight::GetLight()
{
    return &m_Light;
}

CVertexBuffer::CVertexBuffer()
{
    m_Graphics = NULL;
    m_pVB = NULL;
    m_NumVertices = 0;
    m_FVF = 0;
    m_Locked = FALSE;
    m_Ptr = NULL;
}

CVertexBuffer::~CVertexBuffer()
{
    Free();
}

IDirect3DVertexBuffer9 *CVertexBuffer::GetVertexBufferCOM()
{
    return m_pVB;
}

unsigned long CVertexBuffer::GetVertexSize()
{
    return D3DXGetFVFVertexSize(m_FVF);
}

unsigned long CVertexBuffer::GetVertexFVF()
{
    return m_FVF;
}

unsigned long CVertexBuffer::GetNumVertices()
{
    return m_NumVertices;
}

BOOL CVertexBuffer::Create(CGraphics *Graphics, unsigned long NumVertices, DWORD Descriptor, long VertexSize)
{
    Free();
    if((m_Graphics = Graphics) == NULL)
        return FALSE;
    if(m_Graphics->GetDeviceCOM() == NULL)
        return FALSE;
    if(!(m_NumVertices = NumVertices) || !(m_FVF = Descriptor) || !(m_VertexSize = VertexSize))
        return FALSE;
    if(FAILED(m_Graphics->GetDeviceCOM()->CreateVertexBuffer(m_NumVertices * m_VertexSize,
		0, m_FVF,D3DPOOL_MANAGED,&m_pVB, NULL)))               
		return FALSE;
	return TRUE;
}

BOOL CVertexBuffer::Free()
{
    Unlock();
    ReleaseCOM(m_pVB);
    m_Graphics = NULL;
    m_NumVertices = 0;
    m_FVF = 0;
    m_Locked = FALSE;
    m_Ptr = NULL;
    return TRUE;
}

BOOL CVertexBuffer::Set(unsigned long FirstVertex, unsigned long NumVertices, void *VertexList)
{
    if(m_Graphics == NULL || VertexList == NULL || m_pVB == NULL)
        return FALSE;
    if(m_Graphics->GetDeviceCOM() == NULL)
        return FALSE;
    if(Lock(FirstVertex, NumVertices) == FALSE)// Lock the vertex buffer
        return FALSE;  
    memcpy(m_Ptr, VertexList, NumVertices * m_VertexSize);// Copy vertices to vertex buffer  
    if(Unlock() == FALSE)// Unlock vertex buffer
        return FALSE;
    return TRUE;
}

BOOL CVertexBuffer::Render(unsigned long FirstVertex, unsigned long NumPrimitives, DWORD Type)
{
    if(m_Graphics->GetDeviceCOM() == NULL || m_pVB == NULL)
        return FALSE;
    m_Graphics->GetDeviceCOM()->SetStreamSource(0, m_pVB, 0, m_VertexSize);
    m_Graphics->GetDeviceCOM()->SetFVF(m_FVF);
    m_Graphics->GetDeviceCOM()->DrawPrimitive((D3DPRIMITIVETYPE)Type, FirstVertex, NumPrimitives);
    return TRUE;
}

BOOL CVertexBuffer::SendData()
{
	if(m_Graphics->GetDeviceCOM() == NULL || m_pVB == NULL)
        return FALSE;
	m_Graphics->GetDeviceCOM()->SetStreamSource(0, m_pVB, 0, m_VertexSize);
    m_Graphics->GetDeviceCOM()->SetFVF(m_FVF);
    return TRUE;
}

BOOL CVertexBuffer::Lock(unsigned long FirstVertex, unsigned long NumVertices)
{
    if(m_pVB == NULL)
        return FALSE;
	if(FAILED(m_pVB->Lock(FirstVertex * m_VertexSize,	  
		NumVertices * m_VertexSize, (void**)&m_Ptr, 0)))//           
		return FALSE; 
	m_Locked = TRUE;
	return TRUE;
}

BOOL CVertexBuffer::Unlock()
{ 
    if(m_pVB == NULL)
        return FALSE;
    if(FAILED(m_pVB->Unlock()))
        return FALSE;
    m_Locked = FALSE;
    return TRUE;
}

BOOL CVertexBuffer::IsLoaded()
{
    return (m_pVB == NULL) ? FALSE : TRUE;
}

void *CVertexBuffer::GetPtr()
{
    return (void*)m_Ptr;
}
