#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "colourconverter.h"

//--------------------------------------------------------
// FUNCTION PROTOTYPES
//--------------------------------------------------------
static int indexofchar(char *str, char c);

static int calculate_hue(RGBdec rgbd);
static double calculate_saturation(RGBdec rgbd, double lightness);
static double calculate_lightness(RGBdec rgbd);

//--------------------------------------------------------
// FUNCTION showRGBdec
//--------------------------------------------------------
void showRGBdec(RGBdec rgbd)
{
	printf("rgb(%d, %d, %d)\n", rgbd.R, rgbd.G, rgbd.B);
}

//--------------------------------------------------------
// FUNCTION showRGBhex
//--------------------------------------------------------
void showRGBhex(RGBhex rgbh)
{
	printf("#%s%s%s\n", rgbh.R, rgbh.G, rgbh.B);
}

//--------------------------------------------------------
// FUNCTION showHSL
//--------------------------------------------------------
void showHSL(HSL hsl)
{
	printf("hsl(%d, %4.2lf%%, %4.2lf%%)\n", hsl.H, hsl.S, hsl.L);
}

//--------------------------------------------------------
// FUNCTION validhex - checks character is 0-9, A-F or a-f
//--------------------------------------------------------
static bool validhex(char c)
{
	if ((c < 48 || c > 57) && (c < 65 || c > 70) && (c < 97 || c > 102))
		return false;
	else
		return true;
}

//--------------------------------------------------------
// FUNCTION validateRGBhex
//--------------------------------------------------------
bool validateRGBhex(RGBhex rgbh)
{
	//R, G and B must each have exactly 2 chars
	if (strlen(rgbh.R) != 2 || strlen(rgbh.G) != 2 || strlen(rgbh.B) != 2)
		return false;

	//combine into one string for easier iterating
	char combined[7] = "";

	combinehex(rgbh, combined);

	//iterate combined, checking each character is 0-9, A-F or A-F
	for (int c = 0; c <= 5; c++)
		if (!validhex(combined[c]))
			return false;

	return true;
}

//--------------------------------------------------------
// FUNCTION validateHSL
//--------------------------------------------------------
bool validateHSL(HSL hsl)
{
	//hue - 0-360
	if (hsl.H < 0 || hsl.H > 360)
		return false;

	//saturation - 0-100
	if (hsl.S < 0 || hsl.S > 100)
		return false;

	//lightness - 0-100
	if (hsl.L < 0 || hsl.L > 100)
		return false;

	return true;
}

//--------------------------------------------------------
// FUNCTION combinehex
// write R, G & B to single string
//--------------------------------------------------------
void combinehex(RGBhex rgbh, char *combined)
{
	strcat(combined, rgbh.R);
	strcat(combined, rgbh.G);
	strcat(combined, rgbh.B);
}

//--------------------------------------------------------
// FUNCTION splithex
// populate RGBhex values from single string
//--------------------------------------------------------
bool splithex(char *hexcolour, RGBhex *rgbh)
{
	//must have 6 chars
	if (strlen(hexcolour) != 6) {
		rgbh = NULL;

		return false;
	}

	//check each char valid hex
	for (int c = 0; c <= 5; c++) {
		if (!validhex(hexcolour[c])) {
			rgbh = NULL;

			return false;
		}
	}

	memcpy(rgbh->R, hexcolour + 0, 2);
	rgbh->R[2] = '\0';
	memcpy(rgbh->G, hexcolour + 2, 2);
	rgbh->G[2] = '\0';
	memcpy(rgbh->B, hexcolour + 4, 2);
	rgbh->B[2] = '\0';

	return true;
}

//--------------------------------------------------------
// FUNCTION hex2dec
//--------------------------------------------------------
void hex2dec(RGBhex rgbh, RGBdec *rgbd)
{
	//array of valid hexadecimal digits, indexes correspond to values
	char hexvalues[] = "0123456789ABCDEF";

	rgbd->R = indexofchar(hexvalues, toupper(rgbh.R[1])) + (indexofchar(hexvalues, toupper(rgbh.R[0])) * 16);
	rgbd->G = indexofchar(hexvalues, toupper(rgbh.G[1])) + (indexofchar(hexvalues, toupper(rgbh.G[0])) * 16);
	rgbd->B = indexofchar(hexvalues, toupper(rgbh.B[1])) + (indexofchar(hexvalues, toupper(rgbh.B[0])) * 16);
}

