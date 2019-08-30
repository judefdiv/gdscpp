# GDScpp
GDScpp is a C++ library that make GDS files easier to handle.

Version: 0.1

## Features
* GDS to ASCII
* Easily create GDS file
* Importing GDS files

## Examples
Examples of what GDScpp can do:

### LEF/DEF to GDSII
``` bash
./gdscpp -g lefExample.lef defExample.def -o gdsOutput.gds
./gdscpp -g lefExample.lef defExample.def # automatically assigns output filename
```

## Needed stuffs
``` bash
apt install build-essencials 	# for compiling
```
## To Compile
``` bash
# Current directory: Die2Sim root
mkdir build && cd build
cmake ..
make
```

## Notes
The library main goals to aid development of the ColdFlux team IARPA contract SuperTools 
Author - JF de Villiers (Stellenbosch University)