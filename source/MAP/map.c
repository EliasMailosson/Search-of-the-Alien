#include "../../include/MAP/map.h"

static const int lobbyLUT[MAX_LUT_LEN] ={/* lägg till lobby tiles*/};
static const int nemurLUT[MAX_LUT_LEN] ={1,10,11,12,13,38,37,2,3,4,5,35,34};
static const int auranticLUT[MAX_LUT_LEN] ={/* lägg till tiles*/};
static const int cindoraLUT[MAX_LUT_LEN] ={/* lägg till tiles*/};

struct Map{
    SDL_Texture *texture;
    SDL_Rect tileIndex[MAX_COUNT_SPRITE_TILES];
    int tileID[MAP_HEIGHT][MAP_WIDTH];
    int lobbyTileID[LOBBY_HEIGHT][LOBBY_WIDTH];
    SDL_Rect tileRect; 
    SDL_Rect baseRect;
    PlanetLUT planet;
    uint32_t seed;
};

static void substring(char *buffer, int start, int end, char* result);
static void MAP_StrTrimWhitespace(char *str);
void printMap(Map aMap);
static void MAP_TileSheetload(SDL_Renderer* pRend, char *imagePath, Map aMap);

PlanetLUT MAP_mapGetPlanetLUT(Map aMap){
    return aMap->planet;
}

void MAP_convertTiles(int tileID[MAP_HEIGHT][MAP_WIDTH], PlanetLUT plantet){
    for(int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            int index = tileID[y][x];
            switch (plantet){
            case LOBBY_LUT:
                tileID[y][x] = lobbyLUT[index];
                break;
            case NEMUR_LUT:
                tileID[y][x] = nemurLUT[index];
                break;
            case AURANTIC_LUT:
                tileID[y][x] = auranticLUT[index];
                break;
            case CINDORA_LUT:
                tileID[y][x] = cindoraLUT[index];
                break;
            default:
                tileID[y][x] = 0;
                break;
            }
        }
    }
}
void MAP_mapNewMap(Map aMap,uint32_t seed){
    MAP_generatePerlinNoise(aMap->tileID,MAP_HEIGHT,MAP_WIDTH,RANGE,MAP_OFFSET,seed);
    MAP_convertTiles(aMap->tileID,aMap->planet);
    MAP_mapSetEdgesToZero(aMap->tileID);
}

void MAP_mapSetEdgesToZero(int tileID[][MAP_WIDTH]){
    for (int i = 0; i < MAP_WIDTH; i++){
        tileID[0][i] = 0;
        tileID[MAP_HEIGHT-1][i] = 0;
    }
    for (int i = 0; i < MAP_HEIGHT; i++){
        tileID[i][0] = 0;
        tileID[i][MAP_WIDTH - 1] = 0;
    }
}

void MAP_RenderTileLobby(SDL_Renderer *pRend, Map aMap, int y, int x, SDL_Rect *currentRect){
    int id = aMap->lobbyTileID[y][x]; 
    if(id <= 0 || id >= MAX_COUNT_SPRITE_TILES) return;
    SDL_RenderCopy(pRend, aMap->texture, &aMap->tileIndex[id], currentRect);
}

void MAP_MapRenderLobby(SDL_Renderer *pRend, Map aMap){
    int tileW = aMap->tileRect.w;
    int tileH = aMap->tileRect.h;

    SDL_Rect currentRect;
    currentRect.w = tileW;
    currentRect.h = tileH;

    for(int y = 0; y < LOBBY_HEIGHT; y++){ 
        for(int x = 0; x < LOBBY_WIDTH; x++){
            
            currentRect.x = (int)roundf((x - y) * (tileW * 0.5f)
                                        + aMap->tileRect.x);
            currentRect.y = (int)roundf((x + y) * (tileH * 0.25f)
                                        + aMap->tileRect.y);
            MAP_RenderTileLobby(pRend, aMap, y, x, &currentRect);
        }
    }
}

void MAP_MapNewRender(SDL_Renderer* rend, Map map, SDL_Window* win){
    int winW, winH;
    SDL_GetWindowSize(win, &winW, &winH);

    const int tileW     = map->tileRect.w;
    const int tileH     = map->tileRect.h;
    const int halfW     = tileW / 2;
    const int quarterH  = tileH / 4;
    const int offsetX   = map->tileRect.x;
    const int offsetY   = map->tileRect.y;

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        int edgeL = (int)ceil(( - tileW - offsetX + y*halfW) / (float)halfW);
        int edgeR = (int)floor(( winW - offsetX + y*halfW) / (float)halfW);

        if (edgeL < 0) edgeL = 0;
        if (edgeR >= MAP_WIDTH) edgeR = MAP_WIDTH - 1;
        if (edgeL > edgeR) continue;
        for (int x = edgeL; x <= edgeR; ++x) {
            int screenX = (x - y)*halfW + offsetX;
            int screenY = (x + y)*quarterH + offsetY;
            if (screenY + tileH  < 0 || screenY > winH) continue;
            SDL_Rect dst = { screenX, screenY, tileW, tileH };
            MAP_TileRender(rend, map, y, x, &dst);
        }
    }
}

void MAP_TileRender(SDL_Renderer *pRend, Map aMap, int y, int x, SDL_Rect *currentRect){
    int id = aMap->tileID[y][x]; 
    if(id <= 0 || id >= MAX_COUNT_SPRITE_TILES) return;
    SDL_RenderCopy(pRend, aMap->texture, &aMap->tileIndex[id], currentRect);
}

