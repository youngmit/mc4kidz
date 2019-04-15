# set(CMAKE_WIN32_EXECUTABLE true)
add_library (libmc4kidz "state.cpp;shapes.cpp;materials.cpp;particle.cpp;mesh.cpp")
target_include_directories(libmc4kidz PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "libs ${wxWidgets_LIBRARIES}")

target_link_libraries(libmc4kidz INTERFACE OpenGL::OpenGL ${GLUT_LIBRARIES})

add_executable(mc4kidz "main.cpp")
target_link_libraries(mc4kidz libmc4kidz OpenGL::GLU OpenGL::OpenGL ${GLUT_LIBRARIES})
message(STATUS "GL stuff: ${OPENGL_GLU_FOUND}")

add_subdirectory(tests)
