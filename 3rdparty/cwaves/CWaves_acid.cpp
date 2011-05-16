/*
 * Copyright (c) 2006, Creative Labs Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * 	     the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
 * 	     promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "CWaves_acid.h"
//#include <ks.h>
//#include <ksmedia.h>
//#include <iostream>
//using namespace std;


////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO SD - from mmsystem and ksmedia in windows API
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WAVE_FORMAT_PCM

/* OLD general waveform format structure (information common to all formats) */
typedef struct waveformat_tag {
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo, etc.) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
} WAVEFORMAT; //, *PWAVEFORMAT, NEAR *NPWAVEFORMAT, FAR *LPWAVEFORMAT;

/* flags for wFormatTag field of WAVEFORMAT */
#define WAVE_FORMAT_PCM     1


/* specific waveform format structure for PCM data */
typedef struct pcmwaveformat_tag {
    WAVEFORMAT  wf;
    WORD        wBitsPerSample;
} PCMWAVEFORMAT; //, *PPCMWAVEFORMAT, NEAR *NPPCMWAVEFORMAT, FAR *LPPCMWAVEFORMAT;
#endif /* WAVE_FORMAT_PCM */


// in ksmedia.h
#if !defined(WAVE_FORMAT_EXTENSIBLE)
#define  WAVE_FORMAT_EXTENSIBLE                 0xFFFE
#endif // !defined(WAVE_FORMAT_EXTENSIBLE)
#pragma pack(push, 4)

#define SUCCEEDED(hr) ((int)(hr) >= 0)

////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct
{
	char			szRIFF[4];
	unsigned int	ulRIFFSize;
	char			szWAVE[4];
} WAVEFILEHEADER;

typedef struct
{
	char			szChunkName[4];
	unsigned int	ulChunkSize;
} RIFFCHUNK;


/*typedef struct
{
	unsigned int	acid_type;
	//unsigned short	unknown_1;
	char			acid_key;
	unsigned short  unknown_2;
	char			euro;
	unsigned int   unknown_3;
	unsigned short  acid_nbeats;
	unsigned short	unknown_3bis;
	unsigned short  unknown_4;		//0x0400
	unsigned short  unknown_5;		//0x0400
	unsigned short  unknown_6;		
	unsigned short  unknown_7;
} ACIDCHUNK;*/

typedef struct
{
 unsigned short values[12];
} ACIDCHUNK;


typedef struct
{
	unsigned short	usFormatTag;
	unsigned short	usChannels;
	unsigned int	ulSamplesPerSec;
	unsigned int	ulAvgBytesPerSec;
	unsigned short	usBlockAlign;
	unsigned short	usBitsPerSample;
	unsigned short	usSize;
	unsigned short  usReserved;
	unsigned int	ulChannelMask;
    GUID            guidSubFormat;
} WAVEFMT;

#pragma pack(pop)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CwavesAcid::CwavesAcid()
{
	memset(&m_WaveIDs, 0, sizeof(m_WaveIDs));
}

CwavesAcid::~CwavesAcid()
{
	int lLoop;

	for (lLoop = 0; lLoop < MAX_NUM_WAVEID; lLoop++)
	{
		if (m_WaveIDs[lLoop])
		{
			if (m_WaveIDs[lLoop]->pData)
				delete m_WaveIDs[lLoop]->pData;

			if (m_WaveIDs[lLoop]->pFile)
				fclose(m_WaveIDs[lLoop]->pFile);

			delete m_WaveIDs[lLoop];
			m_WaveIDs[lLoop] = 0;
		}
	}
}


