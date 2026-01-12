#ifndef OBJECT_H
#define OBJECT_H

#include <sys/types.h>
#include "globals.h"
#include "libgte.h"

typedef struct Object {
	char name[17];
	short num_verts;
	short num_normals;
	short num_primitives;
	short flags;
	VECTOR origin;
	SVECTOR *verts;
	SVECTOR *normals;
	//Primitive *primitives;
	SVECTOR rotation;
	VECTOR position;
	VECTOR scale;
} Object;

void LoadObjectPRM(Object *object, char *filename);

#endif