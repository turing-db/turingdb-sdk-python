cmake_minimum_required(VERSION 3.6)
project(python-turingdb)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(core)

file(GLOB_RECURSE PYMODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/pymodule/*.py")
add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/python_module_copied
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/pymodule/ ${CMAKE_CURRENT_BINARY_DIR}/pymodule/
	COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/python_module_copied 
	DEPENDS ${PYMODULE_FILES}
	COMMENT "Copying Python module files to build directory") 

add_custom_target(copy_python_module ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/python_module_copied)


