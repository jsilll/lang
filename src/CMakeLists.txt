file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(compiler ${SOURCES})

llvm_map_components_to_libnames(llvm_libs core support)

target_link_libraries(compiler ${llvm_libs})