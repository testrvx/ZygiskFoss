project(external)

OPTION(LSPLT_BUILD_SHARED OFF)
add_subdirectory(lsplt/lsplt/src/main/jni)

add_library(phmap INTERFACE)
target_include_directories(phmap INTERFACE parallel-hashmap)
target_compile_options(phmap INTERFACE -Wno-unused-value)

add_library(mimalloc STATIC mimalloc/src/static.c)
target_include_directories(mimalloc PUBLIC mimalloc/include)
target_compile_definitions(mimalloc PUBLIC MI_SECURE=4)
