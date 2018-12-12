/*-------------------------------------------------------
GameCore Component
Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
2007-2008 SCY modified
-------------------------------------------------------*/
#include "Stdafx.h"
#include "Sound.h"

CSound::CSound()
{
	short i;
    // Initialize COM
    CoInitialize(NULL);
    m_hWnd   = NULL;
    m_Volume = 0;
    m_hThread = NULL;
    m_ThreadID = NULL;
    m_ThreadActive = FALSE;
    m_pDS = NULL;
    m_pDSBPrimary = NULL;
    for(i=0;i<32;i++)
        m_EventChannel[i] = NULL;
    for(i=0;i<33;i++)
        m_Events[i] = NULL;
}

CSound::~CSound()
{ 
	Shutdown();  
    CoUninitialize();// Uninitialize COM
}

BOOL CSound::Init(HWND hWnd, long Frequency, short Channels, short BitsPerSample, long CooperativeLevel)
{
    DSBUFFERDESC dsbd;
    WAVEFORMATEX wfex;
    short        i;
    // Shutdown system in case of prior install
    Shutdown();
    // Save parent window handle
    if((m_hWnd = hWnd) == NULL)
        return FALSE;
	// Save settings of sound setup
	if(CooperativeLevel == DSSCL_NORMAL) 
		CooperativeLevel = DSSCL_PRIORITY;
	m_CooperativeLevel = CooperativeLevel;
	m_Frequency        = Frequency;
	m_Channels         = Channels;
	m_BitsPerSample    = BitsPerSample;
	// create an IDirectSound8 object
	if(FAILED(DirectSoundCreate8(NULL, &m_pDS, NULL)))  
		return FALSE; 
	// Set cooperative mode
	if(FAILED(m_pDS->SetCooperativeLevel(m_hWnd, m_CooperativeLevel))) 
		return FALSE;
	// Get primary buffer control
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize        = sizeof(DSBUFFERDESC); 
	dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat   = NULL;
	if(FAILED(m_pDS->CreateSoundBuffer(&dsbd, &m_pDSBPrimary, NULL)))
		return FALSE;
	// Set the primary buffer format
	ZeroMemory(&wfex, sizeof(WAVEFORMATEX)); 
	wfex.wFormatTag      = WAVE_FORMAT_PCM; 
	wfex.nChannels       = (WORD)m_Channels;
	wfex.nSamplesPerSec  = m_Frequency;
	wfex.wBitsPerSample  = (WORD)m_BitsPerSample;
	wfex.nBlockAlign     = wfex.wBitsPerSample / 8 * wfex.nChannels;
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
	if(FAILED(m_pDSBPrimary->SetFormat(&wfex)))  
		return FALSE;
	// Create the events, plus an extra one for thread termination
	for(i=0;i<33;i++) 
	{ 
		if((m_Events[i] = CreateEvent(NULL,FALSE,FALSE,NULL)) == NULL)   
			return FALSE;  
	}
	// Create a thread for handling notifications
	if((m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleNotifications, 
		(LPVOID)this, 0, &m_ThreadID)) == NULL)    
		return FALSE;
	// Start main buffer playing
	if(FAILED(m_pDSBPrimary->Play(0, 0, DSBPLAY_LOOPING)))   
		return FALSE;

	// Set default volume to full
	SetVolume(100);
	return TRUE;
}

