#include <stdlib.h>
#include <sys/types.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include "inline_n.h"
#include "globals.h"
#include "display.h"
#include "joypad.h"
#include "camera.h"
#include "object.h"
#include "utils.h"
#include "libcd.h"
#include <stdbool.h>

extern char __heap_start, __sp;

Camera camera;
Object object;

void HeapSize(int size) {
	InitHeap3((unsigned long *)(&__heap_start), (&__sp - size) - &__heap_start);
}

void Setup(void) {
	HeapSize(0x5000); // 20480
	ScreenInit();
	CdInit();
	JoyPadInit();

	ResetNextPrim(GetCurrentBuffer());

	setVector(&camera.position, 500, -1000, -1200);
	camera.lookat = (MATRIX){0};

	Object obj = {0};
	LoadObjectPRM(&obj, "\\ALLSH.PRM;1");
}

void Update(void) {
	EmptyOT(GetCurrentBuffer());

	JoyPadUpdate();

	if(JoyPadCheck(PAD1_LEFT)) {}
	if(JoyPadCheck(PAD1_RIGHT)) {}
	if(JoyPadCheck(PAD1_UP)) {}
	if(JoyPadCheck(PAD1_DOWN)) {}
	if(JoyPadCheck(PAD1_CROSS)) {}
	if(JoyPadCheck(PAD1_CIRCLE)) {}
}

void Render(void) {
    DisplayFrame();
}

int main(void) {
	Setup();
	while (true) {
		Update();
		Render();
	}
	return 0;
}
