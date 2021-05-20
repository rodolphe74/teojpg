#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

// --------------------------------------------------------
// STRUCTS
// --------------------------------------------------------
typedef struct {
	unsigned char	R;
	unsigned char	G;
	unsigned char	B;
} RGBdec;

typedef struct {
	char	R[3];
	char	G[3];
	char	B[3];
} RGBhex;

typedef struct {
	int	H;
	double	S;
	double	L;
} HSL;

// --------------------------------------------------------
// FUNCTION PROTOTYPES
// --------------------------------------------------------
bool validateRGBhex(RGBhex rgbh);
bool validateHSL(HSL hsl);

bool splithex(char *hexcolour, RGBhex *rgbh);
void combinehex(RGBhex rgbh, char *combined);

void hex2dec(RGBhex rgbh, RGBdec *rgbd);
void hex2hsl(RGBhex rgbh, HSL *hsl);
void dec2hex(RGBdec rgbd, RGBhex *rgbh);
void dec2hsl(RGBdec rgbd, HSL *hsl);
void hsl2dec(HSL hsl, RGBdec *rgbd);
void hsl2hex(HSL hsl, RGBhex *rgbh);

void showRGBdec(RGBdec rgbd);
void showRGBhex(RGBhex rgbh);
void showHSL(HSL hsl);
