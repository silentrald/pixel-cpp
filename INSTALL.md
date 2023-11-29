# Installation

## Linux

### Requirements

- C++ Compiler (Clang or GCC)
- Make
- CMake

**Arch**
```sh
sudo pacman -S make cmake clang
```

**Ubuntu**
```sh
sudo apt install make cmake clang
```

### Steps

Clone the repository
```sh
git clone --recursive https://github.com/silentrald/pixel-cpp.git
```

Build the executable
```sh
cd pixel-cpp
mkdir build
cd build

cmake ..
make
```

Run the program
```sh
./pixel
```

## Windows

### Requirements

- MSYS2

### MSYS2 Setup

After installing MSYS2, open the `MSYS2 MINGW64` terminal.
In there install run the following command to install the necessary packages.

```sh
pacman -S mingw-w64-x86_64-make mingw-w64-x86_64-cmake mingw-w64-x86_64-freetype
```

To add the executables to Windows, we need to add the bin path of mingw64\bin to the PATH variable in Environment Variables. Default installation path is `C:\msys64\mingw64\bin`.

### Steps

```sh
git clone --recursive https://github.com/silentrald/pixel-cpp.git
```

Build the executable
```sh
cd pixel-cpp
mkdir build
cd build

cmake -G "MinGW Makefiles" ..
mingw32-make
```

Run the program
```sh
./pixel
```

## MAC

🚧 Under Construction 🚧
