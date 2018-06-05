// avi.h
// MALLET Maxime

#ifndef _AVI_H
#define _AVI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

// taille des types
// int = 4 octets ==>DWORD ou FOURCC
// short = 2 octets ==> WORD ou RECT


typedef struct{
	int dwMicroSecPerFrame; // frame display rate (or 0)
	int dwMaxBytesPerSec; // max. transfer rate
	int dwPaddingGranularity; // pad to multiples of this
	// size;
	int dwFlags; // the ever-present flags, in hexa
	int dwTotalFrames; // Number of frames in file
	int dwInitialFrames; // Frame initiale, de départ
	int dwStreams; //Stream numbers
	int dwSuggestedBufferSize; // taille suggérée du buffer
	int dwWidth; // largeur de l'image
	int dwHeight; // hauteur de l'image
	int dwReserved[4];
} MainAVIHeader; //CHUNK HDRL

typedef struct {
	int fccType; // Type du stream
	int fccHandler; //Codec
	int dwFlags; // Dépend du type du stream
	short wPriority; // Priorité du stream e.g. si plusieurs fichiers audio
	short wLanguage; // language
	int dwInitialFrames; // Frame initiale, de départ
	int dwScale;
	int dwRate; /* dwRate / dwScale == samples/second */
	int dwStart;
	int dwLength; // In units above...
	int dwSuggestedBufferSize; // Taille suggérée du buffer
	int dwQuality; // Quality, from 0 to 1000
	int dwSampleSize; //
	short rcFrame;
} AVIStreamHeader;  // CHUNK STREAM HEADER

typedef struct {
	//Format de l'IMAGE
 	int biSize;
 	int biWidth;
 	int biHeight;
 	short biPlanes;
 	short biBitCount;
 	int biCompression;
 	int biSizeImage;
 	int biXPelsPerMeter;
 	int biYPelsPerMeter;
 	int biClrUsed;
 	int biClrImportant;
 	//RGBQUAD bmiColors[1];
} IMAGEAVI;

typedef struct {
	//Format du SON
 	short wFormatTag;
 	short nChannels;
 	int nSamplesPerSec;
 	int nAvgBytesPerSec;
 	short nBlockAlign;
 	short wBitsPerSample;
 	short cbSize;
} WAVEFORMAT;

typedef struct {
	int dwFourCC;
	int dwSize;
	//BYTE data[dwSize]; // contains headers or video/audio data
} CHUNK;

typedef struct{
	int dwList;
	int dwSize;
	int dwFourCC;
	//BYTE data[dwSize-4]; // contient les Listes et Chunk
} LIST;


#endif
