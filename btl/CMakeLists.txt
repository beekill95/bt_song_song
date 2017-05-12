cmake_minimum_required(VERSION 2.6)
project("mpi_matrix_multiplication")

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

add_executable(genmatrix tools/genmatrix.cpp)
add_executable(viewmatrix tools/viewmatrix.cpp utils/io.cpp)
add_executable(compareMatrices tools/compareMatrices.cpp utils/io.cpp)
add_executable(two_sided main.cpp utils/io.cpp common.cpp)
add_executable(seq main_seq.cpp utils/io.cpp common.cpp)

target_link_libraries(genmatrix ${MPI_LIBRARIES})
target_link_libraries(viewmatrix ${MPI_LIBRARIES})
target_link_libraries(compareMatrices ${MPI_LIBRARIES})
target_link_libraries(two_sided ${MPI_LIBRARIES})
target_link_libraries(seq ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
	set_target_properties(genmatrix PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
	set_target_properties(viewmatrix PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
	set_target_properties(compareMatrices PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
	set_target_properties(two_sided PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
	set_target_properties(seq PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
	set_target_properties(genmatrix PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
	set_target_properties(viewmatrix PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
	set_target_properties(compareMatrices PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
	set_target_properties(two_sided PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
	set_target_properties(seq PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()
