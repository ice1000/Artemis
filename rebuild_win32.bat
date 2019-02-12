@echo off
cd cmake-vs-debug
msbuild ALL_BUILD.vcxproj
cd ..
cmake-vs-debug\Debug\Artemis.exe