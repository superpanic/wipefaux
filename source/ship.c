#include "ship.h"
#include <stdio.h>

void ShipInit(Ship *ship, Track *track, VECTOR *startpos) {
	ship->object->position.vx = startpos->vx;
	ship->object->position.vy = startpos->vy;
	ship->object->position.vz = startpos->vz;

	ship->vel = (VECTOR) {0,0,0};
	ship->acc = (VECTOR) {0,0,0};

	ship->thrust = (VECTOR) {0,0,0};
	ship->drag   = (VECTOR) {0,0,0};

	ship->yaw   = 0;
	ship->pitch = 0;
	ship->roll  = 0;

	ship->accyaw   = 0;
	ship->accpitch = 0;
	ship->accroll  = 0;

	ship->speed     = 0;
	ship->thrustmag = 0;
	ship->thrustmax = 700;

	ship->mass = 150;
}

void ShipUpdate(Ship *ship) {
	VECTOR force;
	
	ship->forward = (VECTOR) {0,0,ONE};
	ship->thrust.vx = (ship->thrustmag * ship->forward.vx);
	ship->thrust.vy = (ship->thrustmag * ship->forward.vy);
	ship->thrust.vz = (ship->thrustmag * ship->forward.vz);

	force = (VECTOR){0,0,0};
	force.vx = ship->thrust.vx;
	force.vy = ship->thrust.vy;
	force.vz = ship->thrust.vz;

	// a = F/m
	ship->acc.vx += force.vx / ship->mass;
	ship->acc.vy += force.vy / ship->mass;
	ship->acc.vz += force.vz / ship->mass;

	// update velocity
	ship->vel.vx += ship->acc.vx;
	ship->vel.vy += ship->acc.vy;
	ship->vel.vz += ship->acc.vz;

	// update position
	ship->object->position.vx += ship->vel.vx;
	ship->object->position.vy += ship->vel.vy;
	ship->object->position.vz += ship->vel.vz;
}
