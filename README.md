# PSX_SWitch Pro-con
PlayStation1, PlayStation2 コントローラを Raspberry Pi Picoを介して Nintendo Pro Controller のようにエミュレートをするものです。

mzyy94さんの解析資料を多いに参照させていただき、作成しました。

オマケとして、PS2用タタコンを Switch版太鼓の達人で何とか使えるようにするモードも搭載しています。

# 材料
1. PlayStation1/2 コントローラ (SCPH-110, SCPH-10010など)
1. PlayStationコントローラ延長ケーブルなど [例](https://www.amazon.co.jp/third-party-PS1-2%E7%94%A8%E3%82%B3%E3%83%B3%E3%83%88%E3%83%AD%E3%83%BC%E3%83%A9%E3%83%BC%E5%BB%B6%E9%95%B7%E3%82%B1%E3%83%BC%E3%83%96%E3%83%AB/dp/B00C0NZWUI)
1. 1kΩ抵抗2本、配線
1. 3端子スライドスイッチ (タタコンモードとの切り替えが必要な場合)
1. Raspberry Pi Pico および USBケーブル


# 準備
## Raspberry Pi Pico のファームウェア書き込み
Raspberry Pi Pico のBOOTSELボタンを押しながらPCにUSB接続し、`ps_switch.uf2`ファイルを書き込みます

## ハードウェア準備
[回路図(PSX-USB.Converter.pdf)](https://github.com/beijingduckx/psx_cyber_usb/releases/tag/release_1_0_1)にしたがって、PlayStationコントローラ延長ケーブルと Raspberry Pi Pico を接続します

# 使い方
## 接続
1. PlayStation延長ケーブルに、PlayStationコントローラを接続します
1. Raspberry Pi Pico を Switch に接続します

## 操作
回路中の MODE SWの設定によって、操作が変わります

### PlayStationコントローラ アナログモード
#### MODE SW = GNDの場合 (Pro Controllerモード)

| PlayStation | Switch|
|-------------|------|
|LEFT ANALOG | LEFT ANALOG |
|RIGHT ANALOG | RIGHT ANALOG|
|LEFT | LEFT|
|RIGHT | RIGHT|
|UP | UP|
|DOWN| DOWN|
|□ | Y|
|△| X|
|○| A|
|×| B|
|L1| L|
|R1| R|
|L2| SL|
|R2| SR|
|SELECT|HOME|
|START|+|


#### MODE SW = HIGH の場合 (タタコンモード)

| PlayStation | PS2タタコン | Switch|
|-------------|------|------|
|LEFT ANALOG | -|LEFT ANALOG |
|RIGHT ANALOG | -|RIGHT ANALOG|
|LEFT |面-左  |RIGHT|
|○|面-右|B |
|L1 |ふち-左|LEFT|
|R1|ふち-右|A |
|SELECT|SELECT|HOME|
|START|START|DOWN|

つまり、
* 左のふちと面で、左右
* 右のふちと面で、決定・キャンセル

です。割り当てが独特ですが、ご了承ください。

太鼓の達人 ドンダフルフェスティバル 体験版で、タタコン操作を選んだ時に演奏ゲームができる程度の確認のみです  
PS2のタタコンでは選択できない項目や、遊べない内容があるかもしれませんが、ご了承ください。

## 留意点
- PS1/2のアナログスティックは、センターが出にくいようなので、非活性エリア(dead zone)を広めにとってあります  
  アナログスティックを少し多めに倒さないと、効きはじめないかもしれません  
  (Switchでは、dead zoneの設定は、意味がないかもしれません)
- コントローラがSwitchに認識されたら、PlayStationコントローラのANALOGモードを有効にし、両方のアナログスティックを一回転させることをおすすめします  
  特に、デジタルパッドの方向キーの動きがおかしい(メニューなどの操作で一方向に押しっぱなしにしても押しっぱなしにならない等)ときは、この操作をしてみてください  
- 本機を2台以上Switchに接続した場合の動作は、確認していません

## 動作確認済みPlayStation1/2コントローラ
- SCPH-110
- SCPH-10010
- NPC-107 (PS2タタコン)


# 非保証
- 本リポジトリ内のプログラム、回路図は、正常に動作することを期待して作成していますが、正常な動作を保証しません  
- 本リポジトリ内のプログラム・回路図を参照・利用したことにより生じた損害(Switchが破損する、Raspberry Pi Picoが破損する、PlayStationコントローラが破損するなど)に対し、制作者は一切補償しません  
- 制作時は他の資料も参照し、回路図に誤りがないかどうか確認しながら行ってください

# 補足
## プログラムについて
このプログラムは、[Raspberry Pi Picoのサンプルプログラム](https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_hid_composite)をベースに制作しています

コンパイルは、上記のサンプルプログラムと同様に行います。

# 参考文献
- https://www.mzyy94.com/blog/2020/03/20/nintendo-switch-pro-controller-usb-gadget/
- https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering
- https://wiki.handheldlegend.com/nintendo-switch-bluetooth-controller-protocol
- https://github.com/chromium/chromium/blob/main/device/gamepad/nintendo_controller.cc
