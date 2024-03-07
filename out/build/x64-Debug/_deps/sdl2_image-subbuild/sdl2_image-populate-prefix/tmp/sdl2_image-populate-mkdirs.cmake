# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/build/x64-Debug/_deps/sdl2_image-src"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/build/x64-Debug/_deps/sdl2_image-build"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/build/x64-Debug/_deps/sdl2_image-subbuild/sdl2_image-populate-prefix"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/build/x64-Debug/_deps/sdl2_image-subbuild/sdl2_image-populate-prefix/tmp"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/build/x64-Debug/_deps/sdl2_image-subbuild/sdl2_image-populate-prefix/src/sdl2_image-populate-stamp"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/build/x64-Debug/downloads"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/build/x64-Debug/_deps/sdl2_image-subbuild/sdl2_image-populate-prefix/src/sdl2_image-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey-CMake Git/out/build/x64-Debug/_deps/sdl2_image-subbuild/sdl2_image-populate-prefix/src/sdl2_image-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey-CMake Git/out/build/x64-Debug/_deps/sdl2_image-subbuild/sdl2_image-populate-prefix/src/sdl2_image-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