BOOL CSound::Shutdown()
{
    short i;
	// Go through all used sound channels and free them
	for(i=0;i<32;i++) 
	{
		if(m_EventChannel[i] != NULL)
		{    
			m_EventChannel[i]->Free();   
			m_EventChannel[i] = NULL; 
		}
		// Clear the event status  
		if(m_Events[i] != NULL) 
			ResetEvent(m_Events[i]);
	}
	// Stop the primary channel from playing
	if(m_pDSBPrimary != NULL)  
		m_pDSBPrimary->Stop();
	// Release the DirectSound objects
	ReleaseCOM(m_pDSBPrimary);
	ReleaseCOM(m_pDS);
	// Force a closure of the thread by triggering the last event and waiting for it to terminate
	if(m_hThread != NULL) 
	{ 
		if(m_Events[32] != NULL) 
		{ 
			while(m_ThreadActive == TRUE)        
				SetEvent(m_Events[32]);
		}
		else 
		{    
			// getting here means no event assigned on thread and need to terminate it - not desirable   
			TerminateThread(m_hThread, 0);   
		}
	}
	// Close all event handles
	for(i=0;i<33;i++) 
	{ 
		if(m_Events[i] != NULL)
		{
			CloseHandle(m_Events[i]);     
			m_Events[i] = NULL;   
		}
	}
	// Free the thread handle
	if(m_hThread != NULL) 
	{
		CloseHandle(m_hThread);  
		m_hThread = NULL; 
	}
	m_ThreadID = NULL;
	return TRUE;
}

IDirectSound8 *CSound::GetDirectSoundCOM()
{
    return m_pDS;
}

IDirectSoundBuffer *CSound::GetPrimaryBufferCOM()
{
    return m_pDSBPrimary;
}

BOOL CSound::AssignEvent(CSoundChannel *Channel, short *EventNum, HANDLE *EventHandle)
{
    short i;
    for(i=0;i<32;i++) 
	{  
		if(m_Events[i] != NULL && m_EventChannel[i] == NULL) 
		{   
			ResetEvent(m_Events[i]);    
			m_EventChannel[i] = Channel;
			*EventNum = i;   
			*EventHandle = m_Events[i];       
			return TRUE;  
		}
	}
	return FALSE;
}

BOOL CSound::ReleaseEvent(CSoundChannel *Channel, short *EventNum)
{
	if((unsigned short)*EventNum < 33 && m_EventChannel[*EventNum] == Channel)
	{ 
		ResetEvent(m_Events[*EventNum]);
		m_EventChannel[*EventNum] = NULL;
		*EventNum = -1;  
		return TRUE;
	} 
	return FALSE;
}

long CSound::GetVolume()
{
    return m_Volume;
}

BOOL CSound::SetVolume(long Percent)
{
    long Volume;
    // Set the sound main volume
    if(m_pDSBPrimary == NULL)
        return FALSE;
    // calculate a usable volume level
    if(!Percent)
        Volume = DSBVOLUME_MIN;
    else 
        Volume = -20 * (100 - (Percent % 101));
    if(FAILED(m_pDSBPrimary->SetVolume(Volume)))
        return FALSE;
    m_Volume = Percent % 101;
    return TRUE;
}

DWORD CSound::HandleNotifications(LPVOID lpvoid)
{
    DWORD   dwResult, Channel;
    CSound *SoundPtr;
    BOOL    Complete;
    MSG     Msg;
    SoundPtr = (CSound*)lpvoid;
    SoundPtr->m_ThreadActive = TRUE;
    Complete = FALSE;
    while(Complete == FALSE) 
	{
		// Wait for a message  
		dwResult = MsgWaitForMultipleObjects(33, SoundPtr->m_Events,FALSE, INFINITE, QS_ALLEVENTS);                                                                               
		// Get channel # to update 
		Channel = dwResult - WAIT_OBJECT_0;
		// Check for channel update 
		if(Channel >=0 && Channel < 32) 
		{   
			if(SoundPtr->m_EventChannel[Channel] != NULL)      
				SoundPtr->m_EventChannel[Channel]->Update();
		} 
		else if(Channel == 32)// Check for thread closure		
		{
			Complete = TRUE; 
		} 
		else if(Channel > 32) // Check for waiting messages
		{    
			while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
			{
       
				if(Msg.message == WM_QUIT) 
				{       
					Complete = TRUE;         
					break;        
				}     
			}    
		}
	}
	SoundPtr->m_ThreadActive = FALSE;
	return 0L;
}

