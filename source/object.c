#include "object.h"


void LoadObjectPRM(Object *object, char *filename) {
	u_char *bytes;
	u_long b;
	u_long i;
	u_long length;

	bytes = (u_char*) FileRead(filename, &length);

	if (bytes == NULL) {
		printf("Error reading %s from the CD.\n", filename);
		return;
	}

	b = 0;

	for (i = 0; i < 16; i++) {
		object->name[i] = GetChar(bytes, &b);
	}
	printf("Loading object: %s \n", object->name);

	object->num_verts = GetShortBE(bytes, &b);
	object->verts = NULL;
	b += 6; // unused padding
	printf("Num verts = %d \n", object->num_verts);

	object->num_normals = GetShortBE(bytes, &b);
	object->normals = NULL;
	b += 6; // unused padding
	printf("Num normals = %d \n", object->num_normals);

	object->num_primitives = GetShortBE(bytes, &b);
	//object->primitives = NULL;
	b += 22; // unused padding
	printf("Num primitives = %d \n", object->num_primitives);

	object->flags = GetShortBE(bytes, &b);
	b += 26; // unused padding

	object->origin.vx = GetLongBE(bytes, &b);
	object->origin.vy = GetLongBE(bytes, &b);
	object->origin.vz = GetLongBE(bytes, &b);
	printf("Origin (%ld, %ld, %ld) \n", object->origin.vx, object->origin.vy, object->origin.vz);

	// Skip unused bytes ("skeleton" information, rotation matrices, and extra flags)
	b += 48;

	object->verts = (SVECTOR*) malloc3(object->num_verts * sizeof(SVECTOR));
	for (i = 0; i < object->num_verts; i++) {
		object->verts[i].vx = (GetShortBE(bytes, &b));
		object->verts[i].vy = (GetShortBE(bytes, &b));
		object->verts[i].vz = (GetShortBE(bytes, &b));
		b += 2; // padding
		printf("Vertex[%d] = (%d, %d, %d) \n", i, object->verts[i].vx, object->verts[i].vy, object->verts[i].vz);
	}

	object->normals = (SVECTOR*) malloc3(object->num_normals * sizeof(SVECTOR));
	for (i = 0; i < object->num_normals; i++) {
		object->normals[i].vx = GetShortBE(bytes, &b);
		object->normals[i].vy = GetShortBE(bytes, &b);
		object->normals[i].vz = GetShortBE(bytes, &b);
		b += 2; // padding
		printf("Normal[%d] = (%d, %d, %d) \n", i, object->normals[i].vx, object->normals[i].vy, object->normals[i].vz);
	}

	free3(bytes);
}