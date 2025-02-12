### SteamAPI_asiloader

Hitch a ride on steam_api.dll/steam_api64.dll loading to load asi files, inspired by https://github.com/sst311212/SteamAPI_Mini

#### Usage

0. navigate to game exe directory
1(x86). rename steam_api.dll to valveapi.dll
1(x64). rename steam_api64.dll to valveapi64.dll 
2. place .asi files into game directory, eg. one can inject reshade/specialK as asi files, for when all their supported hitch hiking don't work

#### Build

0. on a linux/cygwin environment, install i686-w64-mingw32-c++ and x86_64-w64-mingw32-c++
1. run build.sh, it will fail during main.cpp build, that is okay
2(x86). copy steam_api.dll from game directory next to steamapi_enumerate.exe
2(x64). copy steam_api64.dll from game directory next to steamapi_enumerate64.exe
3. run build.sh again, steam_api.dll and steam_api64.dll should be built successfully
