#include "ship.h"
#include <stdio.h>

void ShipInit(Ship *ship, Track *track, VECTOR *startpos) {
	ship->object->position.vx = startpos->vx;
	ship->object->position.vx = startpos->vy;
	ship->object->position.vx = startpos->vz;

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
	printf("Updating the ship frame per frame! --> THRUST MAG: %d\n", ship->thrustmag);
	// 1. find the orientation matrix and update orientation
	// 2. compute forward vel (speed * forward vector)
	// 3. compute the thrust force vector (thrustmag * forward vector)
	// 4. find the total force being applied on the ship
	// 5. compute the acceleration
	// 6. compute the new velocity based on acceleration
	// 7. compute the new position based on velocity
}
