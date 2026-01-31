#include <stdlib.h>
#include <stdio.h>
#include "texture.h"
#include "stddef.h"
#include "sys/types.h"
#include "utils.h"
#include <assert.h>

void LoadTextureCMP(char *filename) {
	u_long b, i;
	u_long length;
	u_char *bytes;
	u_short numtextures;
	void *timsbaseaddr;
	const u_short MAX_TEXTURES = 100;
	u_long timoffsets[MAX_TEXTURES];

	// file read
	bytes = (u_char*) FileRead(filename, &length);
	if(bytes == NULL) {
		printf("Error reading %s from the CD.\n", filename);
		goto exit;
	}

	// number of tims

	b = 0;
	numtextures = (u_short)GetLongLE(bytes, &b);
	printf("Number of textures %d\n", numtextures);

	// timsizes

	u_long totaltimsize = 0;

	assert(numtextures <= MAX_TEXTURES);
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

	// copy texture to vram

exit:
	free3(bytes);
	return;
}