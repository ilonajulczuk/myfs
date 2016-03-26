file(GLOB SRCS *.cc)

ADD_EXECUTABLE(testbtree ${SRCS})

TARGET_LINK_LIBRARIES(testbtree
    btree
    libgtest
    libgmock
)

add_test(NAME testbtree
         COMMAND testbtree)
