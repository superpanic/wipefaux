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
#include "track.h"
#include "ship.h"

extern char __heap_start, __sp;

Camera camera;
Object *ships;
//Object *current_ship;
u_short ship_index;

Ship ship;

Track track;

Object *scene_objects;

u_short angle = 0;

void HeapSize(int size) {
	InitHeap3((unsigned long *)(&__heap_start), (&__sp - size) - &__heap_start);
}

void Setup(void) {
	u_short shipstarttexture;
	u_short scenestarttexture;
	u_short trackstarttexture;
	VECTOR startpos;

	HeapSize(0x5000); // 20480
	ScreenInit();
	CdInit();
	JoyPadInit();

	ResetNextPrim(GetCurrentBuffer());

	shipstarttexture = GetTextureCount();
	LoadTextureCMP("\\ALLSH.CMP;1", NULL); // all textures for all ships

	scenestarttexture = GetTextureCount();
	LoadTextureCMP("\\TRACK02\\SCENE.CMP;1", NULL); // all textures for the scene
	
	// load textures and tile information from the CD-rom
	trackstarttexture = GetTextureCount();
	LoadTextureCMP("\\TRACK02\\LIBRARY.CMP;1", "\\TRACK02\\LIBRARY.TTF;1");

	// load track vertices, faces and sections
	LoadTrackVertices( &track, "\\TRACK02\\TRACK.TRV;1" ); // .TRV file
	LoadTrackFaces( &track, "\\TRACK02\\TRACK.TRF;1", trackstarttexture);    // .TRF file
	LoadTrackSections( &track, "\\TRACK02\\TRACK.TRS;1" ); // .TRS file

	printf("NUM TRACK VERTICES: %d\n", track.numvertices);
	printf("NUM TRACK FACES: %d\n", track.numfaces);
	printf("NUM TRACK SECTIONS: %d\n", track.numsections);

	ships = (Object *) malloc3(sizeof(Object));
	u_char n_ships = LoadObjectsPRM(ships, "\\ALLSH.PRM;1", shipstarttexture);
	if(DEBUG) PrintObjectNames(ships, n_ships);
	ship.object = GetObjectByIndex(ships,1);

	setVector(&startpos, 32599, -347, -45310);
	
	ShipInit(&ship, &track, &startpos);

	setVector(&camera.position, ship.object->position.vx, ship.object->position.vy-200, ship.object->position.vz-800);
	camera.lookat = (MATRIX){0};
	camera.rotmat = (MATRIX){0};
}

void NextObject() {
	if(ship.object->next)
		ship.object = ship.object->next;
}

void PrevObject() {
	if(ship.object->prev)
		ship.object = ship.object->prev;
}

void PrintPrev() {
	printf("previous: %s\n", ship.object->prev->name);
}

void Update(void) {
	EmptyOT(GetCurrentBuffer());

	JoyPadUpdate();

	if(JoyPadCheck(PAD1_LEFT)) {
		ship.yaw -= 50;
	}

	if(JoyPadCheck(PAD1_RIGHT)) {
		ship.yaw += 50;
	}

	if(JoyPadCheck(PAD1_UP)) {
		ship.pitch -= 10;
	}

	if(JoyPadCheck(PAD1_DOWN)) {
		ship.pitch += 10;
	}

	if(JoyPadCheck(PAD1_CROSS)) {
		ship.thrustmag += 10;
	} else if(ship.thrustmag > 0) {
		ship.thrustmag -= 100;
	}
	if(ship.thrustmag > ship.thrustmax) ship.thrustmag = ship.thrustmax;

	/* select ship
		if(JoyLeftDown()) { PrevObject(); }
		if(JoyRightDown()) { NextObject(); }
		if(JoyDownDown()) { PrintPrev(); }
	*/

	// rotate ship
	//angle += 2;
	//current_ship->rotation.vy = angle;

	ShipUpdate(&ship);

	camera.position.vx = ship.object->position.vx;
	camera.position.vy = ship.object->position.vy-500;
	camera.position.vz = ship.object->position.vz-800;

	LookAt(
		&camera, 
		&camera.position, 
		&ship.object->position, 
		&(VECTOR){ 0, -ONE, 0 }
	);

	//RenderSceneObjects(scene_objects, &camera);
	RenderTrack(&track, &camera);
	RenderObject(ship.object, &camera);
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
