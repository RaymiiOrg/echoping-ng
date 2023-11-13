set(BINARY ${CMAKE_PROJECT_NAME})

add_executable(${BINARY} echoping.h echoping.c
        http.h http.c
        util.h util.c
        HTParse.h HTParse.c
        icp.h icp.c
        smtp.h smtp.c
        error.h error.c
        writen.h writen.c
        readline.h readline.c)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE popt m)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra)
