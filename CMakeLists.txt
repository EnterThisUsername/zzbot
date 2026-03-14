cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")

project(BotToolkit VERSION 1.0.0)

# ── Geode SDK ────────────────────────────────────────────────────────────────
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR
        "GEODE_SDK environment variable is not set.\n"
        "Run: geode config set sdk <path-to-sdk>"
    )
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# ── Sources ───────────────────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

# ── Include paths ─────────────────────────────────────────────────────────────
target_include_directories(${PROJECT_NAME} PRIVATE
    src/
)

# ── Compiler options ──────────────────────────────────────────────────────────
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /wd4251 /wd4275>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused-parameter>
)

# ── Link ──────────────────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} geode-sdk)

# ── Geode setup macro (handles packaging, resources, etc.) ────────────────────
setup_geode_mod(${PROJECT_NAME})
