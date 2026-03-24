#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "track.h"
#include "globals.h"
#include "libgpu.h"
#include "utils.h"
#include "inline_n.h"
#include "libgte.h"
#include "display.h"
#include "texture.h"


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

void LoadTrackFaces(Track *track, char *filename, u_short texture_counter) {
	u_long i, b, length;
	u_char *bytes;
	Texture *texture;
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

		f->texture += texture_counter;
		texture = GetFromTextureStore(f->texture);
		f->tpage = texture->tpage;
		f->clut = texture->clut;

		// 
		if(f->flags & FACE_FLIP_TEXTURE) {
			// flip texture
			f->u0 = texture->u1;
			f->v0 = texture->v1;
			f->u1 = texture->u0;
			f->v1 = texture->v0;
			f->u2 = texture->u3;
			f->v2 = texture->v3;
			f->u3 = texture->u2;
			f->v3 = texture->v2;
		} else {
			// normal texture
			f->u0 = texture->u0;
			f->v0 = texture->v0;
			f->u1 = texture->u1;
			f->v1 = texture->v1;
			f->u2 = texture->u2;
			f->v2 = texture->v2;
			f->u3 = texture->u3;
			f->v3 = texture->v3;
		}

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

void RenderQuadRecursive(Face *face, SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3, u_short level, u_short depth) {
	if(level >= depth) {
		short nclip; // can be positive or negative
		long otz;
		LINE_F2 *line0, *line1, *line2, *line3;
		POLY_FT4 *poly;

		poly = (POLY_FT4 *)GetNextPrim();
		gte_ldv0(v0); // pointer to vector
		gte_ldv1(v1); // pointer to vector
		gte_ldv2(v2); // pointer to vector
		gte_rtpt(); // rotation translate perspective
		gte_nclip(); // normal clip
		gte_stopz(&nclip); // store outer product
		if(nclip < 0) {
			return;
		}

		gte_stsxy0(&poly->x0); // store screen xy
		
		// 4th vertex
		gte_ldv0(v3);
		gte_rtps(); // rotation translate perspective
		gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);

		gte_avsz4(); // average z
		gte_stotz(&otz); // 
		if(otz > 0 && otz < OT_LEN) {
			setPolyFT4(poly);
			poly->r0 = face->color.r;
			poly->g0 = face->color.g;
			poly->b0 = face->color.b;
			poly->tpage = face->tpage;
			poly->clut = face->clut;
			setUV4(poly, face->u0, face->v0, face->u1, face->v1, face->u2, face->v2, face->u3, face->v3);
			addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
			IncrementNextPrim(sizeof(POLY_FT4));

			// draw lines
			line0 = (LINE_F2*) GetNextPrim();
			SetLineF2(line0);
			setXY2(line0, poly->x0, poly->y0, poly->x1, poly->y1);
			setRGB0(line0,0,0,0);
			addPrim(GetOTAt(GetCurrentBuffer(), 0), line0);
			IncrementNextPrim(sizeof(LINE_F2));

			line1 = (LINE_F2*) GetNextPrim();
			SetLineF2(line1);
			setXY2(line1, poly->x1, poly->y1, poly->x3, poly->y3);
			setRGB0(line1,0,0,0);
			addPrim(GetOTAt(GetCurrentBuffer(), 0), line1);
			IncrementNextPrim(sizeof(LINE_F2));

			line2 = (LINE_F2*) GetNextPrim();
			SetLineF2(line2);
			setXY2(line2, poly->x3, poly->y3, poly->x2, poly->y2);
			setRGB0(line2,0,0,0);
			addPrim(GetOTAt(GetCurrentBuffer(), 0), line2);
			IncrementNextPrim(sizeof(LINE_F2));

			line3 = (LINE_F2*) GetNextPrim();
			SetLineF2(line3);
			setXY2(line3, poly->x2, poly->y2, poly->x0, poly->y0);
			setRGB0(line3,0,0,0);
			addPrim(GetOTAt(GetCurrentBuffer(), 0), line3);
			IncrementNextPrim(sizeof(LINE_F2));
		}
	} else {
		SVECTOR vm01, vm02, vm03, vm12, vm32, vm13;
		vm01 = (SVECTOR) {(v0->vx + v1->vx) >> 1, (v0->vy + v1->vy) >> 1, (v0->vz + v1->vz) >> 1};
		vm02 = (SVECTOR) {(v0->vx + v2->vx) >> 1, (v0->vy + v2->vy) >> 1, (v0->vz + v2->vz) >> 1};
		vm03 = (SVECTOR) {(v0->vx + v3->vx) >> 1, (v0->vy + v3->vy) >> 1, (v0->vz + v3->vz) >> 1};
		vm12 = (SVECTOR) {(v1->vx + v2->vx) >> 1, (v1->vy + v2->vy) >> 1, (v1->vz + v2->vz) >> 1};
		vm13 = (SVECTOR) {(v1->vx + v3->vx) >> 1, (v1->vy + v3->vy) >> 1, (v1->vz + v3->vz) >> 1};
		vm32 = (SVECTOR) {(v3->vx + v2->vx) >> 1, (v3->vy + v2->vy) >> 1, (v3->vz + v2->vz) >> 1};

		RenderQuadRecursive(face, v0, &vm01, &vm02, &vm03, level+1, depth);
		RenderQuadRecursive(face, &vm01, v1, &vm03, &vm13, level+1, depth);
		RenderQuadRecursive(face, &vm02, &vm03, v2, &vm32, level+1, depth);
		RenderQuadRecursive(face, &vm03, &vm13, &vm32, v3, level+1, depth);
	}
}

