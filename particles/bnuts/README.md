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
  * cmakeでfind_packageを使うので事前にOpenCV_DIR環境変数を設定しておくこと。パスはOpenCVConfig.cmakeがあるディレクトリで、だいたいbuild配下または、build/install配下。
  * また、実行時ライブラリも使うのでpathを設定しておくこと。Windowsの設定例は `%OpenCV_DIR%\x64\vc14\bin` でdllファイルが配置されている。linuxだとsoファイル？があるところ。パスが通ってなければ実行時にエラーが出るのでわかるはず。

# ビルド手順

```
$ mkdir build
$ cd build
$ cmake -A x64 .. 
$ cmake --build .
```

build/Debugにexeがあるはず。終了時はコンソールでCtrl+C。

また、windowsの場合はslnファイルがbuildディレクトリにあるため、ダブルクリック後、ソリューションエクスプローラーでbnutsを「スタートアッププロジェクトに設定」すればVisual Studio上で開発・デバッグできる。

# メモ

* windowsだとCRLFじゃないと正常にビルドできない。仕方ないのでCRLFで実装する。gitのクローン、プル時にはできればautocrlfを有効にしておくこと。
