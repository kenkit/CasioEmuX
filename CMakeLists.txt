cmake_minimum_required(VERSION 3.16)
project(emulator)
add_subdirectory(emulator)
set_target_properties(casioemu PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)
  add_custom_target(copy_models ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                ${CMAKE_SOURCE_DIR}/models
               $<TARGET_FILE_DIR:casioemu>/models)
