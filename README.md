Mostly an attempt to write an ECS from scratch in C. There is a game there but right now it's just move the wizard around.  

Prereqs:  
Raylib https://github.com/raysan5/raylib  
Raylib 

Uses code from:  
sc_map (simple hashmap) see https://github.com/tezc/sc  

Instructions:  
On Linux Raylib installed as a shared library the commands below should just work. On OSX I had to build Raylib as a static library and 
leave it in /raylib/src. On Windows either of the above methods should work with MinGW but haven't tested.  


```
make  
cd bin  
./eschatology
```