#include "Plan.h"

double baseline = 500.0;
double spacing_Y = 35;
double spacing_X = spacing_Y * 2 * 0.85;

TColor Violett = RGBSkala(100.0,75.3,79.6),
TColor Orange = RGBSkala(100.0,64.7,0.0),
TColor Braun = RGBSkala(60.0,39.6,8.2),

double kugeln_X[16] =  {
	-635.0,
	baseline,
	baseline + spacing_X,
	baseline + spacing_X*2,
	baseline + spacing_X*3,
	baseline + spacing_X*3,
	baseline + spacing_X*4,
	baseline + spacing_X*4,
	baseline + spacing_X*2,
	baseline + spacing_X,
	baseline + spacing_X*2,
	baseline + spacing_X*3,
	baseline + spacing_X*3,
	baseline + spacing_X*4,
	baseline + spacing_X*4,
	baseline + spacing_X*4
};

double kugeln_Y[16]  = {
	spacing_Y * 0,
	spacing_Y * 0,
	- spacing_Y,
	- spacing_Y * 2,
	- spacing_Y,
	- spacing_Y * 3,
	spacing_Y * 2,
	spacing_Y * 4,
	spacing_Y * 0,
	spacing_Y,
	spacing_Y * 2,
	spacing_Y * 1,
	spacing_Y * 3,
	spacing_Y * 0,
	- spacing_Y * 2,
	- spacing_Y * 4
};

TColor colors[16] = {
	Weiss,
	Gelb,
	Blau,
	Hellrot,
	Violett,
	Orange,
	Gruen,
	Braun,
	Schwarz,
	Gelb,
	Blau,
	Hellrot,
	Violett,
	Orange,
	Gruen,
	Braun
};


double loecher_X[6] =  {-1210.0 , -1210.0 , 1210.0 , 1210.0 ,    0.0 ,   0.0};
double loecher_Y[6] =  {-575.0  , 575.0   , -575.0 , 575.0  , -575.0 , 575.0};
