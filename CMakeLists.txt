cmake_minimum_required(VERSION 3.14)
project(raylibFramework VERSION 1.0.0 LANGUAGES C CXX)

# Optionally, disable raylib examples (or other options) before downloading raylib.
# This ensures you only build the raylib library.
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable building raylib examples" FORCE)
set(BUILD_GAMES     OFF CACHE BOOL "Disable building raylib games" FORCE)

include(FetchContent)

# Download and make available raylib
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        5.5 # You can use 'master' or a specific tag/commit
)
FetchContent_MakeAvailable(raylib)

# Collect the source files for your framework.
# Adjust the glob patterns as needed or list the files explicitly.
file(GLOB_RECURSE RAYLIBFRAMEWORK_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE RAYLIBFRAMEWORK_HEADER CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# Create the library target (static or shared as desired).
add_library(raylibFramework ${RAYLIBFRAMEWORK_SRC} ${RAYLIBFRAMEWORK_HEADER})

# Specify include directories for your library.
target_include_directories(raylibFramework PUBLIC 
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>"
)

# Link the downloaded raylib library.
target_link_libraries(raylibFramework PRIVATE raylib)

# Optionally, add installation rules
include(GNUInstallDirs)
install(TARGETS raylibFramework EXPORT raylibFrameworkConfig
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/ DESTINATION include)
