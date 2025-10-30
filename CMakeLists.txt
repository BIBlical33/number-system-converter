cmake_minimum_required(VERSION 3.20)

function(find_make_appx result)
  find_windows_sdk(BIN sdk_bin_dir)

  find_program(
    make_appx
    NAMES MakeAppx
    PATHS "${sdk_bin_dir}"
    REQUIRED
  )

  set(${result} "${make_appx}")

  return(PROPAGATE ${result})
endfunction()

project(number_system_converter VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/O2)
    else()
        add_compile_options(-O3)
    endif()
endif()

if (MSVC)
    add_compile_options(/W4 /Zi)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Svg)

qt_standard_project_setup()

qt_add_executable(number_system_converter
    src/main.cc
    src/application.cc
    src/theme_manager.cc
    include/application.h
    include/theme_manager.h
)
target_include_directories(number_system_converter PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(number_system_converter PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)

qt_add_resources(number_system_converter "resources"
    FILES
        resources/copy-icon-black.svg
        resources/copy-icon-white.svg
        resources/copy-icon-blue.svg
        resources/app-icon.svg
)


set_target_properties(number_system_converter PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)