#include "../../include/MAP/perlinNoise.h"

//static void MAP_perlinNoise1D(int count, float *fSeed, int octaves,float bias, float *output);
static void MAP_perlinNoise2D(int width, int height, float *fSeed, int octaves,float bias, float *output);


void MAP_generatePerlinNoise(int output[][MAP_WIDTH], int height, int width, int range, int offset, uint32_t seed){
    float fOutput[height * width];
    MAP_generate2DNoise(fOutput,height,width,seed);
    range--;
    //printf("h %d w %d\n\n\n",height,width);
    for (int y = 0; y < height; y++){
        for(int x = 0;x < width; x++){
            int idx = y * width + x;
            output[y][x] = (int)(roundf(fOutput[idx] * range) + offset);
        }
    }

}

void MAP_generate2DNoise(float *output,int outputHeight, int outputWidth,uint32_t seed){
    float *fNoiseSeed2D = malloc((outputHeight * outputWidth) * sizeof *fNoiseSeed2D);
    for (int y = 0; y < outputHeight; y++){
        for(int x = 0; x< outputWidth; x++){
            int idx = y * outputWidth + x;
            fNoiseSeed2D[idx] = MAP_converRandToFloat(x, y, seed);
        }
    }
    MAP_perlinNoise2D(outputWidth, outputHeight, fNoiseSeed2D, OCTAVES, BIAS, output);
    free(fNoiseSeed2D);
}

// void MAP_generat1DNoise(float *output, int size){
//     float *fNoiseSeed1D = malloc(size * sizeof *fNoiseSeed1D);;
//     for(int i = 0;i < size;i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;
//     MAP_perlinNoise1D(size ,fNoiseSeed1D ,6 ,2.0f ,output);
//     free(fNoiseSeed1D);
// }

// static void MAP_perlinNoise1D(int count, float *fSeed, int octaves,float bias, float *output){
//     for (int i = 0; i < count; i++){
//         float noise = 0.0f;
//         float scale = 1.0f;
//         float scaleAcc = 0.0f;
        
//         for (int j = 0; j < octaves; j++){
//             int pitch = count >> 2;
//             if (pitch == 0) continue; 
//             int sample1 = (i / pitch) * pitch;
//             int sample2 = (sample1 + pitch) % count;

//             float blend = (float)(i - sample1) / (float)pitch;
//             //linier interpalation
//             float sample = (1.0f - blend) * fSeed[sample1] + blend * fSeed[sample2];
//             noise += sample * scale;
//             scaleAcc += scale;
//             scale = scale/ bias;
//         }
//         output[i] = noise / scaleAcc;
//     }
    
// }

static void MAP_perlinNoise2D(int width, int height, float *fSeed, int octaves,float bias, float *output){
    for (int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            float noise = 0.0f;
            float scale = 1.0f;
            float scaleAcc = 0.0f;

            for (int j = 0; j < octaves; j++){
            int pitchX = width  >> j;
            int pitchY = height >> j;
            if (!pitchX || !pitchY) continue;

            int sampleX1 = (x / pitchX) * pitchX;
            int sampleY1 = (y / pitchY) * pitchY;
            int sampleX2 = (sampleX1 + pitchX) % width;
            int sampleY2 = (sampleY1 + pitchY) % height;

            float blendX = (float)(x - sampleX1) / pitchX;
            float blendY = (float)(y - sampleY1) / pitchY;
                //linier interpalation
                float sampleT = (1.0f - blendX) * fSeed[sampleY1 * width + sampleX1] + blendX * fSeed[sampleY1 * width + sampleX2];
                float sampleB = (1.0f - blendX) * fSeed[sampleY2 * width + sampleX1] + blendX * fSeed[sampleY2 * width + sampleX2];
                noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
                scaleAcc += scale;
                scale = scale/ bias;
            }
            output[y * width + x] = noise / scaleAcc;
        }
    }
    
}

uint32_t MAP_randOnSeed2D(int x, int y, uint32_t seed){
    uint32_t h = seed ^ (x * 374761393u) ^ (y * 668265263u);
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

float MAP_converRandToFloat(int x, int y, uint32_t seed){
    return (MAP_randOnSeed2D(x, y, seed) & 0xFFFFFF) / (float)0xFFFFFF;
}

uint32_t MAP_generate_seed(){
    uintptr_t ptr_entropy = (uintptr_t)&ptr_entropy;
    return (uint32_t)(time(NULL) ^ ptr_entropy);
}