#include "sound.h"
#include "libspu.h"
#include <stdbool.h>

#define SOUND_MALLOC_MAX (10)

static SpuCommonAttr spucommonattr;
static SpuVoiceAttr spuvoiceattr;

static u_long vagspuaddr;
static char spumallocrec[SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1)];

#define SOUNDQ_MAX (4)
static Snd soundq[SOUNDQ_MAX];
static char soundq_index = 0;
static u_long sound_total_data_size = 0;
static u_long current_sound_data = NULL;

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
	u_long sampling_rate;
	SpuSetTransferMode(SpuTransByDMA);
	vagspuaddr = SpuMalloc(length);
	SpuSetTransStartAddr(vagspuaddr);
	SpuWrite(data, length);
	SpuIsTransferCompleted(SPU_TRANSFER_WAIT);
	sampling_rate = VAGSamplingRate(data);
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
	spuvoiceattr.pitch = (sampling_rate<<12)/44100; // adjust pitch to sampling rate using this formula
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

void AudioUpdate() {
	bool sound_playing;
	sound_playing = (SpuGetKeyStatus(SPU_00CH) == SPU_ON);
	if(!sound_playing && soundq_index >= 0) {
		//sfx = LoadVAGSound("\\ROCKET.VAG;1", &sfx_len);
		// QSound(sfx,sfx_len,SPU_0CH);
		// transfer VAG data to the SPU and play in one of the channels
		if(current_sound_data != NULL) {
			printf("free audio at memory address: %lu\n", current_sound_data);
			AudioFree(current_sound_data);
		}
		current_sound_data = (u_long)soundq[soundq_index].data;
		TransferVAGToSPU(soundq[soundq_index].data, soundq[soundq_index].length, soundq[soundq_index].channel);
		AudioPlay(SPU_0CH);
		soundq_index--;
	}
}

void QSoundName(char *name) {
	if( soundq_index < SOUNDQ_MAX ) {
		soundq[soundq_index].data = LoadVAGSound(name, &soundq[soundq_index].length);
		sound_total_data_size += soundq[soundq_index].length;
		soundq[soundq_index].channel = SPU_0CH;
		soundq[soundq_index].sample_rate = VAGSamplingRate(soundq[soundq_index].data);
		if(soundq_index < SOUNDQ_MAX-1) soundq_index++;
	}
	printf("sound data size: %lu\n", sound_total_data_size);
}

u_long VAGSamplingRate(u_char *data) {
	u_long head = 16;
	long sampling_rate; 
	sampling_rate = GetLongBE(data, &head);
	printf("sampling rate: %lu\n", sampling_rate);
	return sampling_rate;
}

void AudioPlay(int channel) {
	SpuSetKey(SpuOn, channel);
}

void AudioFree(unsigned long address) {
	SpuFree(address);
}