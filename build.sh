set -xe

CPPC=i686-w64-mingw32-c++
$CPPC -g -o steamapi_enumerate.o -c steamapi_enumerate.cpp 
$CPPC -static -v -o steamapi_enumerate.exe steamapi_enumerate.o -lntdll -ldbghelp


CPPC=x86_64-w64-mingw32-c++
$CPPC -g -o steamapi_enumerate.o -c steamapi_enumerate.cpp 
$CPPC -static -v -o steamapi_enumerate64.exe steamapi_enumerate.o -lntdll -ldbghelp


CPPC=i686-w64-mingw32-c++
$CPPC -g -fPIC -c main.cpp -std=c++20 -o main.o -O0
$CPPC -static -v -shared -o steam_api.dll main.o -lntdll

CPPC=x86_64-w64-mingw32-c++
$CPPC -g -fPIC -c main.cpp -std=c++20 -o main.o -O0
$CPPC -static -v -shared -o steam_api64.dll main.o -lntdll

rm *.o
