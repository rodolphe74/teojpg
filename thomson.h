#ifndef THOMSON
#define THOMSON

#include "pixel.h"
#include <list.h>


static const char *BM_MODE_STRING[] = { "NONE", "BM40", "BM16", "BM80", "BM4" };


struct MAP_SEG_STRUCT {
	unsigned char	columns;
	unsigned char	lines;
	list		rama;
	list		ramb;
};
typedef struct MAP_SEG_STRUCT MAP_SEG;


static int colors_16[][3] = {
	{ 0,   0,   0	},
	{ 255, 0,   0	},
	{ 0,   255, 0	},
	{ 255, 255, 0	},
	{ 0,   0,   255 },
	{ 255, 0,   255 },
	{ 0,   255, 255 },
	{ 255, 255, 255 },
	{ 167, 167, 167 },
	{ 202, 102, 102 },
	{ 102, 202, 102 },
	{ 202, 202, 102 },
	{ 102, 102, 202 },
	{ 202, 102, 202 },
	{ 167, 247, 247 },
	{ 213, 167, 0	}
};


static int colors_8[][3] = {
	{ 0,   0,   0	},
	{ 255, 0,   0	},
	{ 0,   255, 0	},
	{ 255, 255, 0	},
	{ 0,   0,   255 },
	{ 255, 0,   255 },
	{ 0,   255, 255 },
	{ 255, 255, 255 },
};


struct MACHINE_CONF_STRUCT {
	int	colors_count;
	int (*palette)[3];
	int	has_4096_colors;
	int	commut_address;
	int	start_screen_address;
	int	modes[5]; // BM_MODE_STRING
};
typedef struct MACHINE_CONF_STRUCT MACHINE_CONF;

static MACHINE_CONF TO9_CONF = {
	.colors_count		= 16,
	.palette		= NULL,
	.has_4096_colors	= 1,
	.commut_address		= 0xE7C3,
	.start_screen_address	= 0x4000,
	.modes			= { 1, 2, 3, 4 }
};

static MACHINE_CONF TO9_PLUS_CONF = {
	.colors_count		= 16,
	.palette		= NULL,
	.has_4096_colors	= 1,
	.commut_address		= 0xE7C3,
	.start_screen_address	= 0x4000,
	.modes			= { 1, 2, 3, 4 }
};

static MACHINE_CONF TO8_CONF = {
	.colors_count		= 16,
	.palette		= NULL,
	.has_4096_colors	= 1,
	.commut_address		= 0xE7C3,
	.start_screen_address	= 0x4000,
	.modes			= { 1, 2, 3, 4 }
};

static MACHINE_CONF MO6_CONF = {
	.colors_count		= 16,
	.palette		= NULL,
	.has_4096_colors	= 1,
	.commut_address		= 0xA7C0,
	.start_screen_address	= 0x0000,
	.modes			= { 1, 2, 3, 4 }
};

static MACHINE_CONF MO5_CONF = {
	.colors_count		= 16,
	.palette		= colors_16,
	.has_4096_colors	= 0,
	.commut_address		= 0xA7C0,
	.start_screen_address	= 0x0000,
	.modes			= { 1 }
};

static MACHINE_CONF TO7_70_CONF = {
	.colors_count		= 16,
	.palette		= colors_16,
	.has_4096_colors	= 0,
	.commut_address		= 0xE7C3,
	.start_screen_address	= 0x4000,
	.modes			= { 1 }
};

static MACHINE_CONF TO7_CONF = {
	.colors_count		= 8,
	.palette		= colors_8,
	.has_4096_colors	= 0,
	.commut_address		= 0xE7C3,
	.start_screen_address	= 0x4000,
	.modes			= { 1 }
};




//------------------------------------------------------------------------------
// Initialisation des teintes thomson en rvb PC
// Le résultat est placé dans thomson_rvb
//------------------------------------------------------------------------------
extern unsigned int thomson_rvb[4096];
void init_thomson_rvb();
extern float thomson_levels[16];
extern int linear_to_to[256];
extern int rgb_to_to[256];
void init_thomson_linear_levels();


//------------------------------------------------------------------------------
// Table globale contenant la correction gamma pc thomson
//------------------------------------------------------------------------------
static int thomson_level_pc[16] = { 0, 100, 127, 142, 163, 179, 191, 203, 215, 223, 231, 239, 243, 247, 251, 255 };
extern float thomson_linear_levels[16];
void init_thomson_linear_levels();

//------------------------------------------------------------------------------
// Post traitement palette thomson
//------------------------------------------------------------------------------
void thomson_post_trt_palette(PALETTE *src, PALETTE *target);

//------------------------------------------------------------------------------
// Traitement thomson 320x200 16 couleurs (couleur unique sur bloc 8 pixels)
// Image retournee doit etre liberee par l'appelant
//------------------------------------------------------------------------------
unsigned char *thomson_post_trt_combin(IMAGE *source, PALETTE *palette, MAP_SEG *map_40, MACHINE_CONF *conf, char *target_name);


//------------------------------------------------------------------------------
// Génération basic pour le mode 160x200 16 couleurs
//------------------------------------------------------------------------------
void save_bm16_basic(IMAGE *source, unsigned char *pixels, PALETTE *palette, char *target_name);



//------------------------------------------------------------------------------
// Initialisation et destruction structure
//------------------------------------------------------------------------------
void init_map_seg(MAP_SEG *map_seg);
void free_map_seg(MAP_SEG *map_seg);


//------------------------------------------------------------------------------
// Sauvegarde au format thomson 40 cols
// Les map sont générés par la fonction thomson_post_trt_combin
//------------------------------------------------------------------------------
void save_map_40_col(char *filename, MAP_SEG *map_40, PALETTE *palette);


//------------------------------------------------------------------------------
// Sauvegarde au format thomson 16 couleurs 160x200
//------------------------------------------------------------------------------
void save_map_16(char *filename, MAP_SEG *map_16, PALETTE *palette, int x_count);

#endif
