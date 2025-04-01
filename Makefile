# Makefile: building server/client with SDL2
# ==== OS-detektering ====
OS := $(shell uname -s 2>/dev/null)
ifeq ($(OS),)
    OS := Windows_NT
endif

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
    REMOV = del /Q build\*.o $(CLIENT_EXEC) $(SERVER_EXEC)
    SERVER_EXEC = build/server.exe
    CLIENT_EXEC = build/main.exe
    RUN = ./
    PREFORM =
endif

# ==== Vanliga variabler ====
CLIENT_TARGET = $(CLIENT_EXEC)
SERVER_TARGET = $(SERVER_EXEC)
SRCDIR = source
NETDIR = source/NET
UIDIR = source/UI
BUILDDIR = build
OBJ_CLIENT = $(BUILDDIR)/main.o $(BUILDDIR)/game.o $(BUILDDIR)/clientLife.o $(BUILDDIR)/menu.o $(BUILDDIR)/panel.o $(BUILDDIR)/client.o
OBJ_SERVER = $(BUILDDIR)/server.o $(BUILDDIR)/shared.o 

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

$(BUILDDIR)/server.o: $(NETDIR)/server.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/shared.o: $(NETDIR)/shared.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/client.o: $(NETDIR)/client.c
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
