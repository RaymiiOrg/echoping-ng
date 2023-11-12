cmake_minimum_required(VERSION 3.10)
project(echoping-ng)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_subdirectory(src)


set(PACKAGE_BUGREPORT "github-remove-.spam@relst.spam.nl")
set(PACKAGE_NAME string(TOLOWER ${PROJECT_NAME}))
set(PACKAGE_VERSION ${PROJECT_VERSION})
set(PACKAGE_STRING ${PACKAGE_NAME}-${PACKAGE_VERSION})
set(PACKAGE_TARNAME ${PACKAGE_STRING}.tar.gz)
set(PACKAGE_URL "http://github.com/raymiiorg/echoping-ng")

configure_file(configure.h.in src/configure.h)

install(TARGETS echoping-ng DESTINATION bin)
