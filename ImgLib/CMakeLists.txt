cmake_minimum_required(VERSION 3.11)

project(ImgLib CXX)
set(CMAKE_CXX_STANDARD 17)

set(IMGLIB_MAIN_FILES img_lib.h img_lib.cpp)
set(IMGLIB_FORMAT_FILES 
    ppm_image.h ppm_image.cpp
    jpeg_image.h jpeg_image.cpp
    bmp_image.h bmp_image.cpp)
set(IMGLIB_OTHER_FILES pack_defines.h)

set(LIBJPEG_DIR CACHE STRING "LibJPEG static library directory")
if(NOT LIBJPEG_DIR)
    message(FATAL_ERROR "Please, specify LibJPEG directory via -DLIBJPEG_DIR=<dir>")
endif()
message(STATUS "LibJPEG dir is ${LIBJPEG_DIR}, change via -DLIBJPEG_DIR=<dir>")

add_library(ImgLib STATIC ${IMGLIB_MAIN_FILES} 
            ${IMGLIB_FORMAT_FILES} ${IMGLIB_OTHER_FILES})

if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()

target_include_directories(ImgLib PUBLIC "${LIBJPEG_DIR}/include")

# Флаг INTERFACE обозначает видимость параметра - только для зависимых целей.
# Для самой ImgLib не нужно указывать зависимости компоновки, поскольку
# статическая библиотека не компонуется, а просто архивируется
target_link_directories(ImgLib INTERFACE
    "${LIBJPEG_DIR}/lib/$<IF:$<CONFIG:Debug>,Debug,Release>"
    )

# В качестве зависимости указано jpeg. Компоновщик будет искать файл libjpeg.a
target_link_libraries(ImgLib INTERFACE jpeg) 

#add_library(ImgLibDll SHARED
#            imglib_dll_interface.h 
#            imglib_dll_interface.cpp
#            imglib_dll_defines.h)

#target_link_libraries(ImgLibDll ImgLib ${SYSTEM_LIBS})

# определяем для сборки ImgLibDll макрос IMGLIB_DLL,
# чтобы экспорт функций получился правильным
#target_compile_definitions(ImgLibDll PRIVATE IMGLIB_DLL)