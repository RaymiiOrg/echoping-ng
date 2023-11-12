set(BINARY ${CMAKE_PROJECT_NAME})

add_executable(${BINARY} echoping.h echoping.c error.c readline.c writen.c util.c http.c icp.c HTParse.c icp.h HTParse.h smtp.c)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE popt m)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra)
