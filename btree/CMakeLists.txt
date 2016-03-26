set(btree_SRCS
    btree.h
    btree.cc
    bfs.h
    bfs.cc
)

# Declare the library
add_library(btree STATIC
    ${btree_SRCS}
)

# Specify here the include directories exported
# by this library
target_include_directories(btree PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
