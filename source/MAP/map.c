#include "../../include/MAP/map.h"
typedef struct tile { 
    SDL_Rect tileRect; 
    SDL_Rect baseRect;
    int tileID;
} Tile;

struct Map {
    SDL_Texture *texture;
    SDL_Rect tileIndex[MAX_COUNT_SPRITE_TILES];
    Tile **tiles;
};

static void substring(char *buffer, int start, int end, char* result);
static void MAP_StrTrimWhitespace(char *str);
static void printMap(Map aMap);
static void MAP_TileSheetload(SDL_Renderer* pRend, char *imagePath, Map aMap);


void MAP_MapRender(SDL_Renderer *pRend, Map aMap){
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            MAP_TileRender(pRend, aMap, y, x);
        }
    }
    //red lines
    SDL_SetRenderDrawColor(pRend, 255, 0, 0, 255);  // red
    //SDL_RenderDrawLine(pRend, 0, pRend.windowHeight / 2, pView->windowWidth / 2, pView->windowHeight / 2);   // diagonal line
    //SDL_RenderDrawLine(pRend, pView->windowWidth / 2, 0, pView->windowWidth / 2, pView->windowHeight / 2);   // vertical line
}

void MAP_TileRender(SDL_Renderer *pRend, Map aMap, int y, int x){
    if(aMap->tiles[y][x].tileID >= MAX_COUNT_SPRITE_TILES) return;
    SDL_RenderCopy(pRend, aMap->texture, &aMap->tileIndex[aMap->tiles[y][x].tileID], &aMap->tiles[y][x].tileRect);
}

Tile** MAP_TileCreate(int winW,int winH){
    Tile **tiles = malloc(sizeof(Tile*) * MAP_HEIGHT);
    if (!tiles) {
        printf("malloc tiles rows failde\n");
        
    }
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        tiles[y] = malloc(sizeof(Tile) * MAP_WIDTH);
        if (!tiles[y]) {
            printf("malloc tiles[y] failde\n");
        }
    }
    SDL_Point offset = (SDL_Point){.x = 0,.y = 0};
    //MAP_MapGettOffset(winH,winW,&offset);
    //printf("X:%d Y:%d\n",offset.x,offset.y);
    for (int y = 0; y < MAP_HEIGHT; y++){
        for (int x = 0; x < MAP_WIDTH; x++){
            SDL_Rect calculated_rect = {
                .w = TILE_SIZE,
                .h = TILE_SIZE,
                .x = (int)((x - y) * (TILE_SIZE / 2) + (LOGICAL_WIN_W / 2) - offset.x),
                .y = (int)((x + y) * (TILE_SIZE / 4) + (LOGICAL_WIN_H / 2) - offset.y)
            };
            tiles[y][x].baseRect = calculated_rect;
            tiles[y][x].tileRect = calculated_rect; 
            tiles[y][x].tileID = (MAX_COUNT_SPRITE_TILES+1);
        }
    }
    return tiles;
}

Map MAP_MapCreate(SDL_Renderer *pRend, int winW, int winH){
    Map aMap = malloc(sizeof(struct Map));
    if(aMap == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }
    MAP_TileSheetload(pRend, FILE_PHAT_LOBBY_SPRITE, aMap);
    int n = 0;
    for (int y = 0; y < 3; y++){
        for (int x = 0; x < 6; x++){
            aMap->tileIndex[n++] = (SDL_Rect){.x = (TILE_SPRITE_SIZE*x), .y = (TILE_SPRITE_SIZE*y), .w = TILE_SPRITE_SIZE, .h = TILE_SPRITE_SIZE};
        }
    }
    aMap->tileIndex[19] = (SDL_Rect){.x = (TILE_SPRITE_SIZE*0), .y = (TILE_SPRITE_SIZE*3), .w = TILE_SPRITE_SIZE, .h = TILE_SPRITE_SIZE};
    aMap->tileIndex[20] = (SDL_Rect){.x = (TILE_SPRITE_SIZE*1), .y = (TILE_SPRITE_SIZE*3), .w = TILE_SPRITE_SIZE, .h = TILE_SPRITE_SIZE};
    aMap->tiles = MAP_TileCreate(winW,winH);
    MAP_MapGetTilseFromLobby(aMap->tiles);
    MAP_MapRefresh(aMap,winW,winH);
    aMap->tiles[15][15].tileID = MAX_COUNT_SPRITE_TILES + 1;
    return aMap;
}

