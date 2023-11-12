set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.c)

set(SOURCES ${SOURCES})

add_executable(${BINARY} ${SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE popt m)