//--------------------------------------------------------
// FUNCTION dec2hex
//--------------------------------------------------------
void dec2hex(RGBdec rgbd, RGBhex *rgbh)
{
	char hexvalues[] = "0123456789ABCDEF";
	int i0;
	int i1;

	i0 = rgbd.R / 16;
	i1 = rgbd.R - (16 * i0);
	rgbh->R[0] = hexvalues[i0];
	rgbh->R[1] = hexvalues[i1];
	rgbh->R[2] = '\0';

	i0 = rgbd.G / 16;
	i1 = rgbd.G - (16 * i0);
	rgbh->G[0] = hexvalues[i0];
	rgbh->G[1] = hexvalues[i1];
	rgbh->G[2] = '\0';

	i0 = rgbd.B / 16;
	i1 = rgbd.B - (16 * i0);
	rgbh->B[0] = hexvalues[i0];
	rgbh->B[1] = hexvalues[i1];
	rgbh->B[2] = '\0';
}

//--------------------------------------------------------
// FUNCTION dec2hsl
//--------------------------------------------------------
void dec2hsl(RGBdec rgbd, HSL *hsl)
{
	hsl->H = (int)calculate_hue(rgbd);
	hsl->L = calculate_lightness(rgbd);
	hsl->S = calculate_saturation(rgbd, hsl->L);
}

//--------------------------------------------------------
// FUNCTION hex2hsl
//--------------------------------------------------------
void hex2hsl(RGBhex rgbh, HSL *hsl)
{
	RGBdec rgbd;

	hex2dec(rgbh, &rgbd);

	dec2hsl(rgbd, hsl);
}

//--------------------------------------------------------
// FUNCTION hsl2dec
//--------------------------------------------------------
void hsl2dec(HSL hsl, RGBdec *rgbd)
{
	hsl.L /= 100.0;
	hsl.S /= 100.0;

	if (hsl.S == 0) {
		rgbd->R = hsl.L * 255.0;
		rgbd->G = hsl.L * 255.0;
		rgbd->B = hsl.L * 255.0;
	} else {
		//temporary_1
		double temporary_1;

		if (hsl.L < 0.5)
			temporary_1 = hsl.L * (1.0 + hsl.S);
		else
			temporary_1 = hsl.L + hsl.S - hsl.L * hsl.S;

		//temporary_2
		double temporary_2 = 2.0 * hsl.L - temporary_1;

		//hue
		double hue = hsl.H / 360.0;

		//temporary_R,  temporary_G and temporary_B
		double temporary_R = hue + 0.333;
		double temporary_G = hue;
		double temporary_B = hue - 0.333;

		//ensure temporary_R,  temporary_G and temporary_B are between 0 and 1
		if (temporary_R < 0.0)
			temporary_R += 1.0;
		if (temporary_R > 1.0)
			temporary_R -= 1.0;

		if (temporary_G < 0.0)
			temporary_G += 1.0;
		if (temporary_G > 1.0)
			temporary_G -= 1.0;

		if (temporary_B < 0.0)
			temporary_B += 1.0;
		if (temporary_B > 1.0)
			temporary_B -= 1.0;

		//RED
		if ((6.0 * temporary_R) < 1.0)
			rgbd->R = round((temporary_2 + (temporary_1 - temporary_2) * 6.0 * temporary_R) * 255.0);
		else if ((2.0 * temporary_R) < 1.0)
			rgbd->R = round(temporary_1 * 255.0);
		else if ((3.0 * temporary_R) < 2.0)
			rgbd->R = round((temporary_2 + (temporary_1 - temporary_2) * (0.666 - temporary_R) * 6.0) * 255.0);
		else
			rgbd->R = round(temporary_2 * 255.0);

		//GREEN
		if ((6.0 * temporary_G) < 1.0)
			rgbd->G = round((temporary_2 + (temporary_1 - temporary_2) * 6.0 * temporary_G) * 255.0);
		else if ((2.0 * temporary_G) < 1.0)
			rgbd->G = round(temporary_1 * 255.0);
		else if ((3.0 * temporary_G) < 2.0)
			rgbd->G = round((temporary_2 + (temporary_1 - temporary_2) * (0.666 - temporary_G) * 6.0) * 255.0);
		else
			rgbd->G = round(temporary_2 * 255.0);

		//BLUE
		if ((6.0 * temporary_B) < 1.0)
			rgbd->B = round((temporary_2 + (temporary_1 - temporary_2) * 6.0 * temporary_B) * 255.0);
		else if ((2.0 * temporary_B) < 1.0)
			rgbd->B = round(temporary_1 * 255.0);
		else if ((3.0 * temporary_B) < 2.0)
			rgbd->B = round((temporary_2 + (temporary_1 - temporary_2) * (0.666 - temporary_B) * 6.0) * 255.0);
		else
			rgbd->B = round(temporary_2 * 255.0);
	}
}

