/*-------------------------------------------------------
GameCore Component
Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
2007-2008 SCY modified
-------------------------------------------------------*/
#pragma once

class CGraphics;
class CTexture;
class CMaterial;
class CLight;
class CGameFont;
class CVertexBuffer;
class CWorldPosition;

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

struct CUSTOMVERTEX
{
	D3DXVECTOR3  p;
	D3DXVECTOR2  t;
};

class CGraphics
{  
protected:   
	HWND              m_hWnd;
    IDirect3D9       *m_pD3D;
    IDirect3DDevice9 *m_pD3DDevice;
    ID3DXSprite      *m_pSprite;

    D3DDISPLAYMODE    m_d3ddm;

    BOOL              m_Windowed;
    BOOL              m_ZBuffer;
    BOOL              m_HAL;//HAL(Hardware Abstraction Layer) HEL(Hardware Emulation Layer)

    long              m_Width;
    long              m_Height;
    char              m_BPP;

    char              m_AmbientRed;
    char              m_AmbientGreen;
    char              m_AmbientBlue;
 
public:
    CGraphics();
    ~CGraphics();

    IDirect3D9       *GetDirect3DCOM();
    IDirect3DDevice9 *GetDeviceCOM();
    ID3DXSprite      *GetSpriteCOM();

    BOOL Init();
    BOOL Shutdown();

    BOOL SetMode(HWND hWnd, BOOL Windowed = TRUE, BOOL UseZBuffer = FALSE, long Width = 0, long Height = 0, char BPP = 0);

    long GetNumDisplayModes(D3DFORMAT Format);
    BOOL GetDisplayModeInfo(long Num, D3DDISPLAYMODE *Mode, D3DFORMAT Format);

    char GetFormatBPP(D3DFORMAT Format);
    BOOL CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL);

    BOOL Display();

    BOOL BeginScene();
    BOOL EndScene();

    BOOL BeginSprite();
    BOOL EndSprite();

    BOOL Clear(long Color = 0, float ZBuffer = 1.0f);
    BOOL ClearDisplay(long Color = 0);
    BOOL ClearZBuffer(float ZBuffer = 1.0f);

    long GetWidth();
    long GetHeight();
    char GetBPP();
    BOOL GetHAL();
    BOOL GetZBuffer();
    
    BOOL SetWorldPosition(CWorldPosition *WorldPos);
    //BOOL SetCamera(CCamera *Camera);
	BOOL SetPerspective(float FOV=D3DX_PI / 4.0f, float Aspect=1.3333f, float Near=1.0f, float Far=10000.0f);  
    BOOL SetLight(long Num, CLight *Light);
    BOOL SetMaterial(CMaterial *Material);
    BOOL SetTexture(short Num, CTexture *Texture);
    
    BOOL SetAmbientLight(char Red, char Green, char Blue);
    BOOL GetAmbientLight(char *Red, char *Green, char *Blue);

    BOOL EnableLight(long Num, BOOL Enable = TRUE);
    BOOL EnableLighting(BOOL Enable = TRUE);
    BOOL EnableZBuffer(BOOL Enable = TRUE);
    BOOL EnableAlphaBlending(BOOL Enable = TRUE, DWORD Src = D3DBLEND_SRCALPHA, DWORD Dest = D3DBLEND_INVSRCALPHA);
    BOOL EnableAlphaTesting(BOOL Enable = TRUE);

	void SetLinearVertexFog(float Start,float End,DWORD color,bool UseRange);
    void SetExpVertexFog(DWORD color,bool UseRange,float Density);
	void SetExp2VertexFog(DWORD color,bool UseRange,float Density);
	void SetLinearPixelFog(float Start,float End,DWORD color);
	void SetExpPixelFog(DWORD color,float Density);
	void SetExp2PixelFog(DWORD color,float Density);
};

