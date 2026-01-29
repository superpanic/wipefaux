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
#include "texture.h"
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

	setVector(&camera.position, 0, -600, -900);
	camera.lookat = (MATRIX){0};

	LoadTextureCMP("\\ALLSH.CMP;1");
	LoadObjectPRM(&object, "\\ALLSH.PRM;1");
}

void Update(void) {
	EmptyOT(GetCurrentBuffer());

	JoyPadUpdate();

	if(JoyPadCheck(PAD1_LEFT)) { object.rotation.vy -= 15; }
	if(JoyPadCheck(PAD1_RIGHT)) { object.rotation.vy += 15; }

	LookAt(
		&camera, 
		&camera.position, 
		&object.position, 
		&(VECTOR){ 0, -ONE, 0 }
	);

	RenderObject(&object, &camera);
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
