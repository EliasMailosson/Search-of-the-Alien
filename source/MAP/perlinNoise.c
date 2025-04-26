#include "../../include/MAP/perlinNoise.h"

static void MAP_perlinNoise1D(int count, float *fSeed, int octaves,float bias, float *output);


void MAP_generatePerlinNoise(){

}

void MAP_generate2DNoise(float *output,int outputHeight, int outputWidth){
    float *fNoiseSeed2D = {0};
    float *perlinNoise2D = {0};
    for (int i = 0; i < outputHeight * outputWidth; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;

}


void MAP_generat1DNoise(float *output, int size){
    float *fNoiseSeed1D = malloc(size * sizeof *fNoiseSeed1D);;
    for(int i = 0;i < size;i++) fNoiseSeed1D[i] = (float)rand() / (float)RAND_MAX;
    MAP_perlinNoise1D(size ,fNoiseSeed1D ,3 ,2.0f ,output);
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

// int main(){
//     srand(time(NULL));
//     float output[MAP_HEIGHT] = {0};
//     MAP_generat1DNoise(output,MAP_HEIGHT);
//     for (int i = 0; i < MAP_HEIGHT; i++){
//         printf("%.1f ",output[i]*3);
//     }
//     return 0;
// }