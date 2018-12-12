/*-------------------------------------------------------
Core_System.h
GameCore Component
Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
2007-2008 SCY modified
-------------------------------------------------------*/
#pragma once

class CApplication
{ 
private:
	HINSTANCE     m_hInst;
    HWND          m_hWnd;
protected:
    wchar_t       m_Class[MAX_PATH];
	wchar_t       m_Caption[MAX_PATH];
    WNDCLASSEX    m_wcex;
    DWORD         m_Style;
    DWORD         m_XPos;
    DWORD         m_YPos;
    DWORD         m_Width;
    DWORD         m_Height; 
public:
    CApplication();
    HWND      GethWnd();
    HINSTANCE GethInst();
    BOOL Run();
    BOOL Error(BOOL Fatal, char *Text, ...);
    BOOL Move(long XPos, long YPos);
    BOOL Resize(long Width, long Height);
    BOOL ShowCursor(BOOL Show = TRUE);
    virtual int PASCAL MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return (int)DefWindowProc(hWnd, uMsg, wParam, lParam); }
    virtual BOOL Init()       { return TRUE; }
    virtual BOOL Shutdown()   { return TRUE; }
    virtual BOOL Frame()      { return TRUE; }
};

static CApplication *g_pApplication = NULL;
static long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
enum Purposes 
{
    NOPURPOSE = 0,
    INITPURPOSE,
    SHUTDOWNPURPOSE,
    FRAMEPURPOSE
};   

