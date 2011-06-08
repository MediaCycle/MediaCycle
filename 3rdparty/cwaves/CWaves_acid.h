// Waves.h: interface for the CwavesAcid class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CwavesAcid_H_
#define _CwavesAcid_H_

//sleep()
#include <iostream>
#ifdef __MINGW32__
#include <windows.h>
#define sleep(n) Sleep(1000 * n)
#endif

//#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MAX_NUM_WAVEID			1024

#define WORD	short int
#define DWORD	int
#define GUID	int

enum WAVEFILETYPE
{
	WF_EX  = 1,
	WF_EXT = 2
};

enum WAVERESULT
{
	WR_OK = 0,
	WR_INVALIDFILENAME					= - 1,
	WR_BADWAVEFILE						= - 2,
	WR_INVALIDPARAM						= - 3,
	WR_INVALIDWAVEID					= - 4,
	WR_NOTSUPPORTEDYET					= - 5,
	WR_WAVEMUSTBEMONO					= - 6,
	WR_WAVEMUSTBEWAVEFORMATPCM			= - 7,
	WR_WAVESMUSTHAVESAMEBITRESOLUTION	= - 8,
	WR_WAVESMUSTHAVESAMEFREQUENCY		= - 9,
	WR_WAVESMUSTHAVESAMEBITRATE			= -10,
	WR_WAVESMUSTHAVESAMEBLOCKALIGNMENT	= -11,
	WR_OFFSETOUTOFDATARANGE				= -12,
	WR_FILEERROR						= -13,
	WR_OUTOFMEMORY						= -14,
	WR_INVALIDSPEAKERPOS				= -15,
	WR_INVALIDWAVEFILETYPE				= -16,
	WR_NOTWAVEFORMATEXTENSIBLEFORMAT	= -17
};

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct tWAVEFORMATEX
{
    WORD    wFormatTag;
    WORD    nChannels;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBytesPerSec;
    WORD    nBlockAlign;
    WORD    wBitsPerSample;
    WORD    cbSize;
} WAVEFORMATEX;
#endif /* _WAVEFORMATEX_ */

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_
typedef struct {
    WAVEFORMATEX    Format;
    union {
        WORD wValidBitsPerSample;       /* bits of precision  */
        WORD wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
        WORD wReserved;                 /* If neither applies, set to zero. */
    } Samples;
    DWORD           dwChannelMask;      /* which channels are */
                                        /* present in stream  */
    GUID            SubFormat;
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;
#endif // !_WAVEFORMATEXTENSIBLE_

typedef struct
{
	WAVEFILETYPE	wfType;
	WAVEFORMATEXTENSIBLE wfEXT;		// For non-WAVEFORMATEXTENSIBLE wavefiles, the header is stored in the Format member of wfEXT
	char			*pData;
	unsigned int	ulDataSize;
	FILE			*pFile;
	unsigned int	ulDataOffset;
	unsigned short  acid_type;
	unsigned short	acid_nbeats;
	unsigned short	acid_key;
} WAVEFILEINFO, *LPWAVEFILEINFO;

//typedef int (__cdecl *PFNALGETENUMVALUE)( const char *szEnumName );
typedef int	WAVEID;

char *AddMediaPath(const char *filename);

class CwavesAcid  
{
public:
	CwavesAcid();
	virtual ~CwavesAcid();

	WAVERESULT LoadWaveFile(const char *szFilename, WAVEID *WaveID);
	WAVERESULT OpenWaveFile(const char *szFilename, WAVEID *WaveID);
	WAVERESULT ReadWaveData(WAVEID WaveID, void *pData, unsigned int ulDataSize, unsigned int *pulBytesWritten);
	WAVERESULT SetWaveDataOffset(WAVEID WaveID, unsigned int ulOffset);
	WAVERESULT GetWaveDataOffset(WAVEID WaveID, unsigned int *pulOffset);
	WAVERESULT GetWaveType(WAVEID WaveID, WAVEFILETYPE *pwfType);
	WAVERESULT GetWaveFormatExHeader(WAVEID WaveID, WAVEFORMATEX *pWFEX);
	WAVERESULT GetWaveFormatExtensibleHeader(WAVEID WaveID, WAVEFORMATEXTENSIBLE *pWFEXT);
	WAVERESULT GetWaveData(WAVEID WaveID, void **ppAudioData);
	WAVERESULT GetWaveSize(WAVEID WaveID, unsigned int *pulDataSize);
	WAVERESULT GetAcidKey(WAVEID WaveID, unsigned short *c);
	WAVERESULT GetAcidType(WAVEID WaveID, unsigned short *i);
	WAVERESULT GetAcidNbeats(WAVEID WaveID, unsigned short *i);
	WAVERESULT GetNchannels(WAVEID WaveID, int *i);
	WAVERESULT GetWaveFrequency(WAVEID WaveID, unsigned int *pulFrequency);
	//WAVERESULT GetWaveALBufferFormat(WAVEID WaveID, PFNALGETENUMVALUE pfnGetEnumValue, unsigned int *pulFormat);
	WAVERESULT DeleteWaveFile(WAVEID WaveID);

	char *GetErrorString(WAVERESULT wr, char *szErrorString, unsigned int nSizeOfErrorString);
	bool IsWaveID(WAVEID WaveID);

private:
	WAVERESULT ParseFile(const char *szFilename, LPWAVEFILEINFO pWaveInfo);
	WAVEID InsertWaveID(LPWAVEFILEINFO pWaveFileInfo);
	
	LPWAVEFILEINFO	m_WaveIDs[MAX_NUM_WAVEID];
};

#endif // _CwavesAcid_H_
