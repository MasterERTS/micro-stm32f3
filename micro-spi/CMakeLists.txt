#to compile the project:
# mkdir _build
# cd _build
# cmake -D CMAKE_TOOLCHAIN_FILE=../../sys/cmake/arm-none-eabi.cmake ..
# make
# make flash
cmake_minimum_required(VERSION 3.5)

#project name
project(lab4)

#application sources
set(SRCS
	${CMAKE_SOURCE_DIR}/main.cpp
)

option(WITH_TFT "add compilation rules for the TFT support" ON)
include("../sys/cmake/coroLabs.cmake")
