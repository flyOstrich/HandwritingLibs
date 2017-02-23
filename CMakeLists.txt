cmake_minimum_required(VERSION 3.6)
project(HandwritingLibs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories("include")
# Where to find CMake modules and OpenCV
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message("windows......................")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}\\cmake\\)
    set(OpenCV_DIR  D:\\android\\opencv310Custom\\opencv310Custom\\mingw-build\\install)
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SOURCE_FILES main.cpp
                 modules/linear_interpolation/linear_interpolation.cpp
                 modules/mat_util/mat-util.cpp
                 modules/recognizer/recognizer.cpp
                 modules/image_util/image-util.cpp
                 modules/type_util/type-util.cpp
                 modules/json_util/json_util.cpp
                 modules/cluster_analysis/cluster_analysis.cpp
                 modules/painter/painter.cpp
                 modules/trainer/trainer.cpp
                 modules/android-interface.cpp
                 modules/file_util/file-util.cpp modules/stroke_classifier/stroke-classifier.cpp modules/character_recognizer/character_recognizer.cpp)
message("${CMAKE_SYSTEM_NAME}")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message("windows......................")
    add_definitions(-DOP_WINDOWS)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message("not windows .................")
    add_definitions(-DOP_DARWIN)
endif()

message("source files : ${SOURCE_FILES}")
add_executable(HandwritingLibs ${SOURCE_FILES})
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs opencv_ml opencv_objdetect)
target_link_libraries(HandwritingLibs ${OpenCV_LIBS})