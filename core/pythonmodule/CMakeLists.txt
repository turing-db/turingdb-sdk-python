add_library(turingdb_core_pymodule MODULE
            ModuleInit.cpp
            PyTuringRequest.cpp
            PyUtils.cpp)

target_include_directories(turingdb_core_pymodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


set_target_properties(turingdb_core_pymodule PROPERTIES
    PREFIX ""                   # No 'lib' prefix on the output
    SUFFIX "${Python_SOABI}${CMAKE_SHARED_LIBRARY_SUFFIX}"  # Proper Python extension suffix
)

# Link against Python libraries
target_link_libraries(turingdb_core_pymodule PRIVATE Python3::Python)

# Include Python header directories
target_include_directories(turingdb_core_pymodule PRIVATE ${Python3_INCLUDE_DIRS})

target_link_libraries(turingdb_core_pymodule PRIVATE 
                     turing_client_cpp_request )

add_custom_command(TARGET turingdb_core_pymodule POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:turingdb_core_pymodule>
    ${CMAKE_BINARY_DIR}/pymodule/turingdb/
)
