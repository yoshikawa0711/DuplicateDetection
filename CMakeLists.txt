cmake_minimum_required(VERSION 2.8)

# ソリューションファイルの名前を指定する
project(DuplicateDetection )

set(duplicate_detection_src
  main.cpp
  Image.cpp
)

add_executable(DuplicateDetection ${duplicate_detection_src})

#unicodeの設定
add_definitions(-D_UNICODE)

# OpenCVのパッケージを探す
find_package(OpenCV REQUIRED PATHS "C:/opencv/build")

# OpenCVが見つかった場合のみ設定を行う
if(OpenCV_FOUND)
  # インクルードパスを指定
  include_directories(${OpenCV_INCLUDE_DIRS})

  # リンクするライブラリ指定
  target_link_libraries(DuplicateDetection ${OpenCV_LIBS})
endif(OpenCV_FOUND)
