#ifndef OBJECT_H
#define OBJECT_H

#include <sys/types.h>
#include "globals.h"
#include "libgte.h"

typedef struct F3 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[3];     /* Indices of the coords */
	short             pad1;
	CVECTOR           color;
} F3;

typedef struct FT3 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[3];     /* Indices of the coords */
	short             texture;
	short             clut;
	short             tpage;
	char              u0;
	char              v0;
	char              u1;
	char              v1;
	char              u2;
	char              v2;
	short             pad1;
	CVECTOR           color;
} FT3;

typedef struct F4 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[4];     /* Indices of the coords */
	CVECTOR           color;
} F4;

typedef struct FT4 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[4];     /* Indices of the coords */
	short             texture;
	short             clut;
	short             tpage;
	char              u0;
	char              v0;
	char              u1;
	char              v1;
	char              u2;
	char              v2;
	char              u3;
	char              v3;
	short             pad1;
	CVECTOR           color;
} FT4;

typedef struct G3 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[3];     /* Indices of the coords */
	short             pad1;
	CVECTOR           color[3];
} G3;

typedef struct GT3 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[3];     /* Indices of the coords */
	short             texture;
	short             clut;
	short             tpage;
	char              u0;
	char              v0;
	char              u1;
	char              v1;
	char              u2;
	char              v2;
	short             pad1;
	CVECTOR           color[3];
} GT3;

typedef struct G4 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[4];     /* Indices of the coords */
	CVECTOR           color[4];
} G4;

typedef struct GT4 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[4];     /* Indices of the coords */
	short             texture;
	short             clut;
	short             tpage;
	char              u0;
	char              v0;
	char              u1;
	char              v1;
	char              u2;
	char              v2;
	char              u3;
	char              v3;
	short             pad1;
	CVECTOR           color[4];
} GT4;

typedef struct LSF3 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[3];     /* Indices of the coords */
	short             normal;        /* Indices of the normals */
	CVECTOR           color;
} LSF3;

typedef struct LSFT3 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[3];     /* Indices of the coords */
	short             normal;        /* Indices of the normals */
	short             texture;
	short             clut;
	short             tpage;
	char              u0;
	char              v0;
	char              u1;
	char              v1;
	char              u2;
	char              v2;
	CVECTOR           color;
} LSFT3;

typedef struct LSF4 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[4];     /* Indices of the coords */
	short             normal;        /* Indices of the normals */
	short             pad1;
	CVECTOR           color;
} LSF4;

typedef struct LSFT4 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[4];     /* Indices of the coords */
	short             normal;        /* Indices of the normals */
	short             texture;
	short             clut;
	short             tpage;
	char              u0;
	char              v0;
	char              u1;
	char              v1;
	char              u2;
	char              v2;
	char              u3;
	char              v3;
	CVECTOR           color;
} LSFT4;

typedef struct LSG3 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[3];     /* Indices of the coords */
	short             normals[3];    /* Indices of the normals */
	CVECTOR           color[3];
} LSG3;

typedef struct LSGT3 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[3];     /* Indices of the coords */
	short             normals[3];    /* Indices of the normals */
	short             texture;
	short             clut;
	short             tpage;
	char              u0;
	char              v0;
	char              u1;
	char              v1;
	char              u2;
	char              v2;
	CVECTOR           color[3];
} LSGT3;

typedef struct LSG4 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[4];     /* Indices of the coords */
	short             normals[4];    /* Indices of the normals */
	CVECTOR           color[4];
} LSG4;

typedef struct LSGT4 {
	short             type;          /* Type of primitive */
	short             flag;
	short             coords[4];     /* Indices of the coords */
	short             normals[4];    /* Indices of the normals */
	short             texture;
	short             clut;
	short             tpage;
	char              u0;
	char              v0;
	char              u1;
	char              v1;
	char              u2;
	char              v2;
	char              u3;
	char              v3;
	short             pad1;
	CVECTOR           color[4];
} LSGT4;

typedef struct {
	u_long*   tag;
	CVECTOR  color0;
	DVECTOR  xy0;
	DVECTOR  xy1;
} LineF2;

typedef struct LF2 {
	short             type;          /* Type of primitive */
	short             flag;
	short             lines;         /* Number of lines */
	LineF2*           line[2];       /* 2 GPU Primitive arrays for double buffer */
} LF2;

typedef struct SPR {
	short             type;
	short             flag;
	short             coord;
	short             width;
	short             height;
	short             texture;
	CVECTOR           color;
} SPR;

typedef struct Spline {
	short             type;          /* Type of primitive */
	short             flag;
	SVECTOR           control1;
	SVECTOR           position;
	SVECTOR           control2;
	CVECTOR           color;
} Spline;

typedef struct PointLight {
	short             type;
	short             flag;
	VECTOR            position;
	CVECTOR           color;
	short             startFalloff;
	short             endFalloff;
} PointLight;

typedef struct SpotLight {
	short             type;
	short             flag;
	VECTOR            position;
	SVECTOR           direction;
	CVECTOR           color;
	short             startFalloff;
	short             endFalloff;
	short             coneAngle;
	short             spreadAngle;
} SpotLight;

typedef struct InfiniteLight {
	short             type;
	short             flag;
	SVECTOR           direction;
	CVECTOR           color;
} InfiniteLight;


/* Primitive Flags */
#define  SingleSided    0x0001
#define  ShipEngine     0x0002
#define  Translucent    0x0004

/* Primitive Types */
#define     TypeF3               1
#define     TypeFT3              2
#define     TypeF4               3
#define     TypeFT4              4
#define     TypeG3               5
#define     TypeGT3              6
#define     TypeG4               7
#define     TypeGT4              8

#define     TypeLF2              9
#define     TypeTSPR             10
#define     TypeBSPR             11

#define     TypeLSF3             12
#define     TypeLSFT3            13
#define     TypeLSF4             14
#define     TypeLSFT4            15
#define     TypeLSG3             16
#define     TypeLSGT3            17
#define     TypeLSG4             18
#define     TypeLSGT4            19

#define     TypeSpline           20

#define     TypeInfiniteLight    21
#define     TypePointLight       22
#define     TypeSpotLight        23

extern const char *type_names[];

/* Primitive Union (since we want to only hold one of it per list node) */
typedef union Prm {
	F3             *f3;
	FT3            *ft3;
	F4             *f4;
	FT4            *ft4;
	G3             *g3;
	GT3            *gt3;
	G4             *g4;
	GT4            *gt4;
	SPR            *spr;
	Spline         *spline;
	PointLight     *pointLight;
	SpotLight      *spotLight;
	InfiniteLight  *infiniteLight;
	LSF3           *lsf3;
	LSFT3          *lsft3;
	LSF4           *lsf4;
	LSFT4          *lsft4;
	LSG3           *lsg3;
	LSGT3          *lsgt3;
	LSG4           *lsg4;
	LSGT4          *lsgt4;
} Prm;

typedef struct PrimitiveNode {
	short type;
	short flag;
	Prm *primitive;
} PrimitiveNode;

typedef struct Object {
	char name[17];
	short flags;
	VECTOR origin;
	
	short num_verts;
	SVECTOR *verts;
	
	short num_normals;
	SVECTOR *normals;
	
	short num_primitives;
	PrimitiveNode *primitives;

	SVECTOR rotation;
	VECTOR position;
	VECTOR scale;
} Object;

void LoadObjectPRM(Object *object, char *filename);

#endif