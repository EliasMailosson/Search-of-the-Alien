#include "../../include/MAP/map.h"
typedef struct tile { 
    SDL_Rect tileRect;              
    int tileID;
} Tile;

struct Map {
    SDL_Texture *texture;
    int tile_width;
    int tile_height;
    SDL_Rect tileIndex[MAX_COUNT_TILES];
    Tile **tiles;
};

void substring(char *buffer, int start, int end, char* result);
void MAP_StrTrimWhitespace(char *str);
void printMap(Map aMap);


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
    SDL_RenderCopy(pRend, aMap->texture, &aMap->tileIndex[aMap->tiles[y][x].tileID], &aMap->tiles[y][x].tileRect);
}

Tile** MAP_TileCreate(int posX,int posY){
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
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            tiles[y][x].tileRect.w = TILE_SIZE;
            tiles[y][x].tileRect.h = TILE_SIZE;
            tiles[y][x].tileRect.x = (int)((x - y) * (TILE_SIZE/2) + (posX/2) - TILE_SIZE/2); 
            tiles[y][x].tileRect.y = (int)((x + y) * (TILE_SIZE/4) + (posY / 2) - (TILE_SIZE / 4) - (TILE_SIZE * MAP_HEIGHT / 4));
            tiles[y][x].tileID = 0;
        }
    }
    return tiles;
}

Map MAP_MapCreate(SDL_Renderer *pRend, int posX, int posY){
    (void)posX;(void)posY;
    Map aMap = malloc(sizeof(struct Map));
    if(aMap == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }
    MAP_TileSheetload(pRend, FILE_PHAT_LOBBY_SPRITE, aMap);
    aMap->tile_width = TILE_SIZE;
    aMap->tile_height = TILE_SIZE;
    int n = 0;
    for (int y = 0; y < 3; y++){
        for (int x = 0; x < 6; x++){
            aMap->tileIndex[n++] = (SDL_Rect){.x = (TILE_SPRITE_SIZE*x), .y = (TILE_SPRITE_SIZE*y), .w = TILE_SPRITE_SIZE, .h = TILE_SPRITE_SIZE};
        }
    }
    aMap->tileIndex[19] = (SDL_Rect){.x = (TILE_SPRITE_SIZE*0), .y = (TILE_SPRITE_SIZE*3), .w = TILE_SPRITE_SIZE, .h = TILE_SPRITE_SIZE};
    aMap->tileIndex[20] = (SDL_Rect){.x = (TILE_SPRITE_SIZE*1), .y = (TILE_SPRITE_SIZE*3), .w = TILE_SPRITE_SIZE, .h = TILE_SPRITE_SIZE};
    aMap->tiles = MAP_TileCreate(posX,posY);
    MAP_MapGetTilseFromLobby(aMap->tiles);
    printMap(aMap);
    return aMap;
}

void MAP_MapRefresh(Map aMap, int posX, int posY){
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            aMap->tiles[y][x].tileRect.x = (int)((x - y) * (TILE_SIZE/2) + (posX/2) - TILE_SIZE/2); 
            aMap->tiles[y][x].tileRect.y = (int)((x + y) * (TILE_SIZE/4) + (posY/2) - (TILE_SIZE / 4) - (TILE_SIZE * MAP_HEIGHT / 4));
        }
    }
}

void MAP_TileSheetload(SDL_Renderer* pRend, char *imagePath, Map aMap){
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

void MAP_MapGetTilseFromLobby(Tile **ppTiles) {
    char buffer[256];
    FILE *fp = fopen(FILE_PHAT_LOBBY_DATA, "r");
    if (fp == NULL) {
        fprintf(stderr,"Error: Could not open %s!\n", FILE_PHAT_LOBBY_DATA);
        return;
    }
    int y = 0;
    while (fgets(buffer, sizeof(buffer), fp) && y < MAP_HEIGHT) {
        MAP_StrTrimWhitespace(buffer);
        int x = 0;
        int oldIndex = 0;
        int index = 0;
        const char *ptr = buffer;
        while ((ptr = strchr(buffer + oldIndex, ',')) != NULL && x < MAP_WIDTH) {
            index = ptr - buffer;
            char tmp[32];
            substring(buffer, oldIndex, index, tmp);
            int id = atoi(tmp);
            if (id < 0 || id >= TILE_INDEX_COUNT) id = 0;
            ppTiles[y][x++].tileID = id;
            oldIndex = index + 1;
        }
        if (x < MAP_WIDTH && oldIndex < (int)strlen(buffer)) {
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

void substring(char *buffer, int start, int end, char* result){
    if (start < 0 || end < 0 || end <= start || buffer == NULL)
        return;
    int len = end - start;
    strncpy(result, buffer + start, len);
    result[len] = '\0'; 
}

void MAP_StrTrimWhitespace(char *str){
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
            printf("%d,",aMap->tiles[y][x].tileID);
        }
        printf("\n");
    }
}  