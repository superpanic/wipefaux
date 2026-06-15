#include "sound.h"
#include "libspu.h"

#define SOUND_MALLOC_MAX (10)

static SpuCommonAttr spucommonattr;
static SpuVoiceAttr spuvoiceattr;

static u_long vagspuaddr;
static char spumallocrec[SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1)];

void SoundInit(void) {
	SpuInit();
	// allocate the sound memory
	// configure spu settings
	SpuInitMalloc(SOUND_MALLOC_MAX, spumallocrec);
	spucommonattr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX);

	spucommonattr.mvol.left  = 0x3FFF;
	spucommonattr.mvol.right = 0x3FFF;

	spucommonattr.cd.volume.left = 0x7FFF;
	spucommonattr.cd.volume.right = 0x7FFF;

	spucommonattr.cd.mix = SPU_ON;

	SpuSetCommonAttr(&spucommonattr);

}

u_char *LoadVAGSound(char *filename, u_long *length) {
	u_long i;
	u_char *bytes;

	bytes = (u_char*) FileRead(filename, length);

	if(bytes == NULL) {
		printf("Error reading %s from the CD.\n", filename);
		return bytes;
	}

	return bytes;
}

void TransferVAGToSPU(u_char *data, u_long length, int channel) {
	SpuSetTransferMode(SpuTransByDMA);
	vagspuaddr = SpuMalloc(length);
	SpuSetTransStartAddr(vagspuaddr);
	SpuWrite(data, length);
	SpuIsTransferCompleted(SPU_TRANSFER_WAIT);
	spuvoiceattr.mask = (
		SPU_VOICE_VOLR | 
		SPU_VOICE_VOLL |
		SPU_VOICE_PITCH |
		SPU_VOICE_WDSA |
		SPU_VOICE_ADSR_AMODE |
		SPU_VOICE_ADSR_SMODE |
		SPU_VOICE_ADSR_RMODE |
		SPU_VOICE_ADSR_AR |
		SPU_VOICE_ADSR_DR |
		SPU_VOICE_ADSR_SR |
		SPU_VOICE_ADSR_RR |
		SPU_VOICE_ADSR_SL
	);
	spuvoiceattr.voice = channel;
	spuvoiceattr.volume.left = 0x1FFF;
	spuvoiceattr.volume.right = 0x1FFF;
	spuvoiceattr.pitch = 0x1000;
	spuvoiceattr.addr = vagspuaddr;
	spuvoiceattr.a_mode = SPU_VOICE_LINEARIncN; // attack curve
	spuvoiceattr.s_mode = SPU_VOICE_LINEARIncN; // sustain curve
	spuvoiceattr.r_mode = SPU_VOICE_LINEARIncN; // release curve
	spuvoiceattr.ar = 0x00;
	spuvoiceattr.dr = 0x00;
	spuvoiceattr.sr = 0x00;
	spuvoiceattr.rr = 0x00;
	spuvoiceattr.sl = 0x00;
	SpuSetVoiceAttr(&spuvoiceattr);
}

void AudioPlay(int channel) {
	SpuSetKey(SpuOn, channel);
}

void AudioFree(unsigned long address) {
	SpuFree(address);
}