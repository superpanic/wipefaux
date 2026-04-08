#include "utils.h"
#include "globals.h"
#include "libgte.h"
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <libcd.h>
#include <memory.h>

// ====================== RAM FILE TABLE ======================
typedef struct {
    const char* filename;   // string to match against
    u_long      address;    // where the data lives in RAM
    u_long      size;       // exact original size (not padded)
} RamFile;

RamFile ramFiles[] = {
    { "\\MENU.DAT;1",  0x80200000, 123456 },   // replace with real names & sizes
    { "\\LEVEL1.DAT;1",0x80300000, 987654 },
    { "\\TEXTURES.DAT;1", 0x80400000, 234567 },
    // add all files you need here
    { NULL, 0, 0 }  // terminator
};

char *FileRead(char *filename, u_long *length) {
	if(CDROM) return FileReadCD(filename, length);
	else return FileReadRAM(filename, length);
}

// returns a malloc buffer, use free! (keeps same API as before)
char *FileReadRAM(char *filename, u_long *length) {
    char *buffer = NULL;
    *length = 0;

    // Find the file in our RAM table
    for (int i = 0; ramFiles[i].filename != NULL; i++) {
        if (strcmp(ramFiles[i].filename, filename) == 0) {
            u_long size = ramFiles[i].size;
            *length = size;

            // Allocate and copy from RAM
            buffer = (char*) malloc3((size + 2047) & ~2047);  // pad to sector if you want
            if (buffer) {
                memcpy(buffer, (void*)ramFiles[i].address, size);
                printf("Loaded %s from RAM (0x%08X, %lu bytes)\n", filename, ramFiles[i].address, size);
            } else {
                printf("Error allocating %lu bytes for %s!\n", size, filename);
            }
            return buffer;
        }
    }

    printf("%s file not found in RAM table.\n", filename);
    return NULL;
}

// returns a malloc buffer, use free!
char *FileReadCD(char *filename, u_long *length) {
	CdlFILE filepos;
	int numsectors;
	char *buffer;

	buffer = NULL;

	if (CdSearchFile(&filepos, filename)==NULL) {
		printf("%s file not found on the CD.\n", filename);
	} else {
		printf("Found %s on the CD.\n", filename);
		numsectors = (filepos.size + 2047) / 2048;
		buffer = (char*) malloc3(2048 * numsectors);
		if(!buffer) {
			printf("Error allocating %d sectors!\n", numsectors);
		}
		CdControl(CdlSetloc, (u_char*) &filepos.pos, 0);
		CdRead(numsectors, (u_long*) buffer, CdlModeSpeed);
		CdReadSync(0, 0);
	}
	*length = filepos.size;
	return buffer;
}

char GetChar(u_char *bytes, u_long *b) {
	return bytes[(*b)++];
}

short GetShortLE(u_char *bytes, u_long *b) {
	unsigned short value = 0;
	value = value | bytes[(*b)++] << 0;
	value = value | bytes[(*b)++] << 8;
	return (short)value;
}

short GetShortBE(u_char *bytes, u_long *b) {
	unsigned short value = 0;
	value = value | bytes[(*b)++] << 8;
	value = value | bytes[(*b)++] << 0;
	return (short)value;
}

long GetLongLE(u_char *bytes, u_long *b) {
	u_long value = 0;
	value |= bytes[(*b)++] <<  0;
	value |= bytes[(*b)++] <<  8;
	value |= bytes[(*b)++] << 16;
	value |= bytes[(*b)++] << 24;
	return (long) value;
}

long GetLongBE(u_char *bytes, u_long *b) {
	u_long value = 0;
	value |= bytes[(*b)++] << 24;
	value |= bytes[(*b)++] << 16;
	value |= bytes[(*b)++] <<  8;
	value |= bytes[(*b)++] <<  0;
	return (long) value;
}

float GetFloatLE(u_char *bytes, u_long *b) {
	u_long value = 0;
	value |= bytes[(*b)++] <<  0;
	value |= bytes[(*b)++] <<  8;
	value |= bytes[(*b)++] << 16;
	value |= bytes[(*b)++] << 24;
	return (float) value;
}