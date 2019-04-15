add_executable(test_mesh "test_mesh.cpp")
target_link_libraries(test_mesh libmc4kidz)
add_test(test_mesh test_mesh)
