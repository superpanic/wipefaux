#include <stdlib.h>
#include "ship.h"
#include <stdio.h>
#include <libgte.h>
#include <libgpu.h>
#include "globals.h"
#include "display.h"
#include <inline_n.h>

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

void DrawXYZAxis(Ship *ship, Camera *camera) {
	SVECTOR v0, v1, v2;
	int i;
	long otz;
	POLY_F3 *polya, *polyb, *polyc, *polyd;
	LINE_F2 lineA, lineB, lineC;

	VECTOR vec;
	SVECTOR rot;
	MATRIX worldmat;
	MATRIX viewmat;

	rot.vx = 0;
	rot.vy = 0;
	rot.vz = 0;

	RotMatrix(&rot, &worldmat);
	TransMatrix(&worldmat, &ship->object->position);
	ScaleMatrix(&worldmat, &ship->object->scale);
	CompMatrixLV(&camera->lookat, &worldmat, &viewmat);
	SetRotMatrix(&viewmat);
	SetTransMatrix(&viewmat);

	polya = (POLY_F3*) GetNextPrim();
	SetPolyF3(polya);
	v0.vx = (short)(0);
	v0.vy = (short)(0);
	v0.vz = (short)(0);
	v1 = v2 = v0;
	otz = RotTransPers(&v0, (long*)&polya->x0, NULL, NULL);
	otz += RotTransPers(&v1, (long*)&polya->x1, NULL, NULL);
	otz += RotTransPers(&v2, (long*)&polya->x2, NULL, NULL);
	otz /= 3;
	setRGB0(polya, 0, 0, 0);
	addPrim(GetOTAt(GetCurrentBuffer(), otz), polya);
	IncrementNextPrim(sizeof(POLY_FT3));

	polyb = (POLY_F3*) GetNextPrim();
	SetPolyF3(polyb);
	v0.vx = (short)(ship->forward.vx >> 3);
	v0.vy = (short)(ship->forward.vy >> 3);
	v0.vz = (short)(ship->forward.vz >> 3);
	v1 = v2 = v0;
	otz = RotTransPers(&v0, (long*)&polyb->x0, NULL, NULL);
	otz += RotTransPers(&v1, (long*)&polyb->x1, NULL, NULL);
	otz += RotTransPers(&v2, (long*)&polyb->x2, NULL, NULL);
	otz /= 3;
	setRGB0(polyb, 0, 0, 0);
	addPrim(GetOTAt(GetCurrentBuffer(), otz), polyb);
	IncrementNextPrim(sizeof(POLY_FT3));

	polyc = (POLY_F3*) GetNextPrim();
	SetPolyF3(polyc);
	v0.vx = (short)(ship->right.vx >> 4);
	v0.vy = (short)(ship->right.vy >> 4);
	v0.vz = (short)(ship->right.vz >> 4);
	v1 = v2 = v0;
	otz = RotTransPers(&v0, (long*)&polyc->x0, NULL, NULL);
	otz += RotTransPers(&v1, (long*)&polyc->x1, NULL, NULL);
	otz += RotTransPers(&v2, (long*)&polyc->x2, NULL, NULL);
	otz /= 3;
	setRGB0(polyc, 0, 0, 0);
	addPrim(GetOTAt(GetCurrentBuffer(), otz), polyc);
	IncrementNextPrim(sizeof(POLY_FT3));

	polyd = (POLY_F3*) GetNextPrim();
	SetPolyF3(polyd);
	v0.vx = (short)(ship->up.vx >> 4);
	v0.vy = (short)(ship->up.vy >> 4);
	v0.vz = (short)(ship->up.vz >> 4);
	v1 = v2 = v0;
	otz = RotTransPers(&v0, (long*)&polyd->x0, NULL, NULL);
	otz += RotTransPers(&v1, (long*)&polyd->x1, NULL, NULL);
	otz += RotTransPers(&v2, (long*)&polyd->x2, NULL, NULL);
	otz /= 3;
	setRGB0(polyd, 0, 0, 0);
	addPrim(GetOTAt(GetCurrentBuffer(), otz), polyd);
	IncrementNextPrim(sizeof(POLY_FT3));

	SetLineF2(&lineA);
	setXY2(&lineA, polya->x0, polya->y0, polyb->x0, polyb->y0);
	setRGB0(&lineA, 55, 150, 255);
	DrawPrim(&lineA);

	SetLineF2(&lineB);
	setXY2(&lineB, polya->x0, polya->y0, polyc->x0, polyc->y0);
	setRGB0(&lineB, 255, 55, 87);
	DrawPrim(&lineB);

	SetLineF2(&lineC);
	setXY2(&lineC, polya->x0, polya->y0, polyd->x0, polyd->y0);
	setRGB0(&lineC, 10, 255, 110);
	DrawPrim(&lineC);
}