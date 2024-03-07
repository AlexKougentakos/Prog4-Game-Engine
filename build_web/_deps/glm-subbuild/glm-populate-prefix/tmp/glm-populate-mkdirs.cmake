# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/glm-src"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/glm-build"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/glm-subbuild/glm-populate-prefix"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/glm-subbuild/glm-populate-prefix/tmp"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/downloads"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey-CMake Git/build_web/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
