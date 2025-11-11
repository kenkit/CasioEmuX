cmake_minimum_required(VERSION 3.16)
project(CasioEmuX)
find_package(Git)
if(GIT_FOUND)
  message("git found: ${GIT_EXECUTABLE}")
endif()
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
if(MSVC)
    add_compile_options(/W3 /EHsc)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(SDL2IMAGE_INSTALL OFF CACHE BOOL "Disable installation of SDL2_image")

 execute_process(
                   COMMAND ${GIT_EXECUTABLE}  reset --hard 
                   ${PROJECT_SOURCE_DIR}/libs/SDL_image)
 execute_process(
                   COMMAND ${GIT_EXECUTABLE}  reset --hard 
                   ${PROJECT_SOURCE_DIR}/libs/lua)
 execute_process(
                   COMMAND ${GIT_EXECUTABLE}  reset --hard 
                   ${PROJECT_SOURCE_DIR}/libs/wineditline)
execute_process(
            COMMAND ${GIT_EXECUTABLE}  clean -f
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/lua) 
execute_process(COMMAND ${GIT_EXECUTABLE}  apply  ${PROJECT_SOURCE_DIR}/patches/lua.patch
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/lua
  ) 
add_custom_target(apply_sdl_patch_reset
                   COMMAND ${GIT_EXECUTABLE}  reset --hard 
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/SDL)                   
add_custom_target(apply_sdl_patch
  COMMENT "|||||||||||PATCHING SDL|||||||||||||||||"
  COMMAND ${GIT_EXECUTABLE}  apply  ${PROJECT_SOURCE_DIR}/patches/sdl.patch
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/SDL
  )

  add_custom_target(apply_sdl_image_patch_reset
                   COMMAND ${GIT_EXECUTABLE}  reset --hard 
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/SDL_image)
add_custom_target(apply_sdl_image_patch
  COMMENT "|||||||||||PATCHING SDL_IMAGE|||||||||||||||||"
  COMMAND ${GIT_EXECUTABLE}  apply  ${PROJECT_SOURCE_DIR}/patches/sdl_image.patch
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/SDL_image
  )


execute_process(
COMMAND ${GIT_EXECUTABLE}  reset --hard 
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/wineditline)
execute_process(
  COMMAND ${GIT_EXECUTABLE}  apply  ${PROJECT_SOURCE_DIR}/patches/wineeditline.patch
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/wineditline)

add_subdirectory(libs/SDL)
add_dependencies(apply_sdl_patch apply_sdl_patch_reset)
add_dependencies(SDL2 apply_sdl_patch)


add_subdirectory(libs/SDL_image)
add_dependencies(apply_sdl_image_patch apply_sdl_image_patch_reset)
add_dependencies(SDL2_image apply_sdl_image_patch)



add_subdirectory(libs/lua)


add_subdirectory(libs/wineditline)

add_subdirectory(src)