void RenderTrackSection(Track *track, Section *section, Camera *camera, u_long distmag) {
	int i, depth;

	MATRIX worldmat;
	MATRIX viewmat;

	VECTOR pos;
	SVECTOR rot;
	VECTOR scale;

	SVECTOR v0, v1, v2, v3;

	setVector(&pos,0,0,0);
	setVector(&rot,0,0,0);
	setVector(&scale,ONE,ONE,ONE);

	worldmat = (MATRIX){0};
	RotMatrix(&rot, &worldmat);
	TransMatrix(&worldmat, &pos);
	ScaleMatrix(&worldmat,&scale);
//	CompMatrixLV(&camera->lookat, &worldmat, &viewmat);
	CompMatrixLV(&camera->rotmat, &worldmat, &viewmat);
	SetRotMatrix(&viewmat);
	SetTransMatrix(&viewmat);

	for(i=0; i<section->numfaces;i++) {
		Face *face = track->faces + section->facestart + i;

		v0.vx = (short) (track->vertices[face->indices[1]].vx - camera->position.vx);
		v0.vy = (short) (track->vertices[face->indices[1]].vy - camera->position.vy);
		v0.vz = (short) (track->vertices[face->indices[1]].vz - camera->position.vz);

		v1.vx = (short) (track->vertices[face->indices[0]].vx - camera->position.vx);
		v1.vy = (short) (track->vertices[face->indices[0]].vy - camera->position.vy);
		v1.vz = (short) (track->vertices[face->indices[0]].vz - camera->position.vz);

		v2.vx = (short) (track->vertices[face->indices[2]].vx - camera->position.vx);
		v2.vy = (short) (track->vertices[face->indices[2]].vy - camera->position.vy);
		v2.vz = (short) (track->vertices[face->indices[2]].vz - camera->position.vz);

		v3.vx = (short) (track->vertices[face->indices[3]].vx - camera->position.vx);
		v3.vy = (short) (track->vertices[face->indices[3]].vy - camera->position.vy);
		v3.vz = (short) (track->vertices[face->indices[3]].vz - camera->position.vz);

		depth = 0;
		if(distmag < 600000) depth = 1;
		if(distmag < 200000) depth = 2;

		RenderQuadRecursive(face, &v0, &v1, &v2, &v3, 0, depth);
	}
}

long Clamp16Bits(long value) {
	if(value > +32767) return +32767;
	if(value < -32767) return -32767;
	return value;
}

void RenderTrack(Track *track, Camera *camera) {
	Section *current_section;
	VECTOR d;
	u_long distmagsq;
	u_long distmag;
	const u_long far_clip = 950000;
	current_section = track->sections;
	do {
		d.vx = Clamp16Bits(current_section->center.vx - camera->position.vx);
		d.vy = Clamp16Bits(current_section->center.vy - camera->position.vy);
		d.vz = Clamp16Bits(current_section->center.vz - camera->position.vz);
		distmagsq = (d.vx*d.vx) + (d.vy*d.vy) + (d.vz*d.vz);
		distmag = SquareRoot12(distmagsq);
		if(distmag <= far_clip) {
			RenderTrackSection(track, current_section, camera, distmag);
		}
		current_section = current_section->next;
	} while(current_section != track->sections); // to prevent looping
}
