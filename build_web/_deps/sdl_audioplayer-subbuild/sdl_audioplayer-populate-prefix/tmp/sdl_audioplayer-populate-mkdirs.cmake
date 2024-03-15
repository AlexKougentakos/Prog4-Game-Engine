# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/sdl_audioplayer-src"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/sdl_audioplayer-build"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/sdl_audioplayer-subbuild/sdl_audioplayer-populate-prefix"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/sdl_audioplayer-subbuild/sdl_audioplayer-populate-prefix/tmp"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/sdl_audioplayer-subbuild/sdl_audioplayer-populate-prefix/src/sdl_audioplayer-populate-stamp"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/sdl_audioplayer-subbuild/sdl_audioplayer-populate-prefix/src"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/sdl_audioplayer-subbuild/sdl_audioplayer-populate-prefix/src/sdl_audioplayer-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/sdl_audioplayer-subbuild/sdl_audioplayer-populate-prefix/src/sdl_audioplayer-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/sdl_audioplayer-subbuild/sdl_audioplayer-populate-prefix/src/sdl_audioplayer-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