BOOL CSound::Restore()
{
    short i;
    // Handle primary 
    if(m_pDSBPrimary != NULL)
        m_pDSBPrimary->Restore();
    // Handle all used sound channels
    for(i=0;i<32;i++) 
	{ 
		if(m_EventChannel[i] != NULL)   
			m_EventChannel[i]->m_pDSBuffer->Restore();
	}
	return TRUE;
}


CSoundData::CSoundData()
{
    m_Frequency     = 22050;
    m_Channels      = 1;
    m_BitsPerSample = 16;
    m_fp       = NULL;
    m_Ptr      = m_Buf = NULL;
    m_StartPos = m_Pos = 0;
    m_Size     = m_Left = 0;
}

CSoundData::~CSoundData()
{
    Free();
}

BOOL CSoundData::Create()
{
    return Create(m_Size);
}

BOOL CSoundData::Create(long Size)
{
    // Free prior allocated data
    Free();
    // Check for valid size
    if((m_Size = Size) == 0)
        return FALSE;
    // Create a new buffer
    m_Buf = new char[m_Size];
    if(m_Buf == NULL)
        return FALSE;
    // Point to new buffer
    m_Ptr = m_Buf;
    m_fp  = NULL;
    // Clear out new buffer
    ZeroMemory(m_Buf, m_Size);
    return TRUE;
}

BOOL CSoundData::Free()
{
    if(m_Buf != NULL) 
	{  
		delete[] m_Buf;  
		m_Buf = NULL;
	}
	m_Ptr = NULL;
	m_Size = 0;
	return TRUE;
}

char *CSoundData::GetPtr()
{
    return m_Buf;
}

long CSoundData::GetSize()
{
    return m_Size;
}

BOOL CSoundData::SetFormat(long Frequency, short Channels, short BitsPerSample)
{
    m_Frequency = Frequency;
    m_Channels = Channels;
    m_BitsPerSample = BitsPerSample;
    return TRUE;
}

BOOL CSoundData::SetSource(FILE *fp, long Pos, long Size)
{
    m_fp = fp;
    m_Ptr = NULL;
    if(Pos != -1)
        m_StartPos = m_Pos = Pos;
    if(Size != -1)
        m_Size = m_Left = Size;
    return TRUE;
}

BOOL CSoundData::SetSource(void *Ptr, long Pos, long Size)
{
    m_fp = NULL;
    m_Ptr = (char*)Ptr;
    if(Pos != -1)
        m_StartPos = m_Pos = Pos;
    if(Size != -1)
        m_Size = m_Left = Size;
    return TRUE;
}

BOOL CSoundData::LoadWAV(wchar_t *Filename, FILE *fp)
{
    if(LoadWAVHeader(Filename, fp) == FALSE)
        return FALSE;
    if(Create() == FALSE)
        return FALSE;
    // open file, seek to position and read in data
    if(Filename != NULL)
	{
		if(_wfopen_s(&fp, Filename, L"rb") != 0)
			return FALSE; 
	}
	fseek(fp, m_StartPos, SEEK_SET);
    fread(m_Buf, 1, m_Size, fp);
    m_StartPos = m_Pos = 0;
    // close up file
    if(Filename != NULL)
        fclose(fp);
    return TRUE;
}

BOOL CSoundData::LoadWAVHeader(wchar_t *Filename, FILE *fp)
{
    sWaveHeader Hdr;
    BOOL ReturnVal;
    long Pos;
    if(Filename == NULL && fp == NULL)
        return FALSE;
    if(Filename != NULL) 
	{ 
		if (_wfopen_s(&fp, Filename, L"rb") != 0)
			return FALSE;
	}
	// Save position in file
	Pos = ftell(fp);
	// Read in header and parse
    ReturnVal = FALSE;
    fread(&Hdr, 1, sizeof(sWaveHeader), fp);
    if(!memcmp(Hdr.RiffSig, "RIFF", 4) && !memcmp(Hdr.WaveSig, "WAVE", 4) && 
		!memcmp(Hdr.FormatSig, "fmt ", 4) && !memcmp(Hdr.DataSig, "data", 4)) 
	{
		m_Frequency     = Hdr.SampleRate;
        m_Channels      = Hdr.Channels;
        m_BitsPerSample = Hdr.BitsPerSample;
        m_Size = m_Left = Hdr.DataSize;
        m_StartPos = m_Pos = ftell(fp);
        ReturnVal = TRUE;
    }
    // Close if we opened file otherwise return to original position
    if(Filename != NULL)
        fclose(fp);
    else 
        fseek(fp, Pos, SEEK_SET);
    return ReturnVal;
}

