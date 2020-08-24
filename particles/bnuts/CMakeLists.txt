cmake_minimum_required(VERSION 3.8)
set(MAIN "bnuts")
# プロジェクト名を指定する
project(${MAIN}  CXX)

# OpenCV
set("OpenCVDIR" "F:\\libs\\opencv\\opencv\\build")

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(libs display.cpp simulator.cpp ball.cpp linked_list.cpp b_nuts_env.cpp)
# インクルードパスを指定
target_include_directories(
  libs
  PRIVATE "${OpenCVDIR}\\include"
)
# ライブラリディレクトリの指定
target_link_directories(libs PRIVATE "${OpenCVDIR}\\x64\\vc15\\lib\\" )
# リンクするライブラリ名指定
target_link_libraries(libs
PRIVATE
opencv_world440
opencv_world440d
  )


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
libs
  )