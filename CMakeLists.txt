# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
project ("mc4kidz")
cmake_minimum_required (VERSION 3.8)


if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()
message(STATUS "VCPKG_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "VCPKG_INSTALLED_DIR: ${_VCPKG_INSTALLED_DIR}")

enable_testing()

set(CMAKE_CXX_STANDARD 17)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

add_definitions(-D_UNICODE -DUNICODE -DWXUSINGDLL -DwxUSE_GUI=1 -D__WXMSW__)
  if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    add_definitions(-D_DEBUG)
    include_directories(${_VCPKG_INSTALLED_DIR}/x64-windows/debug/lib/mswud)
    include_directories(${_VCPKG_INSTALLED_DIR}/x64-windows/include)
    set(wxWidgets_LIB_DIR ${_VCPKG_INSTALLED_DIR}/x64-windows/debug/lib)
    set(wxWidgets_LIBRARIES
      ${wxWidgets_LIB_DIR}/wxbase31ud_net.lib
      ${wxWidgets_LIB_DIR}/wxmsw31ud_core.lib
      ${wxWidgets_LIB_DIR}/wxbase31ud.lib
      winmm comctl32 oleacc rpcrt4 shlwapi version wsock32
    )
  elseif("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    include_directories(${_VCPKG_INSTALLED_DIR}/x64-windows/lib/mswu)
    include_directories(${_VCPKG_INSTALLED_DIR}/x64-windows/include)
    set(wxWidgets_LIB_DIR ${_VCPKG_INSTALLED_DIR}/x64-windows/lib)
    set(wxWidgets_LIBRARIES
      ${wxWidgets_LIB_DIR}/wxbase31u_net.lib
      ${wxWidgets_LIB_DIR}/wxmsw31u_core.lib
      ${wxWidgets_LIB_DIR}/wxbase31u.lib
      winmm comctl32 oleacc rpcrt4 shlwapi version wsock32
    )
  endif()

message(STATUS "wx libraries ${wxWidgets_LIBRARIES}")



# Include sub-projects.
add_subdirectory ("mc4kidz")