BOOL CSoundData::Copy(CSoundData *Source)
{
    if(Source == NULL)
        return FALSE;
    m_Frequency     = Source->m_Frequency;
    m_Channels      = Source->m_Channels;
    m_BitsPerSample = Source->m_BitsPerSample;
    m_fp       = Source->m_fp;
    m_Ptr      = Source->m_Ptr;
    m_Size     = Source->m_Size;
    m_Left     = Source->m_Left;
    m_Pos      = Source->m_Pos;
    m_StartPos = Source->m_StartPos;
    return TRUE;
}


CSoundChannel::CSoundChannel()
{
    m_Sound     = NULL;
    m_pDSBuffer = NULL;
    m_pDSNotify = NULL;

    m_Event     = -1;

    m_Volume    = 0;
    m_Pan       = 0;
    m_Frequency = 0;
    m_Playing   = FALSE;
}

CSoundChannel::~CSoundChannel()
{
    Free();
}

IDirectSoundBuffer8 *CSoundChannel::GetSoundBufferCOM()
{
    return m_pDSBuffer;
}

IDirectSoundNotify8 *CSoundChannel::GetNotifyCOM()
{
    return m_pDSNotify;
}    

BOOL CSoundChannel::Create(CSound *Sound, long Frequency, short Channels, short BitsPerSample)
{
    DSBUFFERDESC dsbd;
    WAVEFORMATEX wfex;
    HANDLE       hEvent;
    DSBPOSITIONNOTIFY   dspn[4];
    IDirectSoundBuffer *pDSBuffer;
    // Free a prior channel
    Free();
    if((m_Sound = Sound) == NULL)
        return FALSE;
    if(m_Sound->GetDirectSoundCOM() == NULL)
        return FALSE;
    // Save playback format
    m_Frequency     = Frequency;
    m_BitsPerSample = BitsPerSample;
    m_Channels      = Channels;
    // Create a new sound buffer for this channel Using specified format
    ZeroMemory(&wfex, sizeof(WAVEFORMATEX)); 
    wfex.wFormatTag      = WAVE_FORMAT_PCM; 
    wfex.nChannels       = (WORD)m_Channels;
    wfex.nSamplesPerSec  = m_Frequency;
    wfex.wBitsPerSample  = (WORD)m_BitsPerSample;
    wfex.nBlockAlign     = wfex.wBitsPerSample / 8 * wfex.nChannels;
    wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE;
    dsbd.dwBufferBytes = g_SoundBufferSize;
    dsbd.lpwfxFormat   = &wfex;
    if(FAILED(m_Sound->GetDirectSoundCOM()->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL)))
        return FALSE;
    // query for newer interface
    if(FAILED(pDSBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_pDSBuffer)))
	{
        pDSBuffer->Release();
        return FALSE;
    }
    // Release old object - we have the newer one now
    pDSBuffer->Release();
    // Create the notification interface
    if(FAILED(m_pDSBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&m_pDSNotify)))
        return FALSE;
    // Get an event for this
    if(m_Sound->AssignEvent(this, &m_Event, &hEvent) == FALSE)
        return FALSE;
    // Setup the 4 notification positions
    dspn[0].dwOffset = g_SoundBufferChunk - 1;
    dspn[0].hEventNotify = hEvent;
    dspn[1].dwOffset = g_SoundBufferChunk * 2 - 1;
    dspn[1].hEventNotify = hEvent;
    dspn[2].dwOffset = g_SoundBufferChunk * 3 - 1;
    dspn[2].hEventNotify = hEvent;
    dspn[3].dwOffset = g_SoundBufferSize - 1;
    dspn[3].hEventNotify = hEvent;
    if(FAILED(m_pDSNotify->SetNotificationPositions(4, dspn)))
        return FALSE;
    // set the pan and default volume
    SetVolume(100);
    SetPan(0);
    return TRUE;
}

