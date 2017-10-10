cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(FPRemover)


#set(CMAKE_CXX_FLAGS "-std=c0xx")


find_package(wxWidgets)

if(wxWidgets_FOUND)
	include(${wxWidgets_USE_FILE})
	include_directories(${wxWidgets_INCLUDE_DIRS})
	add_executable(
		FPRemover WIN32
		Character.cpp
		WrestlerLoader.cpp
		Wrestler.cpp
		gui.cpp
	)
	target_link_libraries(FPRemover ${wxWidgets_LIBRARIES})
endif()
