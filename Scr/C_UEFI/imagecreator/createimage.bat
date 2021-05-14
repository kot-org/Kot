gcc.exe -Wall -O2 -std=c17 -m64  -c src/main.c -o main.o

gcc.exe  -o bin/ImageCreator.exe main.o  -s -static -m64  

@echo off
del *.o
@echo on

pause