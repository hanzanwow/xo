cmake_minimum_required(VERSION 3.16)
project(xo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

# C++23 std::print / std::println on libstdc++ (GCC) needs the experimental lib.
link_libraries(stdc++exp)

set(ENGINE TicTacToe.cpp XOBot.cpp Player.cpp)

# --- Terminal game (unchanged behavior) ---
add_executable(gamehub main.cpp ${ENGINE})

# --- Unit tests (XOBot only, via the HardModeProbe friend hook) ---
add_executable(test_runner test_runner.cpp XOBot.cpp)

# --- Minimax timing benchmark ---
add_executable(execution-time execution-time.cpp ${ENGINE})

# --- Graphical (SFML) game ---
find_package(SFML 2.6 COMPONENTS graphics window system QUIET)
if(SFML_FOUND)
    add_executable(gamehub-gui gui_main.cpp GameGui.cpp ${ENGINE})
    target_link_libraries(gamehub-gui sfml-graphics sfml-window sfml-system)
else()
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(SFML sfml-graphics sfml-window sfml-system)
    endif()
    if(SFML_FOUND)
        add_executable(gamehub-gui gui_main.cpp GameGui.cpp ${ENGINE})
        target_include_directories(gamehub-gui PRIVATE ${SFML_INCLUDE_DIRS})
        target_link_directories(gamehub-gui PRIVATE ${SFML_LIBRARY_DIRS})
        target_link_libraries(gamehub-gui ${SFML_LIBRARIES})
    else()
        message(WARNING "SFML not found; skipping gamehub-gui target.")
    endif()
endif()
