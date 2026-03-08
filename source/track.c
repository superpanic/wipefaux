#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "track.h"
#include "globals.h"
#include "utils.h"
#include "inline_n.h"
#include "libgte.h"
#include "display.h"


void LoadTrackVertices(Track *track, char *filename) {
	u_long i, b, length;
	u_char *bytes;
	u_long biggest_val = 0;
	u_long smallest_val = LONG_MAX;
	printf("biggest value init: %ld\n", biggest_val);
	printf("smallest value init: %ld\n", smallest_val);
	bytes = (u_char *) FileRead(filename, &length);
	if(bytes == NULL) {
		printf("Error reading %s from the CD.\n", filename);
		return;
	}
	b = 0;
	track->numvertices = length / BYTES_PER_VERTEX;
	track->vertices = (VECTOR*) malloc3(track->numvertices * sizeof(VECTOR));
	for(i=0;i<track->numvertices;i++) {
		track->vertices[i].vx = GetLongBE(bytes, &b);
		track->vertices[i].vy = GetLongBE(bytes, &b);
		track->vertices[i].vz = GetLongBE(bytes, &b);
		track->vertices[i].pad = GetLongBE(bytes, &b);		
	}
	free3(bytes);
}

void LoadTrackFaces(Track *track, char *filename) {
	u_long i, b, length;
	u_char *bytes;
	bytes = (u_char *) FileRead(filename, &length);
	if(bytes == NULL) {
		printf("Error reading %s from the CD.\n", filename);
		return;
	}
	b = 0;
	track->numfaces = length / BYTES_PER_FACE;
	track->faces = (Face *) malloc3(track->numfaces * sizeof(Face));

	for(i=0;i<track->numfaces;i++) {
		Face *f = &track->faces[i];
		f->indices[0] = GetShortBE(bytes, &b);
		f->indices[1] = GetShortBE(bytes, &b);
		f->indices[2] = GetShortBE(bytes, &b);
		f->indices[3] = GetShortBE(bytes, &b);

		f->normal.vx = GetShortBE(bytes, &b);
		f->normal.vy = GetShortBE(bytes, &b);
		f->normal.vz = GetShortBE(bytes, &b);

		f->texture = GetChar(bytes, &b);

		f->flags = GetChar(bytes, &b);

		f->color.r  = GetChar(bytes, &b);
		f->color.g  = GetChar(bytes, &b);
		f->color.b  = GetChar(bytes, &b);
		f->color.cd = GetChar(bytes, &b);
	}
	free3(bytes);
}

void LoadTrackSections(Track *track, char *filename) {
	u_long i, b, length;
	u_char *bytes;
	bytes = (u_char *) FileRead(filename, &length);
	if(bytes == NULL) {
		printf("Error reading %s from the CD.\n", filename);
		return;
	}
	b = 0;
	track->numsections = length / BYTES_PER_SECTION;
	track->sections = (Section*) malloc3(track->numsections * sizeof(Section));

	for(i=0;i<track->numsections; i++) {
		b+=4; // information about junction index

		track->sections[i].prev = track->sections + GetLongBE(bytes, &b);
		track->sections[i].next = track->sections + GetLongBE(bytes, &b);
	
		track->sections[i].center.vx = GetLongBE(bytes, &b);
		track->sections[i].center.vy = GetLongBE(bytes, &b);
		track->sections[i].center.vz = GetLongBE(bytes, &b);
		
		b += 118;

		track->sections[i].facestart = GetShortBE(bytes, &b);
		track->sections[i].numfaces = GetShortBE(bytes, &b);

		b+=4;

		track->sections[i].flags = GetShortBE(bytes, &b);
		track->sections[i].id = GetShortBE(bytes, &b); // read id
		track->sections[i].id = i; // overwrite id with index instad

		b+=2;
	}

	free3(bytes);
}

void RenderTrackSection(Track *track, Section *section, Camera *camera) {
	u_long i;
	POLY_F4 *poly;
	short nclip; // can be positive or negative
	long otz;

	MATRIX worldmat;
	MATRIX viewmat;

	VECTOR pos;
	SVECTOR rot;
	VECTOR scale;

	setVector(&pos,0,0,0);
	setVector(&rot,0,0,0);
	setVector(&scale,ONE,ONE,ONE);

	worldmat = (MATRIX){0};
	RotMatrix(&rot, &worldmat);
	TransMatrix(&worldmat, &pos);
	ScaleMatrix(&worldmat,&scale);
	CompMatrixLV(&camera->lookat, &worldmat, &viewmat);
	SetRotMatrix(&viewmat);
	SetTransMatrix(&viewmat);

	for(i=0; i<section->numfaces;i++) {
		Face *face = track->faces + section->facestart + i;
		poly = (POLY_F4 *)GetNextPrim();
		gte_ldv0(&track->vertices[face->indices[0]]); // pointer to vector
		gte_ldv1(&track->vertices[face->indices[1]]); // pointer to vector
		gte_ldv2(&track->vertices[face->indices[2]]); // pointer to vector
		gte_rtpt(); // rotation translate perspective
		gte_nclip(); // normal clip
		gte_stopz(&nclip); // store outer product
		if(nclip < 0) {
			continue;
		}
		gte_stsxy0(&poly->x0); // store screen xy
		
		// 4th vertex
		gte_ldv0(&track->vertices[face->indices[3]]);
		gte_rtps(); // rotation translate perspective
		gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);

		gte_avsz4(); // average z
		gte_stotz(&otz); // 
		if(otz > 0 && otz < OT_LEN) {
			setPolyF4(poly);
			poly->r0 = face->color.r;
			poly->g0 = face->color.g;
			poly->b0 = face->color.b;
			addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
			IncrementNextPrim(sizeof(POLY_F4));
		}
	}
}

void RenderTrack(Track *track, Camera *camera) {
	Section *current_section;
	VECTOR d;
	u_long distmagsq;
	u_long distmag;
	const u_long far_clip = 32000;
	current_section = track->sections;
	do {
		d.vx = current_section->center.vx - camera->position.vx;
		d.vy = current_section->center.vy - camera->position.vy;
		d.vz = current_section->center.vz - camera->position.vz;
		distmagsq = (d.vx*d.vx) + (d.vy*d.vy) + (d.vz*d.vz);
		distmag = SquareRoot12(distmagsq);
		if(distmag <= far_clip) {
			RenderTrackSection(track, current_section, camera);
		}
		current_section = current_section->next;
	} while(current_section != track->sections); // to prevent looping
}
