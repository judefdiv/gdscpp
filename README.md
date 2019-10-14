# GDScpp
GDScpp is a C++ library that make GDS files easier to handle.

Version: 0.2

## Features
* GDS to ASCII.
* Easily create GDS file.
* Importing GDS files.
* Extract the root GDS structures.
* Create a hierarchy diagram of all the GDS structure dependencies.
* Direct improving of GDS files with redundancy checking (Still to be completed)

## Examples
Examples of what GDScpp can do:

### GDSII
``` bash
./gdscpp -g -o gdsOutput.gds
./gdscpp -g lefExample.lef defExample.def # automatically assigns output filename if not specified
```

## Needed stuffs
``` bash
apt install build-essencials 	# for compiling
apt install dot               # for creating hierarchy diagram(dot file)
```
## To Compile
``` bash
# Current directory: Die2Sim root
mkdir build && cd build
cmake ..
make
```

## To Do
* Improve text handling
* When reading in GDS files into memory, check for redundancies
* aref has not been tested.

## Notes
The library main goals to aid development of the ColdFlux team IARPA contract SuperTools

Author -

Text and nodes are not fully supported.

Box is currently not supported.