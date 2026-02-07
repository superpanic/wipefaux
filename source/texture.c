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

	printf("total tim size: %lu \n", totaltimsize);

	timsbaseaddr = (char*) malloc3(totaltimsize);

	for(i=0;i<numtextures;i++) {
		timoffsets[i] += (u_long)timsbaseaddr;
	}

	// tim compressed data
	ExpandLZSSData(&bytes[b], timsbaseaddr);
	// timsbaseaddr is now loaded with uncompressed texture data

	// copy texture to vram

	free3(bytes);	

	for(i=0;i<numtextures;i++) {
		printf("texture: %d: ", i);
		UploadTextureToVRAM(timoffsets[i]);
	}
}

// returns malloced texture
Texture *UploadTextureToVRAM(long timptr) {
	Tim *tim;
	Texture *texture;
	u_short x, y;
	RECT rect;
	tim = (Tim*) timptr; // cast to this smaller type, just to perform a type test.

	switch(ClutType(tim)) {
		case CLUT_4BIT: {
			TimClut4 *tc4;
			tc4 = (TimClut4*) tim; // now cast again, but to full struct
			texture = (Texture*)malloc3(sizeof(Texture));

			texture->type = CLUT4;
			texture->textureX = tc4->textureX;
			texture->textureY = tc4->textureY;
			texture->textureW = tc4->textureW;
			texture->textureH = tc4->textureH;
			texture->clutX    = tc4->clutX;
			texture->clutY    = tc4->clutY;
			texture->clutW    = tc4->clutW;
			texture->clutH    = tc4->clutH;

			x = tc4->textureX - TextureHOffset(tc4->textureX);
			y = tc4->textureY - TextureVOffset(tc4->textureY);

			// not real UV-coordinates, 
			// just a quick flat full texture on each primitive
			texture->u0 = (x << 2);
			texture->v0 = (y);
			texture->u1 = ((x + tc4->textureW) << 2) - 1;
			texture->v1 = (y);
			texture->u2 = (x << 2);
			texture->v2 = (y + tc4->textureH) - 1;
			texture->u3 = ((x + tc4->textureW) << 2) - 1;
			texture->v3 = (y + tc4->textureH) - 1;

			texture->tpage = TPAGE(CLUT_4BIT, TRANSLUCENT, texture->textureX, texture->textureY);
			texture->clut  = CLUT(texture->clutX >> 4, texture->clutY);

			// load the clut rect to VRAM
			rect.x = tc4->clutX;
			rect.y = tc4->clutY;
			rect.w = tc4->clutW;
			rect.h = tc4->clutH;
			LoadImage(&rect, (u_long*)( &tc4->clut )); // pointer to clut
			DrawSync(0);

			rect.x = tc4->textureX;
			rect.y = tc4->textureY;
			rect.w = tc4->textureW;
			rect.h = tc4->textureH;
			int lm;
			lm = LoadImage(&rect, (u_long*)(tc4 + 1)); 
			// +1 jumps sizeof(TimClut4), to the position 
			// right after the clut, where the texture data starts;
			DrawSync(0);

			printf("clut4 loaded, result: %d\n", lm);

			break;
		}
		case CLUT_8BIT: {
			TimClut8 *tc8;
			tc8 = (TimClut8*) tim;
			texture = (Texture*) malloc3(sizeof(Texture));
			texture->type = CLUT8;
			texture->textureX = tc8->textureX;
			texture->textureY = tc8->textureY;
			texture->textureW = tc8->textureW;
			texture->textureH = tc8->textureH;
			texture->clutX    = tc8->clutX;
			texture->clutY    = tc8->clutY;
			texture->clutW    = tc8->clutW;
			texture->clutH    = tc8->clutH;

			x = tc8->textureX - TextureHOffset(tc8->textureX);
			y = tc8->textureY - TextureVOffset(tc8->textureY);

			// not real UV-coordinates, 
			// just a quick flat full texture on each primitive
			texture->u0 = (x << 1);
			texture->v0 = (y);
			texture->u1 = ((x + tc8->textureW) << 1) - 1;
			texture->v1 = (y);
			texture->u2 = (x << 1);
			texture->v2 = (y + tc8->textureH) - 1;
			texture->u3 = ((x + tc8->textureW) << 1) - 1;
			texture->v3 = (y + tc8->textureH) - 1;

			texture->tpage = TPAGE(CLUT_8BIT, TRANSLUCENT, texture->textureX, texture->textureY);
			texture->clut  = CLUT(texture->clutX >> 4, texture->clutY);

			// Load the CLUT rectangle to VRAM
			rect.x = tc8->clutX;
			rect.y = tc8->clutY;
			rect.w = tc8->clutW;
			rect.h = tc8->clutH;
			LoadImage(&rect, (u_long*)(&tc8->clut));
			DrawSync(0);

			// Load the Texture rectangle to VRAM
			rect.x = tc8->textureX;
			rect.y = tc8->textureY;
			rect.w = tc8->textureW;
			rect.h = tc8->textureH;
			LoadImage(&rect, (u_long*)(tc8 + 1));
			DrawSync(0);
			printf("clut8 loaded\n");
			break;
		}
		
	}
	return texture; // malloced!
}