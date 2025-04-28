#include "../../include/MAP/perlinNoise.h"

static void MAP_perlinNoise1D(int count, float *fSeed, int octaves,float bias, float *output);
static void MAP_perlinNoise2D(int width, int height, float *fSeed, int octaves,float bias, float *output);


void MAP_generatePerlinNoise(int output[][MAP_WIDTH], int height, int width, int range, int offset){
    float fOutput[height * width];
    MAP_generate2DNoise(fOutput,height,width);
    range--;
    for (int y = 0; y < height; y++){
        for(int x = 0;x < width; x++){
            int idx = y * width + x;
            output[y][x] = (int)(roundf(fOutput[idx]*range) + offset);
        }
    }
}

void MAP_generate2DNoise(float *output,int outputHeight, int outputWidth){
    float *fNoiseSeed2D = malloc((outputHeight * outputWidth) * sizeof *fNoiseSeed2D);
    for (int i = 0; i < outputHeight * outputWidth; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
    MAP_perlinNoise2D(outputWidth, outputHeight, fNoiseSeed2D, 6, 2.0f, output);
    free(fNoiseSeed2D);
}


void MAP_generat1DNoise(float *output, int size){
    float *fNoiseSeed1D = malloc(size * sizeof *fNoiseSeed1D);;
    for(int i = 0;i < size;i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;
    MAP_perlinNoise1D(size ,fNoiseSeed1D ,6 ,2.0f ,output);
    free(fNoiseSeed1D);
}

static void MAP_perlinNoise1D(int count, float *fSeed, int octaves,float bias, float *output){
    for (int i = 0; i < count; i++){
        float noise = 0.0f;
        float scale = 1.0f;
        float scaleAcc = 0.0f;
        for (int j = 0; j < octaves; j++){
            int pitch = count >> 2;
            if (pitch == 0) continue; 
            int sample1 = (i / pitch) * pitch;
            int sample2 = (sample1 + pitch) % count;

            float blend = (float)(i - sample1) / (float)pitch;
            //linier interpalation
            float sample = (1.0f - blend) * fSeed[sample1] + blend * fSeed[sample2];
            noise += sample * scale;
            scaleAcc += scale;
            scale = scale/ bias;
        }
        output[i] = noise / scaleAcc;
    }
    
}

static void MAP_perlinNoise2D(int width, int height, float *fSeed, int octaves,float bias, float *output){
    for (int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            float noise = 0.0f;
            float scale = 1.0f;
            float scaleAcc = 0.0f;

            for (int j = 0; j < octaves; j++){
                int pitch = width >> 2;
                if (pitch == 0) continue; 
                int sampleX1 = (x / pitch) * pitch;
                int sampleY1 = (y / pitch) * pitch;

                int sampleX2 = (sampleX1 + pitch) % width;
                int sampleY2 = (sampleY1 + pitch) % width;

                float blendX = (float)(x - sampleX1) / (float)pitch;
                float blendY = (float)(y - sampleY1) / (float)pitch;
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

// int main(){
//     srand(time(NULL));
//     int output[MAP_HEIGHT][MAP_WIDTH] = {0};
//     MAP_generatePerlinNoise(output,MAP_HEIGHT,MAP_WIDTH,13,0);
//     for (int y = 0; y < MAP_HEIGHT; y++){
//         for(int x = 0;x < MAP_WIDTH; x++){
//             printf("%d,",output[y][x]);
//         }printf("\n");
//     }
//     return 0;
// }