#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "object.h"
#include "globals.h"
#include "inline_n.h"
#include "display.h"
#include "libgpu.h"
#include "utils.h"
#include "libgte.h"
#include "globals.h"
#include "utils.h"
#include "texture.h"

const char *type_names[] = {
	"F3",
	"FT3", 
	"F4",
	"FT4",
	"G3",
	"GT3",
	"G4",
	"GT4",

	"LF2",
	"TSPR",
	"BSPR",

	"LSF3",
	"LSFT3",
	"LSF4",
	"LSFT4",
	"LSG3",
	"LSGT3",
	"LSG4",
	"LSGT4",

	"Spline",
	"InfiniteLight",
	"PointLight",
	"SpotLight"
};

u_char LoadObjectsPRM(Object *root, char *filename, u_short texture_counter) {
	u_char *bytes;
	u_long b;
	u_long i;
	u_long length;
	u_short u_offset, v_offset;
	Texture *texture;
	u_char ship_counter;

	bytes = (u_char*) FileRead(filename, &length);

	if (bytes == NULL) {
		printf("Error reading %s from the CD.\n", filename);
		return 0;
	}

	Object *object = root;
	root->prev = NULL;

	b = 0;
	while(b < length) {
		ship_counter++;
		// check if this is first run
		if(b > 0) {
			object->next = (Object *)malloc3(sizeof(Object));
			if(object->next == NULL) return 0;
			object->next->prev = object;
			object->next->next = NULL;
			object = object->next;
		}

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

		object->verts = (SVECTOR *) malloc3(object->num_verts * sizeof(SVECTOR));
		for (i = 0; i < object->num_verts; i++) {
			object->verts[i].vx = (GetShortBE(bytes, &b));
			object->verts[i].vy = (GetShortBE(bytes, &b));
			object->verts[i].vz = (GetShortBE(bytes, &b));
			b += 2; // padding
			if(DEBUG) printf("Vertex[%d] = (%d, %d, %d) \n", i, object->verts[i].vx, object->verts[i].vy, object->verts[i].vz);
		}

		object->normals = (SVECTOR *) malloc3(object->num_normals * sizeof(SVECTOR));
		for (i = 0; i < object->num_normals; i++) {
			object->normals[i].vx = GetShortBE(bytes, &b);
			object->normals[i].vy = GetShortBE(bytes, &b);
			object->normals[i].vz = GetShortBE(bytes, &b);
			b += 2; // padding
			if(DEBUG) printf("Normal[%d] = (%d, %d, %d) \n", i, object->normals[i].vx, object->normals[i].vy, object->normals[i].vz);
		}

		object->primitives = (PrimitiveNode *) malloc3(object->num_primitives * sizeof(PrimitiveNode));
		for (i = 0; i < object->num_primitives; i++) {
			object->primitives[i].type = GetShortBE(bytes, &b);
			object->primitives[i].flag = GetShortBE(bytes, &b);
			
			switch(object->primitives[i].type) {
				case TypeF3: {
					F3 *prm;
					if(DEBUG) printf("Loading primitive type F3 \n");
					object->primitives[i].primitive = (Prm*) malloc3(sizeof(F3));
					prm            = (F3*) object->primitives[i].primitive;
					prm->type      = TypeF3;
					prm->coords[0] = GetShortBE(bytes, &b);
					prm->coords[1] = GetShortBE(bytes, &b);
					prm->coords[2] = GetShortBE(bytes, &b);
					prm->pad1      = GetShortBE(bytes, &b);
					prm->color.r   = GetChar(bytes, &b);
					prm->color.g   = GetChar(bytes, &b);
					prm->color.b   = GetChar(bytes, &b);
					prm->color.cd  = GetChar(bytes, &b);
					break;
				}
				case TypeFT3: {
					FT3 *prm;
					if(DEBUG) printf("Loading primitive type FT3 \n");
					object->primitives[i].primitive = (Prm*) malloc3(sizeof(FT3));
					prm            = (FT3*) object->primitives[i].primitive;
					prm->type      = TypeFT3;
					prm->coords[0] = GetShortBE(bytes, &b);
					prm->coords[1] = GetShortBE(bytes, &b);
					prm->coords[2] = GetShortBE(bytes, &b);
					prm->texture   = GetShortBE(bytes, &b);
					prm->clut      = GetShortBE(bytes, &b);
					prm->tpage     = GetShortBE(bytes, &b);
					prm->u0        = GetChar(bytes, &b);
					prm->v0        = GetChar(bytes, &b);
					prm->u1        = GetChar(bytes, &b);
					prm->v1        = GetChar(bytes, &b);
					prm->u2        = GetChar(bytes, &b);
					prm->v2        = GetChar(bytes, &b);
					prm->pad1      = GetShortBE(bytes, &b);
					prm->color     = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };

					prm->texture += texture_counter;
					texture = GetFromTextureStore(prm->texture);

					prm->tpage = texture->tpage;
					prm->clut = texture->clut;

					u_offset = texture->u0;
					v_offset = texture->v0;
					prm->u0 += u_offset;
					prm->v0 += v_offset;
					prm->u1 += u_offset;
					prm->v1 += v_offset;
					prm->u2 += u_offset;
					prm->v2 += v_offset;
					break;
				}
				case TypeF4: {
					F4 *prm;
					if(DEBUG) printf("Loading primitive type F4 \n");
					object->primitives[i].primitive = (Prm*) malloc3(sizeof(F4));
					prm            = (F4*) object->primitives[i].primitive;
					prm->type      = TypeF4;
					prm->coords[0] = GetShortBE(bytes, &b);
					prm->coords[1] = GetShortBE(bytes, &b);
					prm->coords[2] = GetShortBE(bytes, &b);
					prm->coords[3] = GetShortBE(bytes, &b);
					prm->color     = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					break;
				}
				case TypeFT4: {
					FT4 *prm;
					if(DEBUG) printf("Loading primitive type FT4 \n");
					object->primitives[i].primitive = (Prm*) malloc3(sizeof(FT4));
					prm            = (FT4*) object->primitives[i].primitive;
					prm->type      = TypeFT4;
					prm->coords[0] = GetShortBE(bytes, &b);
					prm->coords[1] = GetShortBE(bytes, &b);
					prm->coords[2] = GetShortBE(bytes, &b);
					prm->coords[3] = GetShortBE(bytes, &b);
					prm->texture   = GetShortBE(bytes, &b);
					prm->clut      = GetShortBE(bytes, &b);
					prm->tpage     = GetShortBE(bytes, &b);
					prm->u0        = GetChar(bytes, &b);
					prm->v0        = GetChar(bytes, &b);
					prm->u1        = GetChar(bytes, &b);
					prm->v1        = GetChar(bytes, &b);
					prm->u2        = GetChar(bytes, &b);
					prm->v2        = GetChar(bytes, &b);
					prm->u3        = GetChar(bytes, &b);
					prm->v3        = GetChar(bytes, &b);
					prm->pad1      = GetShortBE(bytes, &b);
					prm->color     = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };

					prm->texture += texture_counter;
					texture = GetFromTextureStore(prm->texture);

					prm->tpage = texture->tpage;
					prm->clut = texture->clut;

					u_offset = texture->u0;
					v_offset = texture->v0;
					prm->u0 += u_offset;
					prm->v0 += v_offset;
					prm->u1 += u_offset;
					prm->v1 += v_offset;
					prm->u2 += u_offset;
					prm->v2 += v_offset;
					prm->u3 += u_offset;
					prm->v3 += v_offset;

					break;
				}
				case TypeG3: {
					G3 *prm;
					if(DEBUG) printf("Loading primitive type G3 \n");
					object->primitives[i].primitive = (Prm*) malloc3(sizeof(G3));
					prm            = (G3*) object->primitives[i].primitive;
					prm->type      = TypeG3;
					prm->coords[0] = GetShortBE(bytes, &b);
					prm->coords[1] = GetShortBE(bytes, &b);
					prm->coords[2] = GetShortBE(bytes, &b);
					prm->pad1      = GetShortBE(bytes, &b);
					prm->color[0]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[1]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[2]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					break;
				}
				case TypeGT3: {
					GT3 *prm;
					if(DEBUG) printf("Loading primitive type GT3 \n");
					object->primitives[i].primitive = (Prm*) malloc3(sizeof(GT3));
					prm            = (GT3*) object->primitives[i].primitive;
					prm->type      = TypeGT3;
					prm->coords[0] = GetShortBE(bytes, &b);
					prm->coords[1] = GetShortBE(bytes, &b);
					prm->coords[2] = GetShortBE(bytes, &b);
					prm->texture   = GetShortBE(bytes, &b);
					prm->clut      = GetShortBE(bytes, &b);
					prm->tpage     = GetShortBE(bytes, &b);
					prm->u0        = GetChar(bytes, &b);
					prm->v0        = GetChar(bytes, &b);
					prm->u1        = GetChar(bytes, &b);
					prm->v1        = GetChar(bytes, &b);
					prm->u2        = GetChar(bytes, &b);
					prm->v2        = GetChar(bytes, &b);
					prm->pad1      = GetShortBE(bytes, &b);
					prm->color[0]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[1]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[2]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };

					prm->texture += texture_counter;
					texture = GetFromTextureStore(prm->texture);

					prm->tpage = texture->tpage;
					prm->clut = texture->clut;

					u_offset = texture->u0;
					v_offset = texture->v0;
					prm->u0 += u_offset;
					prm->v0 += v_offset;
					prm->u1 += u_offset;
					prm->v1 += v_offset;
					prm->u2 += u_offset;
					prm->v2 += v_offset;

					break;
				}
				case TypeG4: {
					G4 *prm;
					if(DEBUG) printf("Loading primitive type G4 \n");
					object->primitives[i].primitive = (Prm*) malloc3(sizeof(G4));
					prm            = (G4*) object->primitives[i].primitive;
					prm->type      = TypeG4;
					prm->coords[0] = GetShortBE(bytes, &b);
					prm->coords[1] = GetShortBE(bytes, &b);
					prm->coords[2] = GetShortBE(bytes, &b);
					prm->coords[3] = GetShortBE(bytes, &b);
					prm->color[0]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[1]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[2]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[3]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					break;
				}
				case TypeGT4: {
					GT4 *prm;
					if(DEBUG) printf("Loading primitive type GT4 \n");
					object->primitives[i].primitive = (Prm*) malloc3(sizeof(GT4));
					prm            = (GT4*) object->primitives[i].primitive;
					prm->type      = TypeGT4;
					prm->coords[0] = GetShortBE(bytes, &b);
					prm->coords[1] = GetShortBE(bytes, &b);
					prm->coords[2] = GetShortBE(bytes, &b);
					prm->coords[3] = GetShortBE(bytes, &b);
					prm->texture   = GetShortBE(bytes, &b);
					prm->clut      = GetShortBE(bytes, &b);
					prm->tpage     = GetShortBE(bytes, &b);
					prm->u0        = GetChar(bytes, &b);
					prm->v0        = GetChar(bytes, &b);
					prm->u1        = GetChar(bytes, &b);
					prm->v1        = GetChar(bytes, &b);
					prm->u2        = GetChar(bytes, &b);
					prm->v2        = GetChar(bytes, &b);
					prm->u3        = GetChar(bytes, &b);
					prm->v3        = GetChar(bytes, &b);
					prm->pad1      = GetShortBE(bytes, &b);
					prm->color[0]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[1]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[2]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					prm->color[3]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };

					prm->texture += texture_counter;
					texture = GetFromTextureStore(prm->texture);

					prm->tpage = texture->tpage;
					prm->clut = texture->clut;

					u_offset = texture->u0;
					v_offset = texture->v0;
					prm->u0 += u_offset;
					prm->v0 += v_offset;
					prm->u1 += u_offset;
					prm->v1 += v_offset;
					prm->u2 += u_offset;
					prm->v2 += v_offset;
					prm->u3 += u_offset;
					prm->v3 += v_offset;

					break;
				}
				case TypeTSPR:
				case TypeBSPR: {
					SPR *prm;
					if(DEBUG) printf("Loading primitive type SPR \n");
					object->primitives[i].primitive = (Prm*) malloc3(sizeof(SPR));
					prm            = (SPR*) object->primitives[i].primitive;
					prm->type      = TypeTSPR;
					prm->coord     = GetShortBE(bytes, &b);
					prm->width     = GetShortBE(bytes, &b);
					prm->height    = GetShortBE(bytes, &b);
					prm->texture   = GetShortBE(bytes, &b);
					prm->color     = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
					break;
				}
				case TypeSpline: {
					printf("Loading primitive type Spline \n");
					b += 52; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypePointLight: {
					printf("Loading primitive type PointLight\n");
					b += 24; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeSpotLight: {
					printf("Loading primitive type SpotLight\n");
					b += 36; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeInfiniteLight: {
					printf("Loading primitive type InfiniteLight\n");
					b += 12; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeLSF3: {
					printf("Loading primitive type LSF3\n");
					b += 12; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeLSFT3: {
					printf("Loading primitive type LSFT3\n");
					b += 24; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeLSF4: {
					printf("Loading primitive type LSF4\n");
					b += 16; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeLSFT4: {
					printf("Loading primitive type LSFT4\n");
					b += 28; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeLSG3: {
					printf("Loading primitive type LSG3\n");
					b += 24; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeLSGT3: {
					printf("Loading primitive type LSGT3\n");
					b += 36; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeLSG4: {
					printf("Loading primitive type LSG4\n");
					b += 32; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
				case TypeLSGT4: {
					printf("Loading primitive type LSGT4\n");
					b += 42; // --> skip this amount of bytes to bypass this primitive type
					break;
				}
			}
		};

		object->position.vx = object->origin.vx;
		object->position.vy = object->origin.vy;
		object->position.vz = object->origin.vz;
		object->scale = (VECTOR) {ONE, ONE, ONE};
		object->rotation = (SVECTOR) {0,0,0};

		if(DEBUG) {
			printf("object pos: %li %li %li\n", 
				object->position.vx, 
				object->position.vy, 
				object->position.vz);

			printf("object scale: %li %li %li\n", 
				object->scale.vx, 
				object->scale.vy, 
				object->scale.vz);
		}
	}
	object->next = NULL;
	free3(bytes);
	
	printf("no of ships loaded: %d\n", ship_counter);

	return ship_counter;
}

void RenderObject(Object *object, Camera *camera) {
	int i;
	short nclip;
	long otz, p, flg;

	MATRIX worldmat;
	MATRIX viewmat;

	RotMatrix(&object->rotation, &worldmat);
	TransMatrix(&worldmat, &object->position);
	ScaleMatrix(&worldmat,&object->scale);

	CompMatrixLV(&camera->lookat, &worldmat, &viewmat);
	
	SetRotMatrix(&viewmat);
	SetTransMatrix(&viewmat);

	for(i=0; i<object->num_primitives; i++) {
		switch(object->primitives[i].type) {
			case TypeF3: {
				POLY_F3* poly;
				F3* prm;
				prm = (F3*) object->primitives[i].primitive;
				poly = (POLY_F3*) GetNextPrim();
				gte_ldv0(&object->verts[prm->coords[0]]);
				gte_ldv1(&object->verts[prm->coords[1]]);
				gte_ldv2(&object->verts[prm->coords[2]]);
				gte_rtpt();
				gte_nclip();
				gte_stopz(&nclip);
				if(nclip<0) {
					continue;
				}
				gte_stsxy3(&poly->x0, &poly->x1, &poly->x2);
				gte_avsz3();
				gte_stotz(&otz);
				if(otz > 0 && otz < OT_LEN) {
					SetPolyF3(poly);
					setRGB0(poly, prm->color.r, prm->color.g, prm->color.b);
					addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
					IncrementNextPrim(sizeof(POLY_F3));
				}
				break;
			}
			case TypeG3: {
				POLY_G3* poly;
				G3* prm;
				prm = (G3*) object->primitives[i].primitive;
				poly = (POLY_G3*) GetNextPrim();
				gte_ldv0(&object->verts[prm->coords[0]]);
				gte_ldv1(&object->verts[prm->coords[1]]);
				gte_ldv2(&object->verts[prm->coords[2]]);
				gte_rtpt();
				gte_nclip();
				gte_stopz(&nclip);
				if(nclip<0) {
					continue;
				}
				gte_stsxy3(&poly->x0, &poly->x1, &poly->x2);
				gte_avsz3();
				gte_stotz(&otz);
				if(otz > 0 && otz < OT_LEN) {
					SetPolyG3(poly);
					setRGB0(poly, prm->color[0].r, prm->color[0].g, prm->color[0].b);
					setRGB1(poly, prm->color[1].r, prm->color[1].g, prm->color[1].b);
					setRGB2(poly, prm->color[2].r, prm->color[2].g, prm->color[2].b);
					addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
					IncrementNextPrim(sizeof(POLY_G3));
				}
				break;
			}
			case TypeFT3: {
				POLY_FT3* poly;
				FT3* prm;
				prm = (FT3*) object->primitives[i].primitive;
				poly = (POLY_FT3*) GetNextPrim();
				gte_ldv0(&object->verts[prm->coords[0]]);
				gte_ldv1(&object->verts[prm->coords[1]]);
				gte_ldv2(&object->verts[prm->coords[2]]);
				gte_rtpt();
				gte_nclip();
				gte_stopz(&nclip);
				if(nclip<0) {
					continue;
				}
				gte_stsxy3(&poly->x0, &poly->x1, &poly->x2);
				gte_avsz3();
				gte_stotz(&otz);
				if(otz > 0 && otz < OT_LEN) {
					SetPolyFT3(poly);
					setRGB0(poly, prm->color.r, prm->color.g, prm->color.b);
					poly->tpage = prm->tpage;
					poly->clut = prm->clut;
					setUV3(poly, prm->u0, prm->v0, prm->u1, prm->v1, prm->u2, prm->v2);
					addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
					IncrementNextPrim(sizeof(POLY_FT3));
				}
				break;
			}
			case TypeGT3: {
				POLY_GT3* poly;
				GT3* prm;
				prm = (GT3*) object->primitives[i].primitive;
				poly = (POLY_GT3*) GetNextPrim();
				gte_ldv0(&object->verts[prm->coords[0]]);
				gte_ldv1(&object->verts[prm->coords[1]]);
				gte_ldv2(&object->verts[prm->coords[2]]);
				gte_rtpt();
				gte_nclip();
				gte_stopz(&nclip);
				if(nclip<0) {
					continue;
				}
				gte_stsxy3(&poly->x0, &poly->x1, &poly->x2);
				gte_avsz3();
				gte_stotz(&otz);
				if(otz > 0 && otz < OT_LEN) {
					SetPolyGT3(poly);
					setRGB0(poly, prm->color[0].r, prm->color[0].g, prm->color[0].b);
					setRGB1(poly, prm->color[1].r, prm->color[1].g, prm->color[1].b);
					setRGB2(poly, prm->color[2].r, prm->color[2].g, prm->color[2].b);
					poly->tpage = prm->tpage;
					poly->clut = prm->clut;
					setUV3(poly, prm->u0, prm->v0, prm->u1, prm->v1, prm->u2, prm->v2);
					addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
					IncrementNextPrim(sizeof(POLY_GT3));
				}
				break;
			}
			case TypeF4: {
				POLY_F4 *poly;
				F4 *prm;
				prm = (F4*) object->primitives[i].primitive;
				poly = (POLY_F4*) GetNextPrim();
				gte_ldv0(&object->verts[prm->coords[0]]);
				gte_ldv1(&object->verts[prm->coords[1]]);
				gte_ldv2(&object->verts[prm->coords[2]]);
				gte_rtpt();
				gte_nclip();
				gte_stopz(&nclip);
				if(nclip < 0) {
					continue;
				}
				gte_stsxy0(&poly->x0);
				gte_ldv0(&object->verts[prm->coords[3]]);
				gte_rtps();
				gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);
				gte_avsz4();
				gte_stotz(&otz);
				if(otz > 0 && otz < OT_LEN) {
					SetPolyF4(poly);
					setRGB0(poly, prm->color.r, prm->color.g, prm->color.b);

					addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
					IncrementNextPrim(sizeof(POLY_F4));
				}
				break;
			}
			case TypeFT4: {
				POLY_FT4 *poly;
				FT4 *prm;
				prm = (FT4*) object->primitives[i].primitive;
				poly = (POLY_FT4*) GetNextPrim();
				gte_ldv0(&object->verts[prm->coords[0]]);
				gte_ldv1(&object->verts[prm->coords[1]]);
				gte_ldv2(&object->verts[prm->coords[2]]);
				gte_rtpt();
				gte_nclip();
				gte_stopz(&nclip);
				if(nclip < 0) {
					continue;
				}
				gte_stsxy0(&poly->x0);
				gte_ldv0(&object->verts[prm->coords[3]]);
				gte_rtps();
				gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);
				gte_avsz4();
				gte_stotz(&otz);
				if(otz > 0 && otz < OT_LEN) {
					SetPolyFT4(poly);
					setRGB0(poly, prm->color.r, prm->color.g, prm->color.b);
					poly->tpage = prm->tpage;
					poly->clut = prm->clut;
					setUV4(poly, prm->u0, prm->v0, prm->u1, prm->v1, prm->u2, prm->v2, prm->u3, prm->v3);

					addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
					IncrementNextPrim(sizeof(POLY_FT4));
				}
				break;
			}
			case TypeG4: {
				POLY_G4 *poly;
				G4 *prm;
				prm = (G4*) object->primitives[i].primitive;
				poly = (POLY_G4*) GetNextPrim();
				gte_ldv0(&object->verts[prm->coords[0]]);
				gte_ldv1(&object->verts[prm->coords[1]]);
				gte_ldv2(&object->verts[prm->coords[2]]);
				gte_rtpt();
				gte_nclip();
				gte_stopz(&nclip);
				if(nclip < 0) {
					continue;
				}
				gte_stsxy0(&poly->x0);
				gte_ldv0(&object->verts[prm->coords[3]]);
				gte_rtps();
				gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);
				gte_avsz4();
				gte_stotz(&otz);
				if(otz > 0 && otz < OT_LEN) {
					SetPolyG4(poly);
					setRGB0(poly, prm->color[0].r, prm->color[1].g, prm->color[2].b);
					setRGB1(poly, prm->color[0].r, prm->color[1].g, prm->color[2].b);
					setRGB2(poly, prm->color[0].r, prm->color[1].g, prm->color[2].b);
					setRGB3(poly, prm->color[0].r, prm->color[1].g, prm->color[2].b);

					addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
					IncrementNextPrim(sizeof(POLY_G4));
				}
				break;
			}
			case TypeGT4: {
				POLY_GT4 *poly;
				GT4 *prm;
				prm = (GT4*) object->primitives[i].primitive;
				poly = (POLY_GT4*) GetNextPrim();
				gte_ldv0(&object->verts[prm->coords[0]]);
				gte_ldv1(&object->verts[prm->coords[1]]);
				gte_ldv2(&object->verts[prm->coords[2]]);
				gte_rtpt();
				gte_nclip();
				gte_stopz(&nclip);
				if(nclip < 0) {
					continue;
				}
				gte_stsxy0(&poly->x0);
				gte_ldv0(&object->verts[prm->coords[3]]);
				gte_rtps();
				gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);
				gte_avsz4();
				gte_stotz(&otz);
				if(otz > 0 && otz < OT_LEN) {
					SetPolyGT4(poly);
					setRGB0(poly, prm->color[0].r, prm->color[1].g, prm->color[2].b);
					setRGB1(poly, prm->color[0].r, prm->color[1].g, prm->color[2].b);
					setRGB2(poly, prm->color[0].r, prm->color[1].g, prm->color[2].b);
					setRGB3(poly, prm->color[0].r, prm->color[1].g, prm->color[2].b);
					poly->tpage = prm->tpage;
					poly->clut = prm->clut;
					setUV4(poly, prm->u0, prm->v0, prm->u1, prm->v1, prm->u2, prm->v2, prm->u3, prm->v3);
					
					addPrim(GetOTAt(GetCurrentBuffer(), otz), poly);
					IncrementNextPrim(sizeof(POLY_GT4));
				}
				break;
			}

			
		}
	}
}

void PrintObjectNames(Object *root, u_char n_ships) {
	Object *o = root;
	for(u_char i=0; i<n_ships; i++) {
		printf("ship %s loaded\n",o->name);
		o=o->next;
	}
}