WAVERESULT CwavesAcid::LoadWaveFile(const char *szFilename, WAVEID *pWaveID)
{
	WAVERESULT wr = WR_OUTOFMEMORY;
	LPWAVEFILEINFO pWaveInfo;

	pWaveInfo = new WAVEFILEINFO;
	if (pWaveInfo)
	{
		if (SUCCEEDED(wr = ParseFile(szFilename, pWaveInfo)))
		{
			// Allocate memory for sample data
			pWaveInfo->pData = new char[pWaveInfo->ulDataSize];
			if (pWaveInfo->pData)
			{
				// Seek to start of audio data
				fseek(pWaveInfo->pFile, pWaveInfo->ulDataOffset, SEEK_SET);

				// Read Sample Data
				if (fread(pWaveInfo->pData, 1, pWaveInfo->ulDataSize, pWaveInfo->pFile) == pWaveInfo->ulDataSize)
				{
					int lLoop = 0;
					for (lLoop = 0; lLoop < MAX_NUM_WAVEID; lLoop++)
					{
						if (!m_WaveIDs[lLoop])
						{
							m_WaveIDs[lLoop] = pWaveInfo;
							*pWaveID = lLoop;
							wr = WR_OK;
							break;
						}
					}

					if (lLoop == MAX_NUM_WAVEID)
					{
						delete pWaveInfo->pData;
						wr = WR_OUTOFMEMORY;
					}
				}
				else
				{
					delete pWaveInfo->pData;
					wr = WR_BADWAVEFILE;
				}
			}
			else
			{
				wr = WR_OUTOFMEMORY;
			}

			fclose(pWaveInfo->pFile);
			pWaveInfo->pFile = 0;
		}

		if (wr != WR_OK)
			delete pWaveInfo;
	}

	return wr;
}


WAVERESULT CwavesAcid::OpenWaveFile(const char *szFilename, WAVEID *pWaveID)
{
	WAVERESULT wr = WR_OUTOFMEMORY;
	LPWAVEFILEINFO pWaveInfo;

	pWaveInfo = new WAVEFILEINFO;
	if (pWaveInfo)
	{
		if (SUCCEEDED(wr = ParseFile(szFilename, pWaveInfo)))
		{
			int lLoop = 0;
			for (lLoop = 0; lLoop < MAX_NUM_WAVEID; lLoop++)
			{
				if (!m_WaveIDs[lLoop])
				{
					m_WaveIDs[lLoop] = pWaveInfo;
					*pWaveID = lLoop;
					wr = WR_OK;
					break;
				}
			}

			if (lLoop == MAX_NUM_WAVEID)
				wr = WR_OUTOFMEMORY;
		}

		if (wr != WR_OK)
			delete pWaveInfo;
	}

	return wr;
}