class CTexture
{
protected:
	CGraphics         *m_Graphics;    // Parent CGraphics
    IDirect3DTexture9 *m_Texture;     // Texture COM
    unsigned long m_Width, m_Height;  // Dimensions of texture image.                                    
public:
    CTexture();  // Constructor
    ~CTexture(); // Destructor
    IDirect3DTexture9 *GetTextureCOM(); // Return texture COM
    // Load a texture from file
    BOOL Load(CGraphics *Graphics, const WCHAR* Filename,DWORD Transparent = 0,D3DFORMAT Format = D3DFMT_UNKNOWN);
    // Create a texture using specific dimensions and format
    BOOL Create(CGraphics *Graphics,DWORD Width, DWORD Height, D3DFORMAT Format);
    // Configure a CTexture class from an existing IDirect3DTexture9 object instance.
    BOOL Create(CGraphics *Graphics,IDirect3DTexture9 *Texture);
    BOOL Free();      // Free texture object
    BOOL IsLoaded();  // Returns TRUE if texture is loaded
    long GetWidth();  // Return width (pitch) of texture
    long GetHeight(); // Return height of texture
    D3DFORMAT GetFormat(); // Return texture storage format
    // Draw a 2-D portion of texture to device
    BOOL Blit(long DestX, long DestY,long SrcX = 0, long SrcY = 0,long Width = 0, long Height = 0,
		float XScale = 1.0f, float YScale = 1.0f,D3DCOLOR Color = 0xFFFFFFFF);                                                                
};

class CMaterial
{
protected:
    D3DMATERIAL9  m_Material;
public:
    CMaterial();
    D3DMATERIAL9 *GetMaterial();
    
	BOOL SetDiffuseColor (unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetDiffuseColor (unsigned char *Red, unsigned char *Green, unsigned char *Blue);
    BOOL SetAmbientColor (unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetAmbientColor (unsigned char *Red, unsigned char *Green, unsigned char *Blue); 
    BOOL SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);  
    BOOL SetEmissiveColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetEmissiveColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);

    BOOL  SetPower(float Power);
    float GetPower(float Power);
};

class CLight
{
protected:
    D3DLIGHT9 m_Light;
public:
    CLight();
    D3DLIGHT9 *GetLight();
    BOOL SetType(D3DLIGHTTYPE Type);
    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XPos, float YPos, float ZPos);
    BOOL GetPos(float *XPos, float *YPos, float *ZPos);

    BOOL Point(float XFrom, float YFrom, float ZFrom,float XAt,float YAt,float ZAt);
    BOOL GetDirection(float *XDir, float *YDir, float *ZDir);
	BOOL SetDirection(float XDir, float YDir, float ZDir);
   
	BOOL SetDiffuseColor (unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetDiffuseColor (unsigned char *Red, unsigned char *Green, unsigned char *Blue);
    BOOL SetSpecularColor(unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetSpecularColor(unsigned char *Red, unsigned char *Green, unsigned char *Blue);   
    BOOL SetAmbientColor (unsigned char Red, unsigned char Green, unsigned char Blue);
    BOOL GetAmbientColor (unsigned char *Red, unsigned char *Green, unsigned char *Blue);

    BOOL SetRange(float Range);
    float GetRange();  
    BOOL SetFalloff(float Falloff);
    float GetFalloff();  
    BOOL SetAttenuation0(float Attenuation);
    float GetAttenuation0();
    BOOL SetAttenuation1(float Attenuation);
    float GetAttenuation1();
    BOOL SetAttenuation2(float Attenuation);
    float GetAttenuation2();   
    BOOL SetTheta(float Theta);
    float GetTheta();  
    BOOL SetPhi(float Phi);
    float GetPhi();
};

class CWorldPosition
{
protected:
    BOOL  m_Billboard;
    float m_XPos,      m_YPos,      m_ZPos;
    float m_XRotation, m_YRotation, m_ZRotation;
    float m_XScale,    m_YScale,    m_ZScale;  
	D3DXMATRIX m_matTemp;
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matScale;
    D3DXMATRIX m_matRotation;
    D3DXMATRIX m_matTranslation;
    D3DXMATRIX *m_matCombine1;
    D3DXMATRIX *m_matCombine2;
public:
    CWorldPosition();

	void SetWorldMatrix(const D3DXMATRIX &Matrix) 
	{
		m_matWorld=Matrix;
	}

    D3DXMATRIX *GetMatrix(CGraphics *Graphics = NULL);
    BOOL SetCombineMatrix1(D3DXMATRIX *Matrix = NULL);
    BOOL SetCombineMatrix2(D3DXMATRIX *Matrix = NULL);

    BOOL Copy(CWorldPosition *DestPos);

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XAdd, float YAdd, float ZAdd);
    BOOL Rotate(float XRot, float YRot, float ZRot);
    BOOL RotateRel(float XAdd, float YAdd, float ZAdd);
    BOOL Scale(float XScale, float YScale, float ZScale);
    BOOL ScaleRel(float XAdd, float YAdd, float ZAdd);
    BOOL Update(CGraphics *Graphics = NULL);
    BOOL EnableBillboard(BOOL Enable = TRUE);
    float GetXPos();
    float GetYPos();
    float GetZPos();
    float GetXRotation();
    float GetYRotation();
    float GetZRotation();
    float GetXScale();
    float GetYScale();
    float GetZScale();
};

