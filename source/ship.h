#ifndef SHIP_H
#define SHIP_H

#include "globals.h"
#include "object.h"
#include "track.h"

typedef struct Ship {
	Object *object;

	VECTOR right, up, forward;

	VECTOR vel;
	VECTOR acc;

	VECTOR thrust;
	VECTOR drag;

	short yaw, pitch, roll;

	short velyaw, velpitch, velroll;
	short accyaw, accpitch, accroll;

	short mass;
	short speed;
	short thrustmag;
	short thrustmax;
} Ship;

void ShipInit(Ship *ship, Track *track, VECTOR *startpos);
void ShipUpdate(Ship *ship);

#endif
