# Papaya
Tool to vizualize electron orbits around a nucleus
> **Note**
> Papaya currently only supports orbits up to the 3rd energy level
## Planned Updates
- Add support for more energy levels
- Allow for forward and backward movement in slice mode
- 3D voxel view of orbits
- More settings for customizing experience 

# How To Compile
## Dependencies
This project requires GTK4 to compile

## Compile Instructions
Run the following commands
```
mkdir build
cd build
cmake ../
cmake --build .
```
Then to run the program
run the papaya application
```
./papaya
```

# Controls
- wasd: move
- up/down arrows: zoom in and out
### only in orbit vizualization mode
- z/x: change intensity of orbits color
- 1/2/3: view xz, xy, and zy planes respectively 