Map MAP_MapCreate(SDL_Renderer *pRend, int winW, int winH){
    Map aMap = malloc(sizeof(struct Map));
    if(aMap == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }
    MAP_TileSheetload(pRend, FILE_PHAT_LOBBY_SPRITE, aMap);
    int n = 0;
    for (int y = 0; y < 4; y++){
        for (int x = 0; x < 10; x++){
            aMap->tileIndex[n++] = (SDL_Rect){.x = (TILE_SPRITE_SIZE*x), .y = (TILE_SPRITE_SIZE*y), .w = TILE_SPRITE_SIZE, .h = TILE_SPRITE_SIZE};
        }
    }
    MAP_TilesFillWithBlank(aMap->tileID);
    MAP_MapGetTilesFromLobby(aMap->lobbyTileID);
    //MAP_convertTiles(aMap->tileID,NEMUR_LUT);
    MAP_mapSetEdgesToZero(aMap->tileID);

    SDL_Rect startRect = {
        .h = TILE_SIZE,
        .w = TILE_SIZE,
        .x = 0,
        .y = 0,
    };
    aMap->baseRect = startRect;
    aMap->tileRect = startRect;
    MAP_MapRefresh(aMap,winW,winH);
    // aMap->tileID[0][0] = 1;
    return aMap;
}

void MAP_MapRefresh(Map aMap, int winW, int winH){
    float scaleX = (float)winW / LOGICAL_WIN_W;
    float scaleY = (float)winH / LOGICAL_WIN_H;
    float scale  = (scaleX < scaleY) ? scaleX : scaleY;
    int scaledW = (int)roundf(LOGICAL_WIN_W * scale);
    int scaledH = (int)roundf(LOGICAL_WIN_H * scale);

    int offsetX = (int)roundf((winW - scaledW) * 0.5f);
    int offsetY = (int)roundf((winH - scaledH) * 0.5f);
    SDL_Rect base = aMap->baseRect;
    aMap->tileRect.x = offsetX + (int)roundf(base.x * scale);
    aMap->tileRect.y = offsetY + (int)roundf(base.y * scale);
    aMap->tileRect.w = (int)roundf(base.w * scale);
    aMap->tileRect.h = (int)roundf(base.h * scale);
}

static void MAP_TileSheetload(SDL_Renderer* pRend, char *imagePath, Map aMap){
    SDL_Surface *surface = IMG_Load(imagePath);
    aMap->texture = SDL_CreateTextureFromSurface(pRend, surface);
    SDL_FreeSurface(surface);
    if(!aMap->texture){
        printf("Error creating texture for map %s\n", IMG_GetError());
    }
}

void MAP_MapDestroy(Map aMap){
    SDL_DestroyTexture(aMap->texture);
    aMap->texture = NULL;
    free(aMap);
    aMap = NULL;
}

void MAP_MapGetTilesFromLobby(int lobbyTileID[LOBBY_HEIGHT][LOBBY_WIDTH]){
    char buffer[256];
    FILE *fp = fopen(FILE_PHAT_LOBBY_DATA, "r");
    if (fp == NULL) {
        fprintf(stderr,"Error: Could not open %s!\n", FILE_PHAT_LOBBY_DATA);
        return;
    }
    int y = 0;
    while (fgets(buffer, sizeof(buffer), fp) && y < LOBBY_HEIGHT){
        MAP_StrTrimWhitespace(buffer);
        int x = 0;
        int oldIndex = 0;
        int index = 0;
        const char *ptr = buffer;
        while ((ptr = strchr(buffer + oldIndex, ',')) != NULL && x < LOBBY_WIDTH){
            index = ptr - buffer;
            char tmp[32];
            substring(buffer, oldIndex, index, tmp);
            int id = atoi(tmp);
            if (id < 0 || id >= TILE_INDEX_COUNT) id = 0;
            lobbyTileID[y][x++] = id;
            oldIndex = index + 1;
        }
        if (x < LOBBY_WIDTH && oldIndex < (int)strlen(buffer)){
            char tmp[32];
            substring(buffer, oldIndex, strlen(buffer), tmp);
            int id = atoi(tmp);
            if (id < 0 || id >= TILE_INDEX_COUNT) id = 0;
            lobbyTileID[y][x++] = id;
        }
        y++;
    }
    fclose(fp);
}

SDL_Rect MAP_getTileRect(Map aMap) {
    return aMap->tileRect;
}

static void substring(char *buffer, int start, int end, char* result){
    if (start < 0 || end < 0 || end <= start || buffer == NULL)
        return;
    int len = end - start;
    strncpy(result, buffer + start, len);
    result[len] = '\0'; 
}

void MAP_TilesFillWithBlank(int tileID[MAP_HEIGHT][MAP_WIDTH]){
    for (int y = 0; y < MAP_HEIGHT; y++){
        for (int x = 0; x < MAP_WIDTH; x++){
            tileID[y][x] = MAX_COUNT_SPRITE_TILES+1; 
        }
    }
}

static void MAP_StrTrimWhitespace(char *str){
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
}

void printMap(Map aMap){
    for (int y = 0; y < MAP_HEIGHT; ++y){
        for (int x = 0; x < MAP_WIDTH; ++x){
            printf("%d,",aMap->tileID[y][x]);
        }
        printf("\n");
    }
}

void MAP_MapMoveMap(Map aMap, SDL_Point playerOffset){
    aMap->tileRect.y = - playerOffset.y;
    aMap->tileRect.x = - playerOffset.x;
    //printf("Y: %d, X: %d\n",aMap->tileRect.y,aMap->tileRect.x);
}

void MAP_mapSetPlanet(GameState state,Map aMap){
    switch (state){
    case NEMUR: aMap->planet = NEMUR_LUT;
        break;
    case AURANTIC: aMap->planet = AURANTIC_LUT;
        break;
    case CINDORA: aMap->planet = CINDORA_LUT;
        break;
    default:
        break;
    }
}
