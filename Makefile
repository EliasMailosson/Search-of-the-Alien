# Makefile: building server/client with SDL2
# ==== OS-detektering ====
ifeq ($(OS), Windows_NT)
    OS := Windows_NT
else 
    OS := $(shell uname -s 2>/dev/null)
endif
# $(info === Detekterat OS: $(OS))

ifeq ($(OS), Darwin)
# --- macOS Settings ---
    CC = clang
    CFLAGS = -fsanitize=address -fsanitize=undefined -g -Wall -Wextra \
             -I/opt/homebrew/include/SDL2 \
             -I/opt/homebrew/include/SDL2_image \
             -I/opt/homebrew/include/SDL2_ttf \
             -I/opt/homebrew/include/SDL2_mixer \
             -I/opt/homebrew/include/SDL2_net
    LDFLAGS = -fsanitize=address \
              -L/opt/homebrew/lib \
              -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_net
    REMOV = rm -rf build/*.o $(CLIENT_EXEC) $(SERVER_EXEC)
    SERVER_EXEC = build/server
    CLIENT_EXEC = build/main
    RUN = ./
    PREFORM =
else ifeq ($(OS), Windows_NT)
# --- Windows (MinGW/MSYS) Settings ---
    CC = gcc
    INCLUDE = C:/msys64/mingw64/include/SDL2
    CFLAGS = -g -Wall -Wextra -I$(INCLUDE)
    LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf \
              -lSDL2_mixer -lSDL2_net -lws2_32
    REMOV = del /Q $(BUILDDIR)\*.o & if exist $(CLIENT_EXEC) del /Q $(CLIENT_EXEC) & if exist $(SERVER_EXEC) del /Q $(SERVER_EXEC)
    SERVER_EXEC = build/server.exe
    CLIENT_EXEC = build/main.exe
    RUN = ./
    PREFORM =
else ifeq ($(OS), Linux)
# --- Linux (Fedora/Ubuntu) Settings with clang and sanitizers ---
    CC = clang
    CFLAGS = -fsanitize=address -fsanitize=undefined -g -Wall -Wextra `sdl2-config --cflags`
    LDFLAGS = -fsanitize=address `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_net
    REMOV = rm -rf build/*.o $(CLIENT_EXEC) $(SERVER_EXEC)
    SERVER_EXEC = build/server
    CLIENT_EXEC = build/main
    RUN = ./
    PREFORM =
endif

# ==== Vanliga variabler ====
CLIENT_TARGET = $(CLIENT_EXEC)
SERVER_TARGET = $(SERVER_EXEC)
SRCDIR = source
NETDIR = source/NET
UIDIR = source/UI
MAPDIR = source/MAP
BUILDDIR = build
OBJ_CLIENT = $(BUILDDIR)/main.o $(BUILDDIR)/game.o $(BUILDDIR)/clientLife.o $(BUILDDIR)/menu.o $(BUILDDIR)/panel.o $(BUILDDIR)/client.o $(BUILDDIR)/label.o $(BUILDDIR)/button.o $(BUILDDIR)/checklist.o $(BUILDDIR)/protocol.o $(BUILDDIR)/packetHandler.o $(BUILDDIR)/inputfield.o $(BUILDDIR)/shared.o $(BUILDDIR)/friend.o $(BUILDDIR)/players.o $(BUILDDIR)/map.o $(BUILDDIR)/hud.o $(BUILDDIR)/animation.o
OBJ_SERVER = $(BUILDDIR)/server.o $(BUILDDIR)/shared.o $(BUILDDIR)/protocol.o $(BUILDDIR)/packetHandler.o $(BUILDDIR)/enemies.o
OBJ_CLIENT = $(BUILDDIR)/main.o $(BUILDDIR)/game.o $(BUILDDIR)/clientLife.o $(BUILDDIR)/menu.o $(BUILDDIR)/panel.o $(BUILDDIR)/client.o $(BUILDDIR)/label.o $(BUILDDIR)/button.o $(BUILDDIR)/checklist.o $(BUILDDIR)/protocol.o $(BUILDDIR)/packetHandler.o $(BUILDDIR)/inputfield.o $(BUILDDIR)/shared.o $(BUILDDIR)/friend.o $(BUILDDIR)/players.o $(BUILDDIR)/map.o $(BUILDDIR)/hud.o $(BUILDDIR)/animation.o $(BUILDDIR)/terminalHub.o $(BUILDDIR)/perlinNoise.o
OBJ_SERVER = $(BUILDDIR)/server.o $(BUILDDIR)/shared.o $(BUILDDIR)/protocol.o $(BUILDDIR)/packetHandler.o $(BUILDDIR)/serverLogic.o $(BUILDDIR)/map.o $(BUILDDIR)/perlinNoise.o

# Default Goal
all: $(BUILDDIR) $(CLIENT_TARGET) $(SERVER_TARGET)

# Build one exe file
client: $(CLIENT_EXEC)

server: $(SERVER_EXEC)

# Create build folder if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Client build rule
$(CLIENT_TARGET): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) $(OBJ_CLIENT) -o $(CLIENT_TARGET) $(LDFLAGS)

# Server build rule
$(SERVER_TARGET): $(OBJ_SERVER)
	$(CC) $(CFLAGS) $(OBJ_SERVER) -o $(SERVER_TARGET) $(LDFLAGS)

# Individuella objektfiler
$(BUILDDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/game.o: $(SRCDIR)/game.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/clientLife.o: $(SRCDIR)/clientLife.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/menu.o: $(SRCDIR)/menu.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/panel.o: $(UIDIR)/panel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/label.o: $(UIDIR)/label.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/animation.o: $(UIDIR)/animation.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/button.o: $(UIDIR)/button.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/checklist.o: $(UIDIR)/checklist.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/inputfield.o: $(UIDIR)/inputfield.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/friend.o: $(UIDIR)/friend.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/server.o: $(NETDIR)/server.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/shared.o: $(NETDIR)/shared.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/client.o: $(NETDIR)/client.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/protocol.o: $(NETDIR)/protocol.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/packetHandler.o: $(NETDIR)/packetHandler.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/players.o: $(SRCDIR)/players.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/map.o: $(MAPDIR)/map.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/hud.o: $(SRCDIR)/hud.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/enemies.o: $(NETDIR)/enemies.c
$(BUILDDIR)/terminalHub.o: $(SRCDIR)/terminalHub.c
	$(CC) $(CFLAGS) -c $< -o $@
    
$(BUILDDIR)/serverLogic.o: $(NETDIR)/serverLogic.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/perlinNoise.o: $(MAPDIR)/perlinNoise.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	$(REMOV)

run_server:
	$(RUN)$(SERVER_TARGET)

run_client:
	$(PREFORM) $(RUN)$(CLIENT_TARGET)

ifeq ($(OS), Windows_NT)
run_clients:
	powershell -Command "Start-Process .\\$(CLIENT_TARGET)"
	powershell -Command "Start-Process .\\$(CLIENT_TARGET)"
else
run_clients:
	$(PREFORM) ./$(CLIENT_TARGET) & $(PREFORM) ./$(CLIENT_TARGET)
endif