WAVERESULT CwavesAcid::ReadWaveData(WAVEID WaveID, void *pData, unsigned int ulDataSize, unsigned int *pulBytesWritten)
{
	LPWAVEFILEINFO pWaveInfo;
	WAVERESULT wr = WR_BADWAVEFILE;

	if (!pData || !pulBytesWritten || ulDataSize == 0)
		return WR_INVALIDPARAM;

	if (IsWaveID(WaveID))
	{
		pWaveInfo = m_WaveIDs[WaveID];
		if (pWaveInfo->pFile)
		{
			unsigned int ulOffset = ftell(pWaveInfo->pFile);

			if ((ulOffset - pWaveInfo->ulDataOffset + ulDataSize) > pWaveInfo->ulDataSize)
				ulDataSize = pWaveInfo->ulDataSize - (ulOffset - pWaveInfo->ulDataOffset);

			*pulBytesWritten = (unsigned int)fread(pData, 1, ulDataSize, pWaveInfo->pFile);

			wr = WR_OK;
		}
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT CwavesAcid::SetWaveDataOffset(WAVEID WaveID, unsigned int ulOffset)
{
	LPWAVEFILEINFO pWaveInfo;
	WAVERESULT wr = WR_INVALIDPARAM;

	if (IsWaveID(WaveID))
	{
		pWaveInfo = m_WaveIDs[WaveID];
		if (pWaveInfo->pFile)
		{
			// Seek into audio data
			fseek(pWaveInfo->pFile, pWaveInfo->ulDataOffset + ulOffset, SEEK_SET);
			wr = WR_OK;
		}
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT CwavesAcid::GetWaveDataOffset(WAVEID WaveID, unsigned int *pulOffset)
{
	LPWAVEFILEINFO pWaveInfo;
	WAVERESULT wr = WR_INVALIDPARAM;

	if (IsWaveID(WaveID))
	{
		pWaveInfo = m_WaveIDs[WaveID];
		if ((pWaveInfo->pFile) && (pulOffset))
		{
			// Get current position
			*pulOffset = ftell(pWaveInfo->pFile);
			*pulOffset -= pWaveInfo->ulDataOffset;
			wr = WR_OK;
		}
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT CwavesAcid::ParseFile(const char *szFilename, LPWAVEFILEINFO pWaveInfo)
{
	WAVEFILEHEADER	waveFileHeader;
	RIFFCHUNK		riffChunk;
	WAVEFMT			waveFmt;
	WAVERESULT		wr = WR_BADWAVEFILE;

	ACIDCHUNK		acidChunk;

	if (!szFilename || !pWaveInfo)
		return WR_INVALIDPARAM;

	memset(pWaveInfo, 0, sizeof(WAVEFILEINFO));
	pWaveInfo->acid_type=-1;

	// Open the wave file for reading
	pWaveInfo->pFile = fopen(szFilename, "rb");
	if (pWaveInfo->pFile)
	{
		// Read Wave file header
		fread(&waveFileHeader, 1, sizeof(WAVEFILEHEADER), pWaveInfo->pFile);
		//if (!_strnicmp(waveFileHeader.szRIFF, "RIFF", 4) && !_strnicmp(waveFileHeader.szWAVE, "WAVE", 4))
		//int tmp = strcmp(waveFileHeader.szRIFF, "RIFF");
		if (!strncmp(waveFileHeader.szRIFF, "RIFF", 4) && !strncmp(waveFileHeader.szWAVE, "WAVE", 4))
		{
			while (fread(&riffChunk, 1, sizeof(RIFFCHUNK), pWaveInfo->pFile) == sizeof(RIFFCHUNK))
			{
				//if (!_strnicmp(riffChunk.szChunkName, "fmt ", 4))
				if (!strncmp(riffChunk.szChunkName, "fmt ", 4))
				{
					if (riffChunk.ulChunkSize <= sizeof(WAVEFMT))
					{
						fread(&waveFmt, 1, riffChunk.ulChunkSize, pWaveInfo->pFile);
					
						// Determine if this is a WAVEFORMATEX or WAVEFORMATEXTENSIBLE wave file
						if (waveFmt.usFormatTag == WAVE_FORMAT_PCM)
						{
							pWaveInfo->wfType = WF_EX;
							memcpy(&pWaveInfo->wfEXT.Format, &waveFmt, sizeof(PCMWAVEFORMAT));
						}
						else if (waveFmt.usFormatTag == WAVE_FORMAT_EXTENSIBLE)
						{
							pWaveInfo->wfType = WF_EXT;
							memcpy(&pWaveInfo->wfEXT, &waveFmt, sizeof(WAVEFORMATEXTENSIBLE));
						}
					}
					else
					{
						fseek(pWaveInfo->pFile, riffChunk.ulChunkSize, SEEK_CUR);
					}
				}
				//else if (!_strnicmp(riffChunk.szChunkName, "data", 4))
				else if (!strncmp(riffChunk.szChunkName, "data", 4))
				{
					pWaveInfo->ulDataSize = riffChunk.ulChunkSize;
					pWaveInfo->ulDataOffset = ftell(pWaveInfo->pFile);
					fseek(pWaveInfo->pFile, riffChunk.ulChunkSize, SEEK_CUR);
				}
				//else if (!_strnicmp(riffChunk.szChunkName, "acid", 4))
				else if (!strncmp(riffChunk.szChunkName, "acid", 4))
				{
					if (riffChunk.ulChunkSize <= sizeof(ACIDCHUNK))
					{
						fread(&acidChunk, 1, riffChunk.ulChunkSize, pWaveInfo->pFile);
						memcpy(&pWaveInfo->acid_type, &acidChunk.values[0], sizeof(short));
						memcpy(&pWaveInfo->acid_key, &acidChunk.values[2], sizeof(short));
						memcpy(&pWaveInfo->acid_nbeats, &acidChunk.values[6], sizeof(short));


					
						// Determine if this is a WAVEFORMATEX or WAVEFORMATEXTENSIBLE wave file
					}
					else
					{
						fseek(pWaveInfo->pFile, riffChunk.ulChunkSize, SEEK_CUR);
					}
				}
				else
				{
					fseek(pWaveInfo->pFile, riffChunk.ulChunkSize, SEEK_CUR);
				}

				// Ensure that we are correctly aligned for next chunk
				if (riffChunk.ulChunkSize & 1)
					fseek(pWaveInfo->pFile, 1, SEEK_CUR);
			}

			if (pWaveInfo->ulDataSize && pWaveInfo->ulDataOffset && ((pWaveInfo->wfType == WF_EX) || (pWaveInfo->wfType == WF_EXT)))
				wr = WR_OK;
			else
				fclose(pWaveInfo->pFile);
		}
	}
	else
	{
		wr = WR_INVALIDFILENAME;
	}

	return wr;
}


WAVERESULT CwavesAcid::DeleteWaveFile(WAVEID WaveID)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (m_WaveIDs[WaveID]->pData)
			delete m_WaveIDs[WaveID]->pData;

		if (m_WaveIDs[WaveID]->pFile)
			fclose(m_WaveIDs[WaveID]->pFile);

		delete m_WaveIDs[WaveID];
		m_WaveIDs[WaveID] = 0;
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}


WAVERESULT CwavesAcid::GetWaveType(WAVEID WaveID, WAVEFILETYPE *wfType)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!wfType)
		return WR_INVALIDPARAM;

	*wfType = m_WaveIDs[WaveID]->wfType;

	return WR_OK;
}


WAVERESULT CwavesAcid::GetWaveFormatExHeader(WAVEID WaveID, WAVEFORMATEX *wfex)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!wfex)
		return WR_INVALIDPARAM;

	memcpy(wfex, &(m_WaveIDs[WaveID]->wfEXT.Format), sizeof(WAVEFORMATEX));

	return WR_OK;
}


WAVERESULT CwavesAcid::GetWaveFormatExtensibleHeader(WAVEID WaveID, WAVEFORMATEXTENSIBLE *wfext)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (m_WaveIDs[WaveID]->wfType != WF_EXT)
		return WR_NOTWAVEFORMATEXTENSIBLEFORMAT;

	if (!wfext)
		return WR_INVALIDPARAM;

	memcpy(wfext, &(m_WaveIDs[WaveID]->wfEXT), sizeof(WAVEFORMATEXTENSIBLE));

	return WR_OK;
}

