# set(CMAKE_WIN32_EXECUTABLE true)
add_library (libmc4kidz "state.cpp;shapes.cpp;materials.cpp;particle.cpp;mesh.cpp")
target_include_directories(libmc4kidz PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "libs ${wxWidgets_LIBRARIES}")

target_link_libraries(libmc4kidz PRIVATE OpenGL::GL GLUT::GLUT OpenGL::GLU)

add_executable(mc4kidz "main.cpp")
target_link_libraries(mc4kidz PRIVATE libmc4kidz OpenGL::GL GLUT::GLUT)

add_subdirectory(tests)
