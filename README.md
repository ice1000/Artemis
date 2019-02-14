# Artemis

Shooting Game editor.

Just a proof-of-concept, code quality / cross-platform is not considered important during development.
Only support Windows with DX11 but can be migrated to GLFW/SDL easily.

# Build

```shell
$ git clone https://github.com/ice1000/Artemis.git
$ cd Artemis
$ mkdir Debug ; cd Debug
$ cmake .. && msbuild ALL_BUILD.vcxproj
$ cd ..
$ Debug\Debug\Artemis.exe
```
