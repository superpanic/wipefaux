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
Object *ships;
Object *current_ship;

Object *scene_objects;


u_short angle = 0;

void HeapSize(int size) {
	InitHeap3((unsigned long *)(&__heap_start), (&__sp - size) - &__heap_start);
}

void Setup(void) {
	u_short ship_texture_counter;
	u_short scene_texture_counter;
	HeapSize(0x5000); // 20480
	ScreenInit();
	CdInit();
	JoyPadInit();

	ResetNextPrim(GetCurrentBuffer());

	setVector(&camera.position, 0, -600, -900);
	camera.lookat = (MATRIX){0};

	ship_texture_counter = GetTextureCount();
	LoadTextureCMP("\\ALLSH.CMP;1"); // all textures for all ships

	scene_texture_counter = GetTextureCount();
	LoadTextureCMP("\\TRACK02\\SCENE.CMP;1"); // all textures for the scene
	
	ships = (Object *) malloc3(sizeof(Object));
	u_char n_ships = LoadObjectsPRM(ships, "\\ALLSH.PRM;1", ship_texture_counter);
	PrintObjectNames(ships, n_ships);
	current_ship = ships;

	scene_objects = (Object *) malloc3(sizeof(Object));
	u_char n_scene_objects = LoadObjectsPRM(scene_objects, "\\TRACK02\\SCENE.PRM;1", scene_texture_counter);

}

void NextObject() {
	if(current_ship->next) {
		current_ship = current_ship->next;
	}
}

void PrevObject() {
	if(current_ship->prev) {
		current_ship = current_ship->prev;
	}
}

void PrintPrev() {
	printf("previous: %s\n", current_ship->prev->name);
}

void Update(void) {
	EmptyOT(GetCurrentBuffer());

	JoyPadUpdate();

	if(JoyLeftDown()) { PrevObject(); }
	if(JoyRightDown()) { NextObject(); }
	if(JoyDownDown()) { PrintPrev(); }

	angle += 2;
	current_ship->rotation.vy = angle;

	LookAt(
		&camera, 
		&camera.position, 
		&current_ship->position, 
		&(VECTOR){ 0, -ONE, 0 }
	);

	RenderObject(current_ship, &camera);
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
