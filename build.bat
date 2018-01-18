@echo off

set CMP_FLAGS=/FC /Zi /MD /Fesilentcommand.exe
set INC_FLAGS=/I ..\inc /I ..\inc\gl3w
set LNK_FLAGS=..\lib\*.lib opengl32.lib shell32.lib /SUBSYSTEM:CONSOLE
mkdir bin
pushd bin
cl %CMP_FLAGS% %INC_FLAGS% ..\main.cpp /link %LNK_FLAGS%
popd