BOOL CSoundChannel::Create(CSound *Sound, CSoundData *SoundDesc)
{
    return Create(Sound, SoundDesc->m_Frequency, SoundDesc->m_Channels, SoundDesc->m_BitsPerSample);
}

BOOL CSoundChannel::Free()
{
    // Stop any playback
    Stop();
    // Release the notification
    ReleaseCOM(m_pDSNotify);
    // Release the buffer
    ReleaseCOM(m_pDSBuffer);
    // Release event from parent CSound class
    m_Sound->ReleaseEvent(this, &m_Event);
    // Set to no parent sound
    m_Sound = NULL;
    return TRUE;
}

BOOL CSoundChannel::Play(CSoundData *Desc, long VolumePercent, long Loop)
{
    if(Desc == NULL)
        return FALSE;
    if(m_pDSBuffer == NULL)
        return FALSE;
    if(m_pDSNotify == NULL)
        return FALSE;
    // Stop any playback
    Stop();
    // Restore a lost buffer just in case
    m_pDSBuffer->Restore();
    // Setup playing information
    m_Desc.Copy(Desc);
    // Set looping data
    m_Loop = Loop;
    // Calculate stop section position
    if(!m_Loop)
        m_StopSection = -1;
    else
        m_StopSection = (short)(((m_Desc.m_Size * m_Loop) % g_SoundBufferSize) / g_SoundBufferChunk);
    // Buffer in data
    m_LoadSection = 0;
    BufferData();
    BufferData();
    BufferData();
    BufferData();
    // Set the volume
    SetVolume(VolumePercent);
    // Set position and begin play
    m_NextNotify = 0;
    if(FAILED(m_pDSBuffer->SetCurrentPosition(0)))
        return FALSE;
    if(FAILED(m_pDSBuffer->Play(0,0,DSBPLAY_LOOPING)))
        return FALSE;
    // Flag as playing
    m_Playing = TRUE;
    return TRUE;
}

BOOL CSoundChannel::Stop()
{
    if(m_pDSBuffer)
        m_pDSBuffer->Stop();
    m_Playing = FALSE;
    return TRUE;
}

long CSoundChannel::GetVolume()
{
    return m_Volume;
}

BOOL CSoundChannel::SetVolume(long Percent)
{
    long Volume;
    if(m_pDSBuffer == NULL)
        return FALSE;
    // calculate a usable volume level
    if(!Percent)
        Volume = DSBVOLUME_MIN;
    else 
        Volume = -20 * (100 - (Percent % 101));

    if(FAILED(m_pDSBuffer->SetVolume(Volume)))
        return FALSE;
    m_Volume = Percent % 101;
    return TRUE;
}

signed long CSoundChannel::GetPan()
{
    return m_Pan;
}

BOOL CSoundChannel::SetPan(signed long Level)
{
    signed long Pan;
    if(m_pDSBuffer == NULL)
        return FALSE;
    // calculate a usable setting
    if(Level < 0) 
        Pan = DSBPAN_LEFT / 100 * ((-Level) % 101);
    else
        Pan = DSBPAN_RIGHT / 100 * (Level % 101);
    if(FAILED(m_pDSBuffer->SetPan(Pan)))
        return FALSE;
    m_Pan = Level % 101;
    return TRUE;  
}

long CSoundChannel::GetFrequency()
{
    return m_Frequency;
}

BOOL CSoundChannel::SetFrequency(long Level)
{
    if(m_pDSBuffer == NULL)
        return FALSE;
    if(FAILED(m_pDSBuffer->SetFrequency(Level)))
        return FALSE;
    m_Frequency = Level;
    return TRUE;
}

