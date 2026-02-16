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
#include <stdio.h>

extern char __heap_start, __sp;

Camera camera;
Object object;
Object *current_object;

void HeapSize(int size) {
	InitHeap3((unsigned long *)(&__heap_start), (&__sp - size) - &__heap_start);
}

void Setup(void) {
	u_short texture_counter;
	HeapSize(0x5000); // 20480
	ScreenInit();
	CdInit();
	JoyPadInit();

	ResetNextPrim(GetCurrentBuffer());

	setVector(&camera.position, 0, -600, -900);
	camera.lookat = (MATRIX){0};

	texture_counter = GetTextureCount();
	LoadTextureCMP("\\ALLSH.CMP;1"); // all textures for all ships
	
	u_char n_ships = LoadObjectsPRM(&object, "\\ALLSH.PRM;1", texture_counter);
	PrintObjectNames(&object, n_ships);
	current_object = &object;
}

void NextObject() {
	if(current_object->next) {
		current_object = current_object->next;
		current_object->rotation.vy = 0;
	}
}

void PrevObject() {
	if(current_object->prev) {
		current_object = current_object->prev;
		current_object->rotation.vy = 0;
	}
}

void PrintPrev() {
	printf("previous: %s\n", current_object->prev->name);
}

void Update(void) {
	EmptyOT(GetCurrentBuffer());

	JoyPadUpdate();

	if(JoyLeftDown()) { PrevObject(); }
	if(JoyRightDown()) { NextObject(); }
	if(JoyDownDown()) { PrintPrev(); }

	current_object->rotation.vy += 2;

	LookAt(
		&camera, 
		&camera.position, 
		&current_object->position, 
		&(VECTOR){ 0, -ONE, 0 }
	);

	RenderObject(current_object, &camera);
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
