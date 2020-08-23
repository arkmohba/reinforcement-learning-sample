cmake_minimum_required(VERSION 3.8)
set(TARGET "bnuts")
# プロジェクト名を指定する
project(${TARGET}  CXX)

# OpenCV
set("OpenCVDIR" "F:\\libs\\opencv\\opencv\\build")

set(CMAKE_CXX_STANDARD 14)
add_executable(${TARGET} main.cpp display.cpp simulator.cpp ball.cpp linked_list.cpp b_nuts_env.cpp)
# インクルードパスを指定
set(CMAKE_INCLUDE_CURRENT_DIR ON)
target_include_directories(
  ${TARGET} 
  PRIVATE "${OpenCVDIR}\\include"
)
# ライブラリディレクトリの指定
target_link_directories(${TARGET} PRIVATE "${OpenCVDIR}\\x64\\vc15\\lib\\" )



# リンクするライブラリ名指定
target_link_libraries(${TARGET}
PRIVATE
opencv_world440
opencv_world440d
  )