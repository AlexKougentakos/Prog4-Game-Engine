# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/_deps/glm-src"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/_deps/glm-build"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/_deps/glm-subbuild/glm-populate-prefix"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/_deps/glm-subbuild/glm-populate-prefix/tmp"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/downloads"
  "D:/Programming/VisualStudio/Odyssey-CMake Git/out/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey-CMake Git/out/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Programming/VisualStudio/Odyssey-CMake Git/out/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
