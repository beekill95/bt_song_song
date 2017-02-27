cmake_minimum_required(VERSION 2.8)
project(matrix_multiply)

# required mpi
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# include headers and sources
set(HEADERS io.h matrix.h)
set(SOURCES io.c matrix.c)

add_executable(main_p2p ${SOURCES} ${HEADERS} main_p2p.c)
add_executable(main_group ${SOURCES} ${HEADERS} main_group.c)

target_link_libraries(main_p2p ${MPI_LIBRARIES})
target_link_libraries(main_group ${MPI_LIBRARIES})

if (MPI_COMPILE_FLAGS)
	set_target_properties(main_p2p PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
	set_target_properties(main_group PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if (MPI_LINK_FLAGS)
	set_target_properties(main_p2p PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
	set_target_properties(main_group PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
