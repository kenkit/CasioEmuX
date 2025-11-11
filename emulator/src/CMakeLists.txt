add_executable(casioemu
    casioemu.cpp
    Emulator.cpp
    Logger.cpp
    Chipset/CPU.cpp
    Chipset/CPUPushPop.cpp
    Chipset/MMURegion.cpp
    Chipset/CPUControl.cpp
    Chipset/CPUArithmetic.cpp
    Chipset/CPULoadStore.cpp
    Chipset/Chipset.cpp
    Chipset/MMU.cpp
    Chipset/InterruptSource.cpp
    Chipset/Coprocessor.cpp
    Peripheral/BatteryBackedRAM.cpp
    Peripheral/Peripheral.cpp
    Peripheral/Keyboard.cpp
    Peripheral/Screen.cpp
    Peripheral/Timer.cpp
    Peripheral/StandbyControl.cpp
    Peripheral/ROMWindow.cpp
    Peripheral/Miscellaneous.cpp
    Peripheral/BCDCalc.cpp
    Peripheral/PowerSupply.cpp
    Peripheral/TimerBaseCounter.cpp
    Peripheral/RealTimeClock.cpp
    Peripheral/WatchdogTimer.cpp
    Peripheral/ExternalInterrupts.cpp
    Peripheral/IOPorts.cpp
    Peripheral/Flash.cpp
    Gui/CodeViewer.cpp
    Gui/Command.cpp
    Data/ModelInfo.cpp
    Gui/imgui/imgui_impl_sdl2.cpp
    Gui/imgui/imgui_impl_sdlrenderer2.cpp
    Gui/imgui/imgui.cpp
    Gui/imgui/imgui_widgets.cpp
    Gui/imgui/imgui_tables.cpp
    Gui/imgui/imgui_draw.cpp
)

target_include_directories(casioemu PRIVATE
    $<TARGET_PROPERTY:SDL2::SDL2,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:SDL2_image::SDL2_image,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:lua,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:edit,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:edit_a,INTERFACE_INCLUDE_DIRECTORIES>
    ${CMAKE_SOURCE_DIR}/libs/wineditline/src
)

target_link_libraries(casioemu PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    lua
    edit_a
)
set_target_properties(edit_a PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
    LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
)
set_target_properties(lua PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
    LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
)
set_target_properties(SDL2_image PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
    LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
)
set_target_properties(SDL2  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
    LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
)
set_target_properties(SDL2main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
    LIBRARY_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:casioemu>
)