# Godot FFT Water
Works on Godot 3.2-beta

This project is still not finished. See TODO board in here:
[Road to version 1.0](https://github.com/Grouter/godot-fftwater/projects/1)

![Water-1](https://github.com/Grouter/godot-fftwater/blob/master/screenshots/water-alpha1.png)
![Water-2](https://github.com/Grouter/godot-fftwater/blob/master/screenshots/water-alpha2.png)

# Usage (if you want to just use it in your game)
[WIKI](https://github.com/Grouter/godot-fftwater/wiki)

# Usage (if you want to extend this in c++)
1) Clone this repo **recursively** with `git clone --recursive`
2) Navigate to `godot-cpp` folder
3) Install dependencies (see `Dependencies` section below) and run:
```bash
# This may take a longer time to compile
scons p=linux/osx/windows generate_bindings=yes
```
4) Navigate back to the root directory of this repo and run command in `Compile` section below

# Dependencies
1) Scons
2) g++
3) FFTW3 configured with 
```
./configure --enable-shared --with-pic
make
make install
```

# Compile
Everytime you make change in c++ code you must recompile with one of these commands to see change in Godot
``` bash
scons p=linux/osx/windows
```

# Thanks to

[Ocean simulation part two: using the fast Fourier transform](https://www.keithlantz.net/2011/11/ocean-simulation-part-two-using-the-fast-fourier-transform/)