//--------------------------------------------------------
// FUNCTION hsl2hex
//--------------------------------------------------------
void hsl2hex(HSL hsl, RGBhex *rgbh)
{
	RGBdec rgbd;

	hsl2dec(hsl, &rgbd);

	dec2hex(rgbd, rgbh);
}

//--------------------------------------------------------
// FUNCTION indexofchar
//--------------------------------------------------------
static int indexofchar(char *str, char c)
{
	char *i = strchr(str, c);

	return i != NULL ? i - str : -1;
}

//---------------------------------------------------------------------------------
// FUNCTION calculate_hue
// Hue is 0 if grey (Max == Min)
//---------------------------------------------------------------------------------
static int calculate_hue(RGBdec rgbd)
{
	double max = 0;
	double min = 0;

	double dR;
	double dG;
	double dB;

	dR = (double)rgbd.R / 255.0;
	dG = (double)rgbd.G / 255.0;
	dB = (double)rgbd.B / 255.0;

	if (dR >= dG && dR >= dB)
		max = dR;
	else if (dG >= dB && dG >= dR)
		max = dG;
	else if (dB >= dG && dB >= dR)
		max = dB;

	if (dR <= dG && dR <= dB)
		min = dR;
	else if (dG <= dB && dG <= dR)
		min = dG;
	else if (dB <= dG && dB <= dR)
		min = dB;

	double hue;

	if (max == min) {
		hue = 0;
	} else {
		if (max == dR)
			hue = (dG - dB) / (max - min);
		else if (max == dG)
			hue = 2.0 + (dB - dR) / (max - min);
		else if (max == dB)
			hue = 4.0 + (dR - dG) / (max - min);

		hue *= 60.0;

		if (hue < 0.0)
			hue += 360.0;
	}

	return hue;
}

//---------------------------------------------------------------------------------
// FUNCTION calculate_saturation
// 0 - 100
//---------------------------------------------------------------------------------
static double calculate_saturation(RGBdec rgbd, double lightness)
{
	double max = 0;
	double min = 0;

	double dR;
	double dG;
	double dB;

	dR = (double)rgbd.R / 255.0;
	dG = (double)rgbd.G / 255.0;
	dB = (double)rgbd.B / 255.0;

	if (dR >= dG && dR >= dB)
		max = dR;
	else if (dG >= dB && dG >= dR)
		max = dG;
	else if (dB >= dG && dB >= dR)
		max = dB;

	if (dR <= dG && dR <= dB)
		min = dR;
	else if (dG <= dB && dG <= dR)
		min = dG;
	else if (dB <= dG && dB <= dR)
		min = dB;

	double saturation;

	if (max == min) {
		saturation = 0.0;
	} else {
		if (lightness < 50.0)
			saturation = ((max - min) / (max + min)) * 100.0;
		else
			saturation = ((max - min) / (2.0 - max - min)) * 100.0;
	}

	return saturation;
}

//---------------------------------------------------------------------------------
// FUNCTION calculate_lightness
// 0 - 100
//---------------------------------------------------------------------------------
static double calculate_lightness(RGBdec rgbd)
{
	double max = 0;
	double min = 0;

	double dR;
	double dG;
	double dB;

	dR = (double)rgbd.R / 255.0;
	dG = (double)rgbd.G / 255.0;
	dB = (double)rgbd.B / 255.0;

	if (dR >= dG && dR >= dB)
		max = dR;
	else if (dG >= dB && dG >= dR)
		max = dG;
	else if (dB >= dG && dB >= dR)
		max = dB;

	if (dR <= dG && dR <= dB)
		min = dR;
	else if (dG <= dB && dG <= dR)
		min = dG;
	else if (dB <= dG && dB <= dR)
		min = dB;

	double lightness = ((min + max) / 2.0) * 100.0;

	return lightness;
}
