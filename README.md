Mostly an attempt to write an ECS from scratch in C. There is a game there but right now it's just move the wizard around.  

Prereqs:  
raylib https://github.com/raysan5/raylib

Contains code from:  
sc https://github.com/tezc/sc (sc_map.c and sc_map.h, those files BSD-3 license, see source)

Instructions:  
On Linux with raylib installed as a shared library the commands below should just work. On OSX I had to build raylib as a static library and 
leave it in /lib. On Windows either of the above methods should work with MinGW but haven't tested.  

```
make  
cd bin  
./eschatology
```
use 'make BUILD=RELEASE' for non-debug build