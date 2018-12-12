/*-------------------------------------------------------
GameCore Component
Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
2007-2008 SCY modified
-------------------------------------------------------*/
#include "Stdafx.h"
#include "System.h"

CApplication::CApplication()
{	
	g_pApplication = this;// Save instance handle  
    m_hInst = GetModuleHandle(NULL);// Get the instance handle  
	wcsncpy_s(m_Class, L"AppClass", MAX_PATH);// Set default 
	wcsncpy_s(m_Caption, L"Application Caption", MAX_PATH);
    m_Style  = WS_OVERLAPPEDWINDOW;
    m_XPos   = 0;
    m_YPos   = 0;
    m_Width  = 256;
    m_Height = 256;
    m_wcex.cbSize        = sizeof(WNDCLASSEX);
    m_wcex.style         = CS_CLASSDC;
    m_wcex.lpfnWndProc   = AppWindowProc;
    m_wcex.cbClsExtra    = 0;
    m_wcex.cbWndExtra    = 0;
    m_wcex.hInstance     = m_hInst;
    m_wcex.hIcon         = LoadIcon(m_hInst, MAKEINTRESOURCE("IDI_ICON1"));
    m_wcex.hCursor       = LoadCursor(NULL,IDC_ARROW);
    m_wcex.hbrBackground = NULL;
    m_wcex.lpszMenuName  = NULL;
    m_wcex.lpszClassName = m_Class;
    m_wcex.hIconSm       = LoadIcon(m_hInst, MAKEINTRESOURCE("IDI_ICON1"));
}

HWND CApplication::GethWnd()
{
	return m_hWnd;
}

HINSTANCE CApplication::GethInst()
{
    return m_hInst;
}

BOOL CApplication::Run()
{
	MSG Msg;   
    if(!RegisterClassEx(&m_wcex))// Register window class
        return FALSE;
    // Create the Main Window
    m_hWnd = CreateWindow(m_Class, m_Caption,m_Style,m_XPos,m_YPos,m_Width,m_Height,NULL,NULL,m_hInst,NULL);
    if(!m_hWnd)
        return FALSE;
    // Show and update the window
    ShowWindow(m_hWnd, SW_NORMAL);
    UpdateWindow(m_hWnd);
    // Make sure client area is correct size
    Resize(m_Width, m_Height);
    // Initialize COM
    CoInitialize(NULL); 
	if(Init() == TRUE) 
	{   
		// Enter the message pump   
		ZeroMemory(&Msg, sizeof(MSG));  
		do
		{
			// Handle Windows messages (if any)     
			if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) 
			{     
				TranslateMessage(&Msg);      
				DispatchMessage(&Msg);     
			} 	
			else 
			{     			
				if(Frame() == FALSE)        					
					break; 
						
			} 
		}while(Msg.message != WM_QUIT);
	} 
	Shutdown();
	// Shutdown COM
	CoUninitialize();
	// Unregister the window class
	UnregisterClass(m_Class, m_hInst);
	return TRUE;
}

BOOL CApplication::Error(BOOL Fatal, char *Text, ...)
{
	//wchar_t CaptionText[12];
	//wchar_t ErrorText[2048];
	//va_list valist;
	//// Build the message box caption based on fatal flag
	//if(Fatal == FALSE)
	//	strcpy(CaptionText, "Error");
	//else 
	//	strcpy(CaptionText, "Fatal Error");
	//// Build variable text buffer
	//va_start(valist, Text);
	//vsprintf(ErrorText, Text, valist); 
	//va_end(valist); 
	//// Display the message box
	//MessageBox(NULL, ErrorText, CaptionText, MB_OK | MB_ICONEXCLAMATION); 
	//// Post a quit message if error was fatal 
	//if(Fatal == TRUE)   
	//	PostQuitMessage(0);
	return TRUE;
}

BOOL CApplication::Move(long XPos, long YPos)
{ 
	RECT ClientRect;
    GetClientRect(m_hWnd, &ClientRect);
    MoveWindow(m_hWnd, XPos, YPos, ClientRect.right, ClientRect.bottom, TRUE);
    return TRUE;
}

BOOL CApplication::Resize(long Width, long Height)
{ 
	RECT WndRect, ClientRect;
    long WndWidth, WndHeight;
    GetWindowRect(m_hWnd, &WndRect);
    GetClientRect(m_hWnd, &ClientRect);
    WndWidth  = (WndRect.right  - (ClientRect.right  - Width))  - WndRect.left;
    WndHeight = (WndRect.bottom - (ClientRect.bottom - Height)) - WndRect.top;
    MoveWindow(m_hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);
    return TRUE;
}

BOOL CApplication::ShowCursor(BOOL Show)
{
	::ShowCursor(Show);
	return TRUE;
}

// The message procedure - empty except for destroy message
long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{  
	switch(uMsg) 
	{  
	case WM_DESTROY: 
		PostQuitMessage(0); 
		return 0;
	case WM_SYSKEYDOWN://ÆÁ±ÎALT²Ëµ¥µ¯³ö¼ü
		return 0;
    default:
		return g_pApplication->MsgProc(hWnd, uMsg, wParam, lParam);
	}
}

