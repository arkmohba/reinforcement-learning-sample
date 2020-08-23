# 概要

粉体のシュミレーションプログラム（ブラジルナッツ効果のシミュレーション）

# 修正方針

* とりあえずOpenCV化する 済
* リファクタ
* 強化学習で使える環境にする
  * 必要なIFを調査


# 必要なライブラリ

* OpenCV
  * 画像化

# ビルド手順

```
$ mkdir build
$ cd build
$ cmake .. 
$ cmake --build .
```

build/Debugにexeがあるはず。また、windowsの場合はslnファイルがbuildディレクトリにあるため、ダブルクリック後、ソリューションエクスプローラーでbnutsを「スタートアッププロジェクトに設定」すればVisual Studio上で開発・デバッグできる。

# メモ

* windowsだとCRLFじゃないと正常にビルドできない。仕方ないのでCRLFで実装する。
