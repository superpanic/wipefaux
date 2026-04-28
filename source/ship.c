#include "ship.h"
#include <stdio.h>
#include <libgte.h>

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

	// rsin, rcos are buggy in nugget
	// use (slower) csin ccos instead
	// FOUND FIXED libgte, seems to work with rsin rcos
	short sinx = rsin(ship->pitch);
	short cosx = rcos(ship->pitch);
	short siny = rsin(ship->yaw);
	short cosy = rcos(ship->yaw);
	short sinz = rsin(ship->roll);
	short cosz = rcos(ship->roll);

	// compute the right, up and forward vectors
	ship->right.vx = ((cosy * cosz) >> 12) + ((((siny * sinx) >> 12) * sinz) >> 12);
	ship->right.vy = (cosx * sinz) >> 12;
	ship->right.vz = ((-siny * cosz) >> 12) + ((((cosy * sinx) >> 12) * sinz) >> 12);

	ship->up.vx = ((-sinz * cosy) >> 12) + ((((siny * sinx) >> 12) * cosz) >> 12);
	ship->up.vy = (cosx * cosz) >> 12;
	ship->up.vz = ((-siny * -sinz) >> 12) + ((((cosy * sinx) >> 12) * cosz) >> 12);

	ship->forward.vx = (siny * cosx) >> 12;
	ship->forward.vy = (-sinx);
	ship->forward.vz = (cosy * cosx) >> 12;

	ship->thrust.vx = (ship->thrustmag * ship->forward.vx) >> 12;
	ship->thrust.vy = (ship->thrustmag * ship->forward.vy) >> 12;
	ship->thrust.vz = (ship->thrustmag * ship->forward.vz) >> 12;

	force = (VECTOR){0,0,0};
	// TODO: subtract drag
	force.vx += ship->thrust.vx;
	force.vy += ship->thrust.vy;
	force.vz += ship->thrust.vz;

	// a = F/m
	ship->acc.vx += force.vx / ship->mass;
	ship->acc.vy += force.vy / ship->mass;
	ship->acc.vz += force.vz / ship->mass;

	// update velocity
	ship->vel.vx += ship->acc.vx;
	ship->vel.vy += ship->acc.vy;
	ship->vel.vz += ship->acc.vz;

	// update position
	ship->object->position.vx += ship->vel.vx >> 6;
	ship->object->position.vy += ship->vel.vy >> 6;
	ship->object->position.vz += ship->vel.vz >> 6;

	ship->object->rotmat.m[0][0] = ship->right.vx;
	ship->object->rotmat.m[1][0] = ship->right.vy;
	ship->object->rotmat.m[2][0] = ship->right.vz;

	ship->object->rotmat.m[0][1] = ship->up.vx;
	ship->object->rotmat.m[1][1] = ship->up.vy;
	ship->object->rotmat.m[2][1] = ship->up.vz;

	ship->object->rotmat.m[0][2] = ship->forward.vx;
	ship->object->rotmat.m[1][2] = ship->forward.vy;
	ship->object->rotmat.m[2][2] = ship->forward.vz;

}