//class CCamera
//{
//protected:
//    float m_XPos, m_YPos, m_ZPos;
//    float m_XRot, m_YRot, m_ZRot;
//
//    float m_StartXPos, m_StartYPos, m_StartZPos;
//    float m_StartXRot, m_StartYRot, m_StartZRot;
//
//    float m_EndXPos, m_EndYPos, m_EndZPos;
//    float m_EndXRot, m_EndYRot, m_EndZRot;
//    
//    D3DXMATRIX m_matWorld;
//    D3DXMATRIX m_matTranslation;
//    D3DXMATRIX m_matRotation;
//
//public:
//    CCamera();
//
//    //D3DXMATRIX *GetMatrix(); // Get view transformation matrix
//	D3DXMATRIX &GetMatrix();
//    BOOL Update();           // Update transformation matrix
//
//    BOOL Move(float XPos, float YPos, float ZPos);
//    BOOL MoveRel(float XAdd, float YAdd, float ZAdd);
//    BOOL Rotate(float XRot, float YRot, float ZRot);
//    BOOL RotateRel(float XAdd, float YAdd, float ZAdd);
//    BOOL Point(float XEye, float YEye, float ZEye, float XAt, float YAt, float ZAt);
//
//    BOOL SetStartTrack();
//    BOOL SetEndTrack();
//    BOOL Track(float Time, float Length);
//
//    float GetXPos();
//    float GetYPos();
//    float GetZPos();
//    float GetXRotation();
//    float GetYRotation();
//    float GetZRotation();
//};

class CGameFont
{
private:
    ID3DXFont *m_Font;
public:
    CGameFont();
    ~CGameFont();
    ID3DXFont *GetFontCOM();

    BOOL Create(CGraphics *Graphics, const WCHAR* Name, long Size = 16, BOOL Bold = FALSE, 
		BOOL Italic = FALSE, BOOL Underline = FALSE, BOOL Strikeout = FALSE);
    BOOL Free();
	BOOL Print(const WCHAR* Text, long XPos, long YPos, long Width = 0, long Height = 0, 
		D3DCOLOR Color = 0xFFFFFFFF, DWORD Format = 0);
};

class CVertexBuffer
{
private:
    CGraphics              *m_Graphics;
    IDirect3DVertexBuffer9 *m_pVB;
    DWORD                   m_NumVertices;
    DWORD                   m_VertexSize;
    DWORD                   m_FVF;
    BOOL                    m_Locked;
    char                   *m_Ptr; 
public:
    CVertexBuffer();
    ~CVertexBuffer();
    IDirect3DVertexBuffer9 *GetVertexBufferCOM();
    unsigned long           GetVertexSize();
    unsigned long           GetVertexFVF();
    unsigned long           GetNumVertices();
    BOOL Create(CGraphics *Graphics, unsigned long NumVertices, DWORD Descriptor, long VertexSize);
    BOOL Free();
    BOOL IsLoaded();
    BOOL Set(unsigned long FirstVertex, unsigned long NumVertices, void *VertexList);
    BOOL Render(unsigned long FirstVertex, unsigned long NumPrimitives, DWORD Type);
	BOOL SendData();
    BOOL  Lock(unsigned long FirstVertex = 0, unsigned long NumVertices = 0);
    BOOL  Unlock();
    void *GetPtr();
};

