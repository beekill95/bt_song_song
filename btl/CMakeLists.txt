cmake_minimum_required(VERSION 2.6)
project("mpi_matrix_multiplication")

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

add_executable(genmatrix tools/genmatrix.cpp)
add_executable(viewmatrix tools/viewmatrix.cpp)

target_link_libraries(genmatrix ${MPI_LIBRARIES})
target_link_libraries(viewmatrix ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
	set_target_properties(genmatrix PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
	set_target_properties(viewmatrix PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
	set_target_properties(genmatrix PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
	set_target_properties(viewmatrix PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
