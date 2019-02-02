# Godot FFT Water
Works on Godot 3.1-beta

This project is still not finished. See TODO boeard in here:
[Road to version 1.0](https://github.com/Grouter/godot-fftwater/projects/1)

---

# Usage (if you want to just use it in your gameboard
`tessendorf.gdns` library just generates displacement values and pushes them via texture to material(shader) passed in by update funtion. Actual vertex displacement must be done in shader.

Download zip from [release page](https://github.com/Grouter/godot-fftwater/releases) and unzip in your Godot project folder.

1) Create `MeshInstance` and set `PlaneMesh` as mesh property
    * adjust size and subdivide as you want
2) Create `ShaderMaterial` in material property and attach `Water.shader` to it
3) Attach `Water.gd`
4) Change as you want

---

# Usage (if you want to extend this in c++)
1) Clone [godot-cpp](https://github.com/GodotNativeTools/godot-cpp) repo **recursively** (it contains submodules)
2) Put it in this directory next to `godotProject`, `lib` and `src`
3) Navigate to `godot-cpp` folder
4) Install dependencies (see `Dependencies` section below) and run:
```bash
scons p=linux/osx/windows generate_bindings=yes
```
5) Navigate back to the root directory of this repo and run command in `Compile` section below

## Dependencies
1) Scons
2) g++ or clang++
3) FFTW3 configured with `./configure --enable-shared --with-pic` flags

## Compile (depends on your preference g++ or clang++)
Everytime you make change in c++ code you must recompile with one of these commands to see change in Godot
``` bash
# g++
scons p=linux/osx/windows

# clang++
scons p=linux/osx/windows use_llvm=1
```

---

# Thanks to

[Ocean simulation part two: using the fast Fourier transform](https://www.keithlantz.net/2011/11/ocean-simulation-part-two-using-the-fast-fourier-transform/)
