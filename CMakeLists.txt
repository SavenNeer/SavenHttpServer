cmake_minimum_required(VERSION 3.5)
project(SavenServer)


#生成工具库SavenUtils------------------------------------------
add_library(SavenUtils STATIC
    src/utils.cpp
)
#这个的作用就是指明在做SavenUtils静态库时需要寻找的头文件应该去哪里找
target_include_directories(SavenUtils
    PUBLIC
        ${CMAKE_SOURCE_DIR}#添加该选项是为了在码代码时不让VScode报错
        ${CMAKE_SOURCE_DIR}/include
)


#生成工具库SavenSocket------------------------------------------
add_library(SavenSocket STATIC
    src/saven_socket.cpp
)

target_include_directories(SavenSocket
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)



#生成工具库SavenHeaderParse------------------------------------------
add_library(SavenHeaderParse STATIC
    src/saven_headerparse.cpp
)

#库之间发生了调用 所以要注意不同静态库之间的链接
target_link_libraries(SavenHeaderParse PUBLIC SavenUtils)

target_include_directories(SavenHeaderParse
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

#生成工具库SavenHeaderBuilder------------------------------------------
add_library(SavenHeaderBuilder STATIC
    src/saven_headerbuilder.cpp
)

target_link_libraries(SavenHeaderBuilder PUBLIC SavenHeaderParse)
target_link_libraries(SavenHeaderBuilder PUBLIC SavenUtils)

target_include_directories(SavenHeaderBuilder
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)


#生成工具库SavenHeaderSolve------------------------------------------
add_library(SavenHeaderSolve STATIC
    src/saven_headersolver.cpp
)

#库之间发生了调用 所以要注意不同静态库之间的链接
target_link_libraries(SavenHeaderSolve PUBLIC SavenHeaderParse)

target_include_directories(SavenHeaderSolve
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)


#生成工具库SavenHttpServer------------------------------------------
add_library(SavenHttpServer STATIC
    src/saven_httpserver.cpp
)
#库调用
target_link_libraries(SavenHttpServer PUBLIC SavenSocket)
target_link_libraries(SavenHttpServer PUBLIC SavenHeaderSolve)
target_link_libraries(SavenHttpServer PUBLIC SavenHeaderBuilder)

target_include_directories(SavenHttpServer
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)


#生成工具库SavenTimeWheel------------------------------------------
add_library(SavenTimeWheel STATIC
    src/saven_timewheel.cpp
)

target_include_directories(SavenTimeWheel
    PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

#生成可执行文件------------------------------------------
add_executable(SavenServer
    src/main.cpp
)

#链接可执行文件的静态库
target_link_libraries(SavenServer SavenUtils)
target_link_libraries(SavenServer SavenSocket)
target_link_libraries(SavenServer SavenHeaderParse)
target_link_libraries(SavenServer SavenHttpServer)
target_link_libraries(SavenServer SavenHeaderSolve)
target_link_libraries(SavenServer SavenHeaderBuilder)
target_link_libraries(SavenServer SavenTimeWheel)


