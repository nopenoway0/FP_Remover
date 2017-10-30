cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(FP_Remover)
include_directories(headers/)
add_executable(FP_Remover
	source/Character.cpp
	source/loader_tester.cpp
	source/Wrestler.cpp
	source/WrestlerLoader.cpp)