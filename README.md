# BNO085の動作確認用のrepo

STM32とBNO085のSPI通信を、公式サンプルを基に動作確認するためのリポジトリです。

## SPIサンプル

- `src/bno085_rotation_vector_spi_reference.ino`：Game Rotation Vectorのクォータニオン出力
- `src/bno085_quaternion_yaw_pitch_roll_spi_reference.ino`：クォータニオンからYaw/Pitch/Rollへ変換
- `src/bno085_multi_report_spi_reference.ino`：複数のセンサーレポートを出力

Arduino IDEでは使用するスケッチ1つだけを、ファイル名と同名のフォルダーに入れて開いてください。3つを同じスケッチとしてコンパイルすると、`setup()`などが重複します。ライブラリは`Adafruit BNO08x`とその依存ライブラリをインストールします。

各スケッチは標準の`SPI`を使います。SCK/MISO/MOSIは選択したSTM32ボードのデフォルトSPI端子に接続してください。

| BNO085（Adafruit基板） | マイコン側 |
|---|---|
| SCL / SCK | SCK |
| SDA / SO | MISO |
| DI / SI | MOSI |
| CS | `BNO08X_CS`（初期値10） |
| INT | `BNO08X_INT`（初期値9） |
| RST | `BNO08X_RESET`（初期値5） |
| GND | GND |

CS/INT/RSTの初期値はサンプル用です。使用するSTM32ボードと実際の配線に合わせて、各スケッチ冒頭の定義を変更してください。電源は使用基板の仕様に従って接続します。Adafruit基板ではP0（PS0）とP1（PS1）を両方HighにしてSPIモードを選び、設定後に電源を入れ直してください。シリアルモニタは115200 baudです。

## 資料

- [Adafruit BNO085基板ガイド（PDF）](docs/adafruit-bno085-guide.pdf)
- [BNO085の機能・通信・Arduino動作確認（日本語）](docs/bno085-summary-ja.md)
