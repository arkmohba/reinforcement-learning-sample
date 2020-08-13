# 概要

粉体のシュミレーションプログラム

# 修正方針

* とりあえずOpenCV化する 済
* リファクタ
* 強化学習で使える環境にする
  * 必要なIFを調査


# ビルド手順

```
$ mkdir build
$ cd build
$ cmake .. 
$ cmake --build .
```

build/Debugにexeがあるはず。

# メモ

WindowsだとCRLFじゃないと正常にビルドできない。仕方ないのでCRLFで実装する。