BOOL CSoundChannel::IsPlaying()
{
    if(m_Sound == NULL || m_pDSBuffer == NULL || m_pDSNotify == NULL)
        return FALSE;
    return m_Playing;
}

BOOL CSoundChannel::BufferData()
{
    long Pos, Size;
    long ToLoad, LoadPos;
    char *Ptr;
    if(m_pDSBuffer == NULL)
        return FALSE;
    // Setup position to load in
    Pos  = (m_LoadSection % 4) * g_SoundBufferChunk;
    if(FAILED(m_pDSBuffer->Lock(Pos, g_SoundBufferChunk, (void**)&Ptr, (DWORD*)&Size, NULL, NULL, 0)))
        return FALSE;   
    // Clear out buffer if nothing left to load
    if(!m_Desc.m_Left)
        ZeroMemory(Ptr, Size);
    else 
	{
        // Load in the data - take looping into account
        ToLoad = Size;
        LoadPos = 0;
        for(;;) 
		{   
			if(m_Desc.m_Left > ToLoad) 
			{     
				if(m_Desc.m_fp != NULL) 
				{       
					fseek(m_Desc.m_fp, m_Desc.m_Pos, SEEK_SET);         
					fread(&Ptr[LoadPos], 1, ToLoad, m_Desc.m_fp);       
				} 
				else          
					memcpy(&Ptr[LoadPos], &m_Desc.m_Ptr[m_Desc.m_Pos], ToLoad);       
				m_Desc.m_Left -= ToLoad;       
				m_Desc.m_Pos += ToLoad;      
				break;   
			} 
			else
			{       
				if(m_Desc.m_fp != NULL) 
				{       
					fseek(m_Desc.m_fp, m_Desc.m_Pos, SEEK_SET);        
					fread(&Ptr[LoadPos], 1, m_Desc.m_Left, m_Desc.m_fp);     
				} 
				else         
					memcpy(&Ptr[LoadPos], &m_Desc.m_Ptr[m_Desc.m_Pos], m_Desc.m_Left);      
				ToLoad -= m_Desc.m_Left;    
				LoadPos += m_Desc.m_Left;    
				// Check if we need to stop loop      
				if(m_Loop >= 1)
				{        
					m_Loop--;         
					if(!m_Loop) 
					{         
						// Clear out remaining buffer space          
						if(ToLoad)            
							ZeroMemory(&Ptr[LoadPos], ToLoad);        
						m_Desc.m_Left = 0L;           
						break;       
					}
				}    
				m_Desc.m_Pos = m_Desc.m_StartPos;     
				m_Desc.m_Left = m_Desc.m_Size;     
				// See if we need to stop loading data     
				if(!ToLoad)         
					break;    
			} 
		}
	}
	// Unlock the buffer
    m_pDSBuffer->Unlock(Ptr, Size, NULL, 0);
    // Mark next section to load
    if((m_LoadSection+=1) > 3)
        m_LoadSection = 0;
    return TRUE;
}

BOOL CSoundChannel::Update()
{
    // Check for end of sound
    if(m_NextNotify == m_StopSection && !m_Desc.m_Left)
	{
        Stop();
    } 
	else
	{
		// Buffer in more data
		BufferData();  
		if((m_NextNotify+=1) > 3)  
			m_NextNotify = 0;
	}
	return TRUE;
}

CMediaPlayer::CMediaPlayer()
{
	m_pGraphBuilder =NULL;
	m_pMediaControl =NULL;
	m_pMediaEvent   =NULL;
	m_pVideoWindow  =NULL;
	m_pMediaPosition=NULL;
	m_hWnd=NULL;
	m_bPlaying      =false;
	m_bFullScreen   =false;
}

CMediaPlayer::~CMediaPlayer()
{
	Shutdown();
}

