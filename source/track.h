#ifndef TRACK_H
#define TRACK_H

#include <sys/types.h>
#include <libgte.h>

typedef struct Face {
	short indices[4];
	char flags;
	SVECTOR normal;
	CVECTOR color;
	char texture;
	short clut;
	short tpage;
	short u0, v0;
	short u1, v1;
	short u2, v2;
	short u3, v3;
} Face;

typedef struct Section {
	short id;
	short flags;

	struct Section *prev;
	struct Section *next;

	VECTOR center;
	SVECTOR normal;

	short numfaces;
	short facestart;
} Section;

 typedef struct Track {
	long numvertices;
	VECTOR *vertices;

	long numfaces;
	Face *faces;

	long numsections;
	Section *sections;
 } Track;
 

#endif