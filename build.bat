@echo off

del Z:\shellhack\build\engine\Debug\engine_*.pdb > NUL 2> NUL

call cmake -B build -S src
call cmake --build build