WAVERESULT CwavesAcid::GetWaveData(WAVEID WaveID, void **lplpAudioData)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!lplpAudioData)
		return WR_INVALIDPARAM;

	*lplpAudioData = m_WaveIDs[WaveID]->pData;

	return WR_OK;
}

WAVERESULT CwavesAcid::GetWaveSize(WAVEID WaveID, unsigned int *size)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!size)
		return WR_INVALIDPARAM;

	*size = m_WaveIDs[WaveID]->ulDataSize;

	return WR_OK;
}


WAVERESULT CwavesAcid::GetWaveFrequency(WAVEID WaveID, unsigned int *pulFrequency)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (pulFrequency)
			*pulFrequency = m_WaveIDs[WaveID]->wfEXT.Format.nSamplesPerSec;
		else
			wr = WR_INVALIDPARAM;
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT CwavesAcid::GetAcidKey(WAVEID WaveID, unsigned short *c)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (c)
			*c = m_WaveIDs[WaveID]->acid_key;
		else
			wr = WR_INVALIDPARAM;
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT CwavesAcid::GetAcidType(WAVEID WaveID, unsigned short *i)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (i)
			*i = m_WaveIDs[WaveID]->acid_type;
		else
			wr = WR_INVALIDPARAM;
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT CwavesAcid::GetAcidNbeats(WAVEID WaveID, unsigned short *i)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (i)
			*i = m_WaveIDs[WaveID]->acid_nbeats;
		else
			wr = WR_INVALIDPARAM;
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT CwavesAcid::GetNchannels(WAVEID WaveID, int *i)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (i)
			*i = m_WaveIDs[WaveID]->wfEXT.Format.nChannels;
		else
			wr = WR_INVALIDPARAM;
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

/*
WAVERESULT CwavesAcid::GetWaveALBufferFormat(WAVEID WaveID, ALBuff pfnGetEnumValue, unsigned int *pulFormat)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (pfnGetEnumValue && pulFormat)
		{
			*pulFormat = 0;

			if (m_WaveIDs[WaveID]->wfType == WF_EX)
			{
				if (m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 1)
					*pulFormat = m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_MONO16") : pfnGetEnumValue("AL_FORMAT_MONO8");
				else if (m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 2)
					*pulFormat = m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_STEREO16") : pfnGetEnumValue("AL_FORMAT_STEREO8");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 4) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
			}
			else if (m_WaveIDs[WaveID]->wfType == WF_EXT)
			{
				if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 1) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == SPEAKER_FRONT_CENTER))
					*pulFormat = m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_MONO16") : pfnGetEnumValue("AL_FORMAT_MONO8");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 2) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT)))
					*pulFormat = m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_STEREO16") : pfnGetEnumValue("AL_FORMAT_STEREO8");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 2) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
					*pulFormat =  pfnGetEnumValue("AL_FORMAT_REAR16");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 4) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 6) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_51CHN16");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 7) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_BACK_CENTER)))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_61CHN16");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 8) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_SIDE_LEFT|SPEAKER_SIDE_RIGHT)))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_71CHN16");
			}

			if (*pulFormat == 0)
				wr = WR_INVALIDWAVEFILETYPE;
		}
		else
		{
			wr = WR_INVALIDPARAM;
		}
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}
*/

