#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H
#include "../../include/MAP/map.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>  
#include <time.h>

void MAP_generat1DNoise(float *output, int size);
void MAP_generate2DNoise(float *output,int outputHeight, int outputWidth);

void MAP_generatePerlinNoise(int output[][MAP_WIDTH], int height, int width, int range, int offset);


#endif