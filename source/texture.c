#include <stdlib.h>
#include <stdio.h>
#include "texture.h"
#include "stddef.h"
#include "sys/types.h"
#include "utils.h"
#include <assert.h>
#include "lzss.h"
 
void LoadTextureCMP(char *filename) {
	u_long b, i;
	u_long length;
	u_char *bytes;
	u_short numtextures;
	void *timsbaseaddr;

	// file read
	bytes = (u_char*) FileRead(filename, &length);
	if(bytes == NULL) {
		printf("Error reading %s from the CD.\n", filename);
		return; // do not free, as bytes is NULL!
	}

	// number of tims

	b = 0;
	numtextures = (u_short)GetLongLE(bytes, &b);
	printf("Number of textures %d\n", numtextures);

	// timsizes

	u_long totaltimsize = 0;
	u_long timoffsets[numtextures];

	for(int i=0; i<numtextures; i++) {
		u_long timsize;
		timoffsets[i] = totaltimsize;
		timsize = GetLongLE(bytes, &b);
		printf("TIM %d size (uncompressed) %d\n", i, timsize);
		totaltimsize += timsize;
	}

	timsbaseaddr = (char*) malloc(totaltimsize);

	for(i=0;i<numtextures;i++) {
		timoffsets[i] += (u_long)timsbaseaddr;
	}

	// tim compressed data
	ExpandLZSSData(&bytes[b], timsbaseaddr);
	// timsbaseaddr is now loaded with uncompressed texture data

	// copy texture to vram

	free3(bytes);	

	for(i=0;i<numtextures;i++) {
		UploadTextureToVRAM(timoffsets[i]);
	}
}

void UploadTextureToVRAM(long timptr) {
	
}