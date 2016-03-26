set(Main_SRCS
    main.cc
)

add_executable(myApp
    ${Main_SRCS}
)

# Specify here the libraries this program depends on
target_link_libraries(myApp
    btree
)

install(TARGETS myApp DESTINATION bin)
