add_library(turingdb_core_pymodule MODULE
            ModuleInit.cpp
            PyTuringClient.cpp
            PyUtils.cpp
            PyProfilerWrapper.cpp)

target_include_directories(turingdb_core_pymodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


set_target_properties(turingdb_core_pymodule PROPERTIES
    PREFIX ""                   # No 'lib' prefix on the output
    SUFFIX "${Python_SOABI}.so"  # Proper Python extension suffix
)

target_include_directories(turingdb_core_pymodule PRIVATE
    ${PYTHON_INCLUDE_DIR}
    ${NUMPY_INCLUDE_DIR}
)

target_link_libraries(turingdb_core_pymodule PRIVATE 
                      turing_client_cpp_s
                      turing_common_s
                      python3.10)

target_link_directories(turingdb_core_pymodule PRIVATE ${PYTHON_LIB_DIR})

add_custom_command(TARGET turingdb_core_pymodule POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:turingdb_core_pymodule>
    ${CMAKE_BINARY_DIR}/pymodule/turingdb/
)
