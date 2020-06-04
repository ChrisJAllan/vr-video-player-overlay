#!/bin/sh

dependencies="glm glew sdl2 openvr x11 xcomposite"
includes=$(pkg-config --cflags $dependencies)
libs=$(pkg-config --libs $dependencies)
gcc -c src/window_texture.c -O2 $includes
g++ -c src/main.cpp -O2 $includes
g++ -o vr-video-player window_texture.o main.o $libs
