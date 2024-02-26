# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Programming/VisualStudio/Odyssey - CMake Git/out/build/x64-Debug/_deps/sdl2_ttf-src"
  "D:/Programming/VisualStudio/Odyssey - CMake Git/out/build/x64-Debug/_deps/sdl2_ttf-build"
  "D:/Programming/VisualStudio/Odyssey - CMake Git/out/build/x64-Debug/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix"
  "D:/Programming/VisualStudio/Odyssey - CMake Git/out/build/x64-Debug/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/tmp"
  "D:/Programming/VisualStudio/Odyssey - CMake Git/out/build/x64-Debug/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/src/sdl2_ttf-populate-stamp"
  "D:/Programming/VisualStudio/Odyssey - CMake Git/out/build/x64-Debug/downloads"
  "D:/Programming/VisualStudio/Odyssey - CMake Git/out/build/x64-Debug/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/src/sdl2_ttf-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey - CMake Git/out/build/x64-Debug/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/src/sdl2_ttf-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey - CMake Git/out/build/x64-Debug/_deps/sdl2_ttf-subbuild/sdl2_ttf-populate-prefix/src/sdl2_ttf-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