bool CMediaPlayer::Init(HWND hWnd)
{
	//Shutdown();
	CoInitialize(NULL);
	m_hWnd=hWnd;
	if(FAILED(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder,(void**)&m_pGraphBuilder)))
		return false;
	m_pGraphBuilder->QueryInterface(IID_IMediaControl,(void**)&m_pMediaControl);
	m_pGraphBuilder->QueryInterface(IID_IMediaEventEx,(void**)&m_pMediaEvent);
	m_pGraphBuilder->QueryInterface(IID_IVideoWindow,(void**)&m_pVideoWindow);
	m_pGraphBuilder->QueryInterface(IID_IMediaPosition,(void**)&m_pMediaPosition);
	m_pGraphBuilder->QueryInterface(IID_IBasicAudio,(void**)&m_pBasicAudio);
	//m_pVideoWindow->put_MessageDrain((OAHWND)rhWnd);
	
	return true;
}

int CMediaPlayer::GetCurrentPos()
{
	double dLength,dCurLength;
	if(m_pMediaPosition==NULL)
		return 0;
	m_pMediaPosition->get_Duration(&dLength);
	m_pMediaPosition->get_CurrentPosition(&dCurLength);
	return (int)((dCurLength/dLength)*10000);
}

int CMediaPlayer::GetCurrentTime()
{
	if(m_pMediaPosition==NULL)
		return 0;
	double dCurLength;
	m_pMediaPosition->get_CurrentPosition(&dCurLength);
	return (int)dCurLength;
}

int CMediaPlayer::GetDuration()
{
	if(m_pMediaPosition==NULL)
		return 0;
	double dCurLength;
	m_pMediaPosition->get_Duration(&dCurLength);
	return (int)dCurLength;
}

double CMediaPlayer::GetCurrentSpeed()
{
	if(m_pMediaPosition==NULL)
		return false;
	double rate;
	m_pMediaPosition->get_Rate(&rate);   
	return rate/m_dOriginalRate;

}

void CMediaPlayer::PutCurrentPos(double dPos)
{
	if(m_pMediaPosition==NULL)
		return;
	double dLength;
	m_pMediaPosition->get_Duration(&dLength);
	m_pMediaPosition->put_CurrentPosition(dLength/10000.0*dPos);
}

bool CMediaPlayer::Free()
{
	return false;
}

void CMediaPlayer::SetFullScreen()
{
	if(m_pVideoWindow==NULL)
		return;
	//m_pVideoWindow->put_FullScreenMode(0);
	m_bFullScreen = !m_bFullScreen;
	if(m_bFullScreen)
	{
		m_pVideoWindow->put_Owner(NULL);
		m_pVideoWindow->GetWindowPosition(&m_iLeft,&m_iTop,&m_iWidth,&m_iHeight);
		int iWidth=GetSystemMetrics(SM_CXSCREEN); 
        int iHeight=GetSystemMetrics(SM_CYSCREEN); 	
		m_pVideoWindow->SetWindowPosition(0,0,iWidth,iHeight);
		//m_pVideoWindow->HideCursor(0);
	}
	else
	{
		m_pVideoWindow->put_Owner((OAHWND)m_hWnd);
		m_pVideoWindow->SetWindowPosition(m_iLeft,m_iTop,m_iWidth,m_iHeight);
	}
}
bool CMediaPlayer::Shutdown()
{
	ReleaseCOM(m_pBasicAudio);
	ReleaseCOM(m_pMediaPosition);
	ReleaseCOM(m_pVideoWindow);
	ReleaseCOM(m_pMediaEvent);
	ReleaseCOM(m_pMediaControl);
	ReleaseCOM(m_pGraphBuilder);
	CoUninitialize();
	return true;
}

bool CMediaPlayer::Play()
{
	m_bPlaying=true;   
	if(m_pMediaControl==NULL || m_pMediaPosition==NULL)
		return false;
	if(FAILED(m_pMediaControl->Run()))
		return false;
	//PutVolume(0);
	m_pMediaPosition->get_Rate(&m_dOriginalRate);   
	return true;
}