void MAP_MapRefresh(Map aMap, int winW, int winH){
    float scaleX = (float)winW / LOGICAL_WIN_W;
    float scaleY = (float)winH / LOGICAL_WIN_H;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
    int offsetX = (winW - (int)(LOGICAL_WIN_W * scale)) / 2;
    int offsetY = (winH - (int)(LOGICAL_WIN_H * scale)) / 2;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++){
            SDL_Rect base = aMap->tiles[y][x].baseRect;
            aMap->tiles[y][x].tileRect.x = offsetX + (int)(base.x * scale);
            aMap->tiles[y][x].tileRect.y = offsetY + (int)(base.y * scale);
            aMap->tiles[y][x].tileRect.w = (int)(base.w * scale);
            aMap->tiles[y][x].tileRect.h = (int)(base.h * scale);
        }
    }
}

static void MAP_TileSheetload(SDL_Renderer* pRend, char *imagePath, Map aMap){
    SDL_Surface *surface = IMG_Load(imagePath);
    aMap->texture = SDL_CreateTextureFromSurface(pRend, surface);
    SDL_FreeSurface(surface);
    if(!aMap->texture){
        printf("Error creating texture for map %s\n", IMG_GetError());
    }
}

void MAP_TileDestroy(Tile **tiles) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        free(tiles[y]);
    }
    free(tiles);
    tiles = NULL;
}

void MAP_MapDestroy(Map aMap){
    SDL_DestroyTexture(aMap->texture);
    aMap->texture = NULL;
    MAP_TileDestroy(aMap->tiles);
    free(aMap);
    aMap = NULL;
}

void MAP_MapGetTilseFromLobby(Tile **ppTiles){
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
            ppTiles[y][x++].tileID = id;
            oldIndex = index + 1;
        }
        if (x < LOBBY_WIDTH && oldIndex < (int)strlen(buffer)){
            char tmp[32];
            substring(buffer, oldIndex, strlen(buffer), tmp);
            int id = atoi(tmp);
            if (id < 0 || id >= TILE_INDEX_COUNT) id = 0;
            ppTiles[y][x++].tileID = id;
        }
        y++;
    }
    fclose(fp);
}

static void substring(char *buffer, int start, int end, char* result){
    if (start < 0 || end < 0 || end <= start || buffer == NULL)
        return;
    int len = end - start;
    strncpy(result, buffer + start, len);
    result[len] = '\0'; 
}

void MAP_TilesFillWithBlank(Tile **tiles){
    for (int y = 0; y < MAP_HEIGHT; y++){
        for (int x = 0; x < MAP_WIDTH; x++){
            tiles[y][x].tileID = MAX_COUNT_SPRITE_TILES+1; 
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

static void printMap(Map aMap){
    for (int y = 0; y < MAP_HEIGHT; ++y){
        for (int x = 0; x < MAP_WIDTH; ++x){
            printf("%d,",aMap->tiles[y][x].tileID);
        }
        printf("\n");
    }
}

void MAP_MapGettOffset(int winH, int winW, SDL_Point *index){
    //printf("H:%d W:%d\n",winH,winW);
    SDL_Point tileIndex = (*index);
    Tile tiles[MAP_HEIGHT][MAP_WIDTH] = {0};
    for (int y = 0; y < MAP_HEIGHT; ++y){
        for (int x = 0; x < MAP_WIDTH; ++x){
            tiles[y][x].tileRect.w = TILE_SIZE;
            tiles[y][x].tileRect.h = TILE_SIZE;
            tiles[y][x].tileRect.x = (int)((x - y) * (TILE_SIZE/2)); 
            tiles[y][x].tileRect.y = (int)((x + y) * (TILE_SIZE/4));
        }
    }
    (*index) = (SDL_Point){.x = tiles[tileIndex.x]->tileRect.x,
                        .y = tiles[tileIndex.y]->tileRect.y};
}