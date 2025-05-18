# Search of the Älien


# How to install

#### Tested systems:
- **Windows 10** (and later)
- **macOS Sequoia**
- **Ubuntu 22.04** (and later)

> ⚠️ **Note:** If your system isn't on the list above, there is no guarantee that the game will run as expected.

## Linux and macOS
Install all [dependencies](./dependencies.md).

### Client installation
```bash
git clone https://github.com/EliasMailosson/Search-of-the-Alien.git && \
cd Search-of-the-Alien && \
mkdir build && \
make client
```

### Server installation
```bash
git clone https://github.com/EliasMailosson/Search-of-the-Alien.git && \
cd Search-of-the-Alien && \
mkdir build && \
make server
```

## Windows

### Client installation
1. **Download** the latest release from the [Releases page](https://github.com/EliasMailosson/Search-of-the-Alien/releases).
2. Unzip and run **search-of-the-alien.exe**.

### Server installation
1. **Download** the latest release from the [Releases page](https://github.com/EliasMailosson/Search-of-the-Alien/releases).
2. Unzip and run **server.exe**.

## Manual Installation (build from scratch)
1. Clone this repo: ```git clone https://github.com/EliasMailosson/Search-of-the-Alien.git```
2. Install all [dependencies](./dependencies.md).
3. Run ```make all```, ```make client```, or ```make server``` depending on your case.
4. Start the server with ```make run_server``` and the client with ```make run_client```
