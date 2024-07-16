# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/pedro/esp/esp-idf/components/bootloader/subproject"
  "/home/pedro/Pessoal/code/UTFPR/TCC/seebum_embedded/build/bootloader"
  "/home/pedro/Pessoal/code/UTFPR/TCC/seebum_embedded/build/bootloader-prefix"
  "/home/pedro/Pessoal/code/UTFPR/TCC/seebum_embedded/build/bootloader-prefix/tmp"
  "/home/pedro/Pessoal/code/UTFPR/TCC/seebum_embedded/build/bootloader-prefix/src/bootloader-stamp"
  "/home/pedro/Pessoal/code/UTFPR/TCC/seebum_embedded/build/bootloader-prefix/src"
  "/home/pedro/Pessoal/code/UTFPR/TCC/seebum_embedded/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/pedro/Pessoal/code/UTFPR/TCC/seebum_embedded/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/pedro/Pessoal/code/UTFPR/TCC/seebum_embedded/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
