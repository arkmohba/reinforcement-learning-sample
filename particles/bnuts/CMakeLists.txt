cmake_minimum_required(VERSION 3.8)

# OpenCV
# set("OpenCVDIR" "F:\\libs\\opencv\\opencv\\build")
# For OpenCV

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(LIBNAME "libs")
add_library(${LIBNAME} display.cpp simulator.cpp ball.cpp linked_list.cpp b_nuts_env.cpp)
find_package(OpenCV REQUIRED)
# インクルードパスを指定
target_include_directories(
  ${LIBNAME}
  PUBLIC  ${OpenCV_INCLUDE_DIRS}
)
# ライブラリディレクトリの指定
# リンクするライブラリ名指定
target_link_libraries(${LIBNAME}
${OpenCV_LIBS}
  )

# MAINの作成
set(MAIN "bnuts")
# プロジェクト名を指定する
project(${MAIN}  CXX)
add_executable(${MAIN} main.cpp)
# インクルードパスを指定
target_include_directories(
  ${MAIN}
  PUBLIC  ${OpenCV_INCLUDE_DIRS}
)
# ライブラリディレクトリの指定
# リンクするライブラリ名指定
target_link_libraries(${MAIN}
PRIVATE
${OpenCV_LIBS}
${LIBNAME}
)

# pybind11ライブラリの作成
set(PYBINDPROJ "bnutsenv")
project(${PYBINDPROJ} VERSION 0.1.0)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(pybind11)
pybind11_add_module(${PYBINDPROJ} wrapper.cpp)
# インクルードパスを指定
target_include_directories(
  ${PYBINDPROJ}
  PUBLIC  ${OpenCV_INCLUDE_DIRS}
)
# ライブラリディレクトリの指定
# リンクするライブラリ名指定
target_link_libraries(${PYBINDPROJ}
PRIVATE
${OpenCV_LIBS}
${LIBNAME}
)