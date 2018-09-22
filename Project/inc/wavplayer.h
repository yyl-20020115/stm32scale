#ifndef __WAVE_PLAYER_H
#define __WAVE_PLAYER_H

#include "stm32f4xx.h"

typedef struct
{
  u32  chunksize;
  u16  FormatTag;
  u16  Channels;
  u32  SampleRate;
  u32  ByteRate;
  u16  BlockAlign;
  u16  BitCount;
  u32  DataSize;
}wavdef;//wavÕ∑–≈œ¢


#define  CHUNK_ID                            0x52494646//'RIFF'
#define  FILE_FORMAT                         0x57415645//'WAVE'
#define  FORMAT_ID                           0x666D7420//'fmt '
#define  DATA_ID                             0x64617461//'data'
#define  FACT_ID                             0x66616374//'fact'
#define  WAVE_FORMAT_PCM                     0x01
#define  FORMAT_CHNUK_SIZE                   0x10
#define  CHANNEL_MONO                        0x01
#define  CHANNEL_STEREO                      0x02
#define  SAMPLE_RATE_8000                    8000
#define  SAMPLE_RATE_11025                   11025
#define  SAMPLE_RATE_22050                   22050
#define  SAMPLE_RATE_44100                   44100



u8  Wavplay(u8* fname);
u8  WaveParsing(u8* buf);
void Stop_play(void);
u32 ReadUnit(u8 *buf,u8 idx,u8 Num,u8 Endian);
void TX_callback(void);


#endif 
