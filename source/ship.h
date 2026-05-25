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

	short yaw, pitch;
	
	short roll;
	short roll_stabilizer;
	short roll_max;

	short yawpower;
	short velyaw; //, velpitch, velroll;
	//short accvel, accpitch, accroll;

	short mass;
	long speed;
	long thrustmag;
	long thrustmax;

	Section *section; // nearest track section
} Ship;

void ShipInit(Ship *ship, Track *track, VECTOR *startpos);
void UpdateSection(Ship *ship);
void ShipUpdate(Ship *ship);
void DrawXYZAxis(Ship *ship, Camera *camera);
void DrawXYZAxis2(Ship *ship, Camera *camera);


#endif