bool CwavesAcid::IsWaveID(WAVEID WaveID)
{
	bool bReturn = false;

	if ((WaveID >= 0) && (WaveID < MAX_NUM_WAVEID))
	{
		if (m_WaveIDs[WaveID])
			bReturn = true;
	}

	return bReturn;
}


char *CwavesAcid::GetErrorString(WAVERESULT wr, char *szErrorString, unsigned int nSizeOfErrorString)
{	
	switch (wr)
	{
		case WR_OK:
			strncpy(szErrorString, "Success\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDFILENAME:
			strncpy(szErrorString, "Invalid file name or file does not exist\n", nSizeOfErrorString-1);
			break;

		case WR_BADWAVEFILE:
			strncpy(szErrorString, "Invalid Wave file\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDPARAM:
			strncpy(szErrorString, "Invalid parameter passed to function\n", nSizeOfErrorString-1);
			break;

		case WR_FILEERROR:
			strncpy(szErrorString, "File I/O error\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDWAVEID:
			strncpy(szErrorString, "Invalid WAVEID\n", nSizeOfErrorString-1);
			break;

		case WR_NOTSUPPORTEDYET:
			strncpy(szErrorString, "Function not supported yet\n", nSizeOfErrorString-1);
			break;

		case WR_WAVEMUSTBEMONO:
			strncpy(szErrorString, "Input wave files must be mono\n", nSizeOfErrorString-1);
			break;

		case WR_WAVEMUSTBEWAVEFORMATPCM:
			strncpy(szErrorString, "Input wave files must be in Wave Format PCM\n", nSizeOfErrorString-1);
			break;
		
		case WR_WAVESMUSTHAVESAMEBITRESOLUTION:
			strncpy(szErrorString, "Input wave files must have the same Bit Resolution\n", nSizeOfErrorString-1);
			break;

		case WR_WAVESMUSTHAVESAMEFREQUENCY:
			strncpy(szErrorString, "Input wave files must have the same Frequency\n", nSizeOfErrorString-1);
			break;

		case WR_WAVESMUSTHAVESAMEBITRATE:
			strncpy(szErrorString, "Input wave files must have the same Bit Rate\n", nSizeOfErrorString-1);
			break;

		case WR_WAVESMUSTHAVESAMEBLOCKALIGNMENT:
			strncpy(szErrorString, "Input wave files must have the same Block Alignment\n", nSizeOfErrorString-1);
			break;

		case WR_OFFSETOUTOFDATARANGE:
			strncpy(szErrorString, "Wave files Offset is not within audio data\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDSPEAKERPOS:
			strncpy(szErrorString, "Invalid Speaker Destinations\n", nSizeOfErrorString-1);
			break;

		case WR_OUTOFMEMORY:
			strncpy(szErrorString, "Out of memory\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDWAVEFILETYPE:
			strncpy(szErrorString, "Invalid Wave File Type\n", nSizeOfErrorString-1);
			break;

		case WR_NOTWAVEFORMATEXTENSIBLEFORMAT:
			strncpy(szErrorString, "Wave file is not in WAVEFORMATEXTENSIBLE format\n", nSizeOfErrorString-1);
			break;

		default:
			strncpy(szErrorString, "Undefined error\n", nSizeOfErrorString-1);
	}
	szErrorString[nSizeOfErrorString-1] = '\0';
	return szErrorString;
}

char fullPath[260];
char *AddMediaPath(const char *filename)
{
	sprintf(fullPath, "%s%s", "C:\\Program Files\\TortoiseHg\\media\\", filename);
	return fullPath;
}