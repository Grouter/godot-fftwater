# Project is still WIP -> only height map generation works

# Godot FFT Water
Works on Godot 3.1-beta

# Usage
1) Clone [godot-cpp](https://github.com/GodotNativeTools/godot-cpp) repo **recursively** (it contains submodules)
2) Put it in this directory next to `godotProject`, `lib` and `src`

# Dependencies
1) Scons
2) g++ or clang++

# Compile
``` bash
# g++
scons p=linux/mac/windows

# clang++
scons p=linux/osx/windows use_llvm=1
```
