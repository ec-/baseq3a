# Build Instructions For Windows Debug Files

Create a batch file similar to the following and execute it so your ENV VARS are set.

```
cd baseq3a\build\win32-msvc
set QUAKE3DIR=C:\quake3
set QUAKE3EXE=quake3e.x64.exe
set MODDIR=baseq3
set ARCH=x64
start "" baseq3a.sln
exit
```
