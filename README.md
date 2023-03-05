# felica-rpi2040-lcd

![](https://www.waveshare.com/media/catalog/product/cache/1/image/800x800/9df78eab33525d08d6e5fb8d27136e95/r/p/rp2040-lcd-0.96-1.jpg)

FeliCa リーダー・ライターRS-S620Sを使用したICカード残高リーダです。

nanaco等のプリペイド型ICカードやSuica等の交通系ICカードの残高を読み取り、LCD画面に表示することが出来ます。

対応しているICカード

* Suica（モバイルsuica含む）
* PASMO
* ICOCA
* Kitaca
* toica
* SUGOCA
* PiTaPa
* Edy
* nanaco（nanacoモバイル含む）
* WAON

# 使用した機材

* Waveshare RP2040-LCD-0.96
https://www.switch-science.com/products/7887
* FeliCa リーダー・ライター RC-S620S  
https://www.switch-science.com/catalog/353/
* FeliCa RC-S620S/RC-S730 ピッチ変換基板のセット(フラットケーブル付き)  
https://www.switch-science.com/catalog/1029/

## デバイスとの接続

デバイスは以下のように接続してください。

|RP2040-LCD-0.96|RC-S602S|
|---|---|
|3V3(OUT) (pin 36)|VDD (pin 1)|
|GND (pin 23)|GND (pin 4, 6)|
|GPIO16 (pin 21)|RXD (pin 2)|
|GPIO17 (pin 22)|TXD (pin 3)|

# 必要なツールのインストール

## Mbed CLI 1
Mbed CLIは、以下のドキュメントを参照にインストールしてください。コンパイラは、GCC Arm Embedded Compiler を使用して動作確認しています。  
https://os.mbed.com/docs/mbed-os/v6.12/build-tools/install-and-set-up.html

## picotool
以下のサイトを参考にしてください。  
https://github.com/raspberrypi/picotool

# プログラムのビルドと書き込み

## Mbed CLI でビルドする
以下のコマンドでリポジトリをクローンして、ビルドします。

```
$ mbed import　https://github.com/toyowata/felica-rpi2040-lcd
$ cd felica-rpi2040-lcd
$ mbed compile -m raspberry_pi_pico -t gcc_arm
```

## RP2040-LCD-0.96 に書き込む
BOOTSELモードに設定し（BOOTボタンを押しながらRESETボタンを放す）、以下のコマンドを実行します。

```
$ picotool load ./BUILD/RASPBERRY_PI_PICO/GCC_ARM/felica-rpi2040-lcd.bin
```

## プログラムの実行

プログラム書き込み後、USBケーブルを抜き差しするかリセットボタンを押してプログラムを起動します。  
ICカードをFeliCaリーダー・ライター上にかざすと、カード残高が表示されます。
