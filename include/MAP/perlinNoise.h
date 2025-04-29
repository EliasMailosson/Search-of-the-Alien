#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H
#include <math.h>
#include <stdlib.h>
#include <stdio.h>  
#include <time.h>
#include <inttypes.h>

#define MAP_WIDTH 60
#define MAP_HEIGHT 60

void MAP_generat1DNoise(float *output, int size);
void MAP_generate2DNoise(float *output,int outputHeight, int outputWidth,uint32_t seed);


void MAP_generatePerlinNoise(int output[][MAP_WIDTH], int height, int width, int range, int offset, uint32_t seed);


float MAP_converRandToFloat(int x, int y, uint32_t seed);
uint32_t MAP_randOnSeed2D(int x, int y, uint32_t seed);
uint32_t MAP_generate_seed();

#endif