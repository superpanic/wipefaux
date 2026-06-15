#ifndef SOUND_H
#define SOUND_H

#include "globals.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <libspu.h>

void SoundInit(void);
u_char *LoadVAGSound(char *filename, u_long *length);
void TransferVAGToSPU(u_char *data, u_long length, int channel);

void AudioPlay(int channel);
void AudioFree(unsigned long address);

#endif
