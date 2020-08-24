cmake_minimum_required(VERSION 3.8)

# OpenCV
set("OpenCVDIR" "F:\\libs\\opencv\\opencv\\build")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(LIBNAME "libs")
add_library(${LIBNAME} display.cpp simulator.cpp ball.cpp linked_list.cpp b_nuts_env.cpp)
# インクルードパスを指定
target_include_directories(
  ${LIBNAME}
  PRIVATE "${OpenCVDIR}\\include"
)
# ライブラリディレクトリの指定
target_link_directories(${LIBNAME} PRIVATE "${OpenCVDIR}\\x64\\vc15\\lib\\" )
# リンクするライブラリ名指定
target_link_libraries(${LIBNAME}
PRIVATE
opencv_world440
opencv_world440d
  )

# MAINの作成
set(MAIN "bnuts")
# プロジェクト名を指定する
project(${MAIN}  CXX)
add_executable(${MAIN} main.cpp)
# インクルードパスを指定
target_include_directories(
  ${MAIN}
  PRIVATE "${OpenCVDIR}\\include"
)
# ライブラリディレクトリの指定
target_link_directories(${MAIN} PRIVATE "${OpenCVDIR}\\x64\\vc15\\lib\\" )
# リンクするライブラリ名指定
target_link_libraries(${MAIN}
PRIVATE
opencv_world440
opencv_world440d
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
  PRIVATE "${OpenCVDIR}\\include"
)
# ライブラリディレクトリの指定
target_link_directories(${PYBINDPROJ} PRIVATE "${OpenCVDIR}\\x64\\vc15\\lib\\" )
# リンクするライブラリ名指定
target_link_libraries(${PYBINDPROJ}
PRIVATE
opencv_world440
opencv_world440d
${LIBNAME}
)