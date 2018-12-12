/*-------------------------------------------------------
GameCore Component
Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
2007-2008 SCY modified
-------------------------------------------------------*/
#pragma once

// Macros to help create patches numbers
#define PATCH(m,l,p) ((m << 16) | (l << 8) | p)
#define PATCHMSB(x)  ((x >> 16) & 255)
#define PATCHLSB(x)  ((x >>  8) & 255)
#define PATCHNUM(x)  (x & 255)
#define WM_GRAPH_NOTIFY (WM_APP+1)
// These are the fixed sizes for sound channel buffers
const long g_SoundBufferSize  = 65536;
const long g_SoundBufferChunk = g_SoundBufferSize / 4;
// Forward class declarations
class CSound;
class CSoundData;
class CSoundChannel;
class CMusicChannel;

class CSound
{
protected:
	// Sound system related
    HWND                      m_hWnd;
    long                      m_Volume;
    HANDLE                    m_Events[33];
    CSoundChannel            *m_EventChannel[32];
    HANDLE                    m_hThread;
    DWORD                     m_ThreadID;
    BOOL                      m_ThreadActive;
    static DWORD              HandleNotifications(LPVOID lpvoid);
    // Sound related
    IDirectSound8            *m_pDS;
    IDirectSoundBuffer       *m_pDSBPrimary;
    long                      m_CooperativeLevel;
    long                      m_Frequency;
    short                     m_Channels;
    short                     m_BitsPerSample; 
public:
    CSound();
    ~CSound();
    // Assign and release events
    BOOL AssignEvent(CSoundChannel *Channel, short *EventNum, HANDLE *EventHandle);
    BOOL ReleaseEvent(CSoundChannel *Channel, short *EventNum);
    // Functions to retrieve COM interfaces
    IDirectSound8            *GetDirectSoundCOM();
    IDirectSoundBuffer       *GetPrimaryBufferCOM();
    // Init and shutdown functions
    BOOL Init(HWND hWnd, long Frequency = 22050, short Channels = 1, short BitsPerSample = 16, long CooperativeLevel = DSSCL_PRIORITY);
    BOOL Shutdown();
    // Volume get/set
    long GetVolume();
    BOOL SetVolume(long Percent);
    // Restore system to known state
    BOOL Restore();
};

class CSoundData
{
	friend class CSoundChannel; 
protected:
    long  m_Frequency;
    short m_Channels;
    short m_BitsPerSample;
    FILE *m_fp;
    char *m_Ptr;
    char *m_Buf; 
    long  m_Size;
    long  m_Left;
    long  m_StartPos;
    long  m_Pos; 
public:
    CSoundData();
    ~CSoundData();
    char *GetPtr();
    long GetSize();
    BOOL Create();
    BOOL Create(long Size);
    BOOL Free();
    BOOL SetFormat(long Frequency, short Channels, short BitsPerSample);
    BOOL SetSource(FILE *fp, long Pos = -1, long Size = -1);
    BOOL SetSource(void *Ptr, long Pos = -1, long Size = -1);
    BOOL LoadWAV(wchar_t *Filename, FILE *fp = NULL);
	BOOL LoadWAVHeader(wchar_t *Filename, FILE *fp = NULL);
    BOOL Copy(CSoundData *Source);
};

class CSoundChannel
{
	friend class CSound;
protected:
    CSound              *m_Sound;
    IDirectSoundBuffer8 *m_pDSBuffer;
    IDirectSoundNotify8 *m_pDSNotify;
    short                m_Event;

    long                 m_Volume;
    signed long          m_Pan;
    BOOL                 m_Playing;
    long                 m_Loop;

    long                 m_Frequency;
    short                m_BitsPerSample;
    short                m_Channels;

    CSoundData           m_Desc;

    short                m_LoadSection;
    short                m_StopSection;
    short                m_NextNotify;

    BOOL BufferData();
    BOOL Update();  
public:
    CSoundChannel();
    ~CSoundChannel();
    IDirectSoundBuffer8 *GetSoundBufferCOM();
    IDirectSoundNotify8 *GetNotifyCOM(); 
    BOOL Create(CSound *Sound, long Frequency = 22050, short Channels = 1, short BitsPerSample = 16);
    BOOL Create(CSound *Sound, CSoundData *SoundDesc);
    BOOL Free();  
    BOOL Play(CSoundData *Desc, long VolumePercent = 100, long Loop = 1);
    BOOL Stop();
    long GetVolume();
    BOOL SetVolume(long Percent);
    signed long GetPan();
    BOOL SetPan(signed long Level);
    long GetFrequency();
    BOOL SetFrequency(long Level);  
    BOOL IsPlaying();
};

class CMediaPlayer
{
private:
	IGraphBuilder  *m_pGraphBuilder;
    IMediaControl  *m_pMediaControl;
    IMediaEventEx  *m_pMediaEvent;
	IVideoWindow   *m_pVideoWindow;
	IMediaPosition *m_pMediaPosition;
	IBasicAudio    *m_pBasicAudio;
	HWND            m_hWnd;
	bool            m_bPlaying;
	bool            m_bFullScreen;
	long            m_iLeft;
	long            m_iTop;
	long            m_iWidth;
	long            m_iHeight;
	long            m_lVolume;
	double          m_dOriginalRate;
public:
	CMediaPlayer();
	~CMediaPlayer();
	bool Init(HWND hWnd);
	bool Free();
	bool Shutdown();
	bool Render(WCHAR *wFileName);
	bool Play();
	bool Stop();
	bool Pause(); 
    bool Playing();// Playback status (return true if still playing)
	bool Esc();
	bool Fast();
	bool Slow();
	bool Normal();
	int GetCurrentPos();
	int GetCurrentTime();
	double GetCurrentSpeed();
	int GetDuration();
	void PutCurrentPos(double dPos);
	bool PutVolume(long Percent);
	long GetVolume();
	void SetFullScreen();
};

#pragma pack(1) 
typedef struct sWaveHeader
{
	char  RiffSig[4];
    long  WaveformChunkSize;
    char  WaveSig[4];
    char  FormatSig[4];
    long  FormatChunkSize;
    short FormatTag;
    short Channels;
    long  SampleRate;
    long  BytesPerSec;
    short BlockAlign;
    short BitsPerSample;
    char  DataSig[4];
    long  DataSize;
} sWaveHeader;
#pragma pack() 

