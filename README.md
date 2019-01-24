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
# Thanks to
[Simple-FFT](https://github.com/d1vanov/Simple-FFT)

[Ocean simulation part two: using the fast Fourier transform](https://www.keithlantz.net/2011/11/ocean-simulation-part-two-using-the-fast-fourier-transform/)