bool CMediaPlayer::Pause()
{
	m_bPlaying=false;   
	if(m_pMediaControl==NULL)
		return false;
	if(FAILED(m_pMediaControl->Pause()))
		return false;;
	return true;
}

bool CMediaPlayer::Stop()
{
	m_bPlaying=false;  
	if(m_pMediaControl==NULL || m_pMediaPosition==NULL)
		return false;
	if(FAILED(m_pMediaControl->Stop()))
		return false;
	m_pMediaPosition->put_CurrentPosition(0);
	return true;
}

bool CMediaPlayer::Fast()
{
	if(m_pMediaPosition==NULL)
		return false;
	double rate;
	m_pMediaPosition->get_Rate(&rate);   
	m_pMediaPosition->put_Rate(rate*1.1f);	
	return true;
}

bool CMediaPlayer::Slow()
{
	if(m_pMediaPosition==NULL)
		return false;
	double rate;
	m_pMediaPosition->get_Rate(&rate);    
	m_pMediaPosition->put_Rate(rate*0.9f);
	return true;
}

bool CMediaPlayer::Normal()
{
	if(m_pMediaPosition==NULL)
		return false;
	m_pMediaPosition->put_Rate(1.0f);
	return true;
}

bool CMediaPlayer::Esc()
{
	if(m_pMediaControl==NULL)
		return false;
	m_pMediaControl->Stop();
	double dLength;
	m_pMediaPosition->get_Duration(&dLength);
	m_pMediaPosition->put_CurrentPosition(dLength);
	m_pMediaControl->Run();
	::Sleep(2);
	return true;
}

bool CMediaPlayer::Render(WCHAR *wFileName)
{
	if(m_pGraphBuilder == NULL || m_pMediaControl == NULL)// Error checking
        return false;
	m_pMediaEvent->SetNotifyWindow((OAHWND)m_hWnd,WM_GRAPH_NOTIFY,NULL);//register a window to process event notifications
	m_pMediaControl->Stop();//Stop the current song 
	//WCHAR wFileName[MAX_PATH];
	//mbstowcs(wFileName,FileName,MAX_PATH);//字节串→字符串
	//MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,FileName,strlen(FileName),wFileName,0);
	if(FAILED(m_pGraphBuilder->RenderFile(wFileName,NULL)))
		return false;
	m_pVideoWindow->put_Owner((OAHWND)m_hWnd);//在游戏窗口内播放视频
	m_pVideoWindow->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS);
	RECT rWindow;
	GetClientRect(m_hWnd,&rWindow);
	m_pVideoWindow->SetWindowPosition(0,0,rWindow.right,rWindow.bottom);
	m_pVideoWindow->put_Visible(OATRUE);
	return true;
}

bool CMediaPlayer::PutVolume(long Percent)
{
	if(m_pBasicAudio==NULL)
		return false;
	long lVolume;
    if(!Percent)
        lVolume = DSBVOLUME_MIN;
    else 
        lVolume = -20 * (100 - (Percent % 101));
    if(FAILED(m_pBasicAudio->put_Volume(lVolume)))
        return false;
    m_lVolume = Percent % 101;
    return true;
}

long CMediaPlayer::GetVolume()
{
	if(m_pBasicAudio==NULL)
		return 0;
	long lVolume;//x mod y = x - y (x div y)
	m_pBasicAudio->get_Volume(&lVolume);//100%101=100-101*(100/101)
	return m_lVolume=100+lVolume/20;	
	//return m_lVolume;
}

bool CMediaPlayer::Playing()
{ 
    if(m_pMediaEvent == NULL)// Error checking
        return false;
    long Event, Param1, Param2;// Get event and handle it
    m_pMediaEvent->GetEvent(&Event, &Param1, &Param2, 1);
    if(Event == EC_COMPLETE) 
    {
        m_pMediaEvent->FreeEventParams(Event, Param1, Param2);
        return true;
    }
    m_pMediaEvent->FreeEventParams(Event, Param1, Param2);
    return false;
}
