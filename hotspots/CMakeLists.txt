cmake_minimum_required(VERSION 3.14)
project(Hotspots)

find_program(JEMALLOC_FOUND jemalloc-config)
if(JEMALLOC_FOUND)
	execute_process(
		COMMAND jemalloc-config --libs
		OUTPUT_VARIABLE JEMALLOC_LIBS
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(JEMALLOC_LIBS "${JEMALLOC_LIBS} -ljemalloc")
endif()

include(FetchContent)
FetchContent_Declare(
	parlaylib
	GIT_REPOSITORY https://github.com/cmuparlay/parlaylib.git
	GIT_TAG master
)
FetchContent_MakeAvailable(parlaylib)

set(GRAPH_FILE "${CMAKE_BINARY_DIR}/graph_data.tar.bz2")
message(STATUS "Downloading graph data...")
file(DOWNLOAD http://www.cs.cmu.edu/~parlay/graph_data.tar.bz2 ${GRAPH_FILE}
     SHOW_PROGRESS
     STATUS download_status
     LOG download_log)
list(GET download_status 0 status_code)
if(NOT status_code EQUAL 0)
    message(FATAL_ERROR "Error downloading graph data: ${download_log}")
endif()

message(STATUS "Extracting graph data...")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xjf ${GRAPH_FILE}
                RESULT_VARIABLE tar_result
                OUTPUT_VARIABLE tar_output
                ERROR_VARIABLE tar_error)
if(NOT tar_result EQUAL 0)
    message(FATAL_ERROR "Error extracting graph data: ${tar_error}")
endif()
file(REMOVE ${GRAPH_FILE})

add_executable(hotspots hotspots.cpp)
target_include_directories(hotspots PRIVATE ${parlaylib_SOURCE_DIR} ${parlaylib_SOURCE_DIR}/include/)
target_link_libraries(hotspots PRIVATE ${JEMALLOC_LIBS})