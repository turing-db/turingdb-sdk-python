if(NOT PYTHON_INCLUDE_DIR)
    message(FATAL_ERROR "PYTHON_INCLUDE_DIR must be provided")
endif()

if(NOT PYTHON_LIB_DIR)
    message(FATAL_ERROR "PYTHON_LIB_DIR must be provided")
endif()

if(NOT NUMPY_INCLUDE_DIR)
    message(FATAL_ERROR "NUMPY_INCLUDE_DIR must be provided")
endif()

message(STATUS "Python include: ${PYTHON_INCLUDE_DIR}")
message(STATUS "Python lib: ${PYTHON_LIB_DIR}")
message(STATUS "NumPy include: ${NUMPY_INCLUDE_DIR}")

# Check of newlines at EOF
message(STATUS "Checking for newlines at EOF")
execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build/findnoeol ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE NOEOLATEOF)

if (${NOEOLATEOF} EQUAL 1)
    message(FATAL_ERROR "Found files without EOL at EOF")
endif ()

# C++ compiler
set(CLANG_NEEDED_VERSION "9.4.0")

# C++ standard
Set(CMAKE_CXX_STANDARD 20)

set(TURING_CPP_WARNINGS -Wall -Werror -Wunused-function -Wunused-variable)

# Create compilation database and symlink in the src folder
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)

# Threads
find_package(Threads REQUIRED)

enable_testing()

add_subdirectory(pythonmodule)
add_subdirectory(native)
