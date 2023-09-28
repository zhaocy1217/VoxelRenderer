echo off
cd third
cd yaml-cpp
cmake -B Build
cd ..
cd tinyobjloader
cmake -B Build
cd ..
cd glm
cmake -B Build
cd ..
cd glfw
cmake -B Build
cd ..
cd assimp
cmake -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_ZLIB=ON -DLIBRARY_SUFFIX="" -B build
cd ..
cd ..