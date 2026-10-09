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

## Windows + VS Code + PlatformIO

1. WindowsのVS Codeに拡張機能「PlatformIO IDE」をインストールします。
2. このリポジトリをWindows側へコピーまたはcloneし、`platformio.ini`があるフォルダーをVS Codeで開きます。
3. `platformio.ini`の`[env]`に、使用ボードの`platform`と`board`を設定します。現時点では型番未確定のためコメントにしてあり、そのままではビルドできません。Arduino Mega 2560の場合は次の2行です。他のボードには流用しないでください。

   ```ini
   platform = atmelavr
   board = megaatmega2560
   ```

4. 上記の配線表と実際のボードのSPI端子を確認し、選んだスケッチのCS/INT/RSTを合わせます。
5. データ通信対応USBケーブルでボードをWindows PCに接続します。
6. PlatformIOの「Project Tasks」で、実行したい環境の「Build」→「Upload」→「Monitor」を実行します。初回ビルド時に`Adafruit BNO08x`と依存ライブラリが自動取得されます。

| 環境名 | 出力 |
|---|---|
| `quaternion_yaw_pitch_roll`（既定） | Yaw / Pitch / Roll |
| `rotation_vector` | Game Rotation Vectorのクォータニオン |
| `multi_report` | 複数のセンサーレポート |

PlatformIOでは`platformio/main.cpp`から選んだスケッチ1つだけを読み込みます。既存の`src/*.ino`はArduino IDEでも引き続き使えます。

配線や動作の調整は、選んだスケッチ冒頭の`#define`で行います。

| 定義 | 設定内容 |
|---|---|
| `BNO08X_CS` / `BNO08X_INT` / `BNO08X_RESET` | 実際に配線したGPIO番号 |
| `SERIAL_BAUD_RATE` | シリアル通信速度。変更時は`platformio.ini`の`monitor_speed`も合わせる |
| `REPORT_INTERVAL_US` | rotation_vector / multi_reportのレポート要求周期（µs） |
| `LOOP_DELAY_MS` | rotation_vector / multi_reportのループ待ち時間（ms） |
| `FAST_MODE` | Yaw/Pitch/Rollで0なら安定化姿勢、1なら高速姿勢 |
| `STABILIZED_REPORT_INTERVAL_US` / `FAST_REPORT_INTERVAL_US` | 各姿勢モードのレポート要求周期（µs） |
| `OUTPUT_IN_DEGREES` | Yaw/Pitch/Rollで`true`なら度、`false`ならラジアン |

SCK/MISO/MOSIは標準の`SPI`を使うため、ボード標準の端子へ配線します。PS0/PS1はセンサー基板側の配線で設定します。`multi_report`のCS/INTの値は他のサンプルと異なるため、サンプルを切り替える際も冒頭を確認してください。

シリアルモニタは115200 baudです。ポートが自動検出されない場合は、WindowsのデバイスマネージャーでCOM番号を確認し、`platformio.ini`の`upload_port`と`monitor_port`のコメントを外して指定します。書き込み前にArduino IDEなど別アプリのシリアルモニタを閉じてください。

`BNO08x Found!`が出ればセンサー認識成功です。`Failed to find BNO08x chip`の場合はSPI配線、CS/INT/RST、電源、PS0/PS1の設定を確認します。

同梱のAdafruit資料ではUno/LeonardoはRAM不足とされています。Uno R3でこのライブラリを使う前提にはできません。Uno R4は別のボードなので、正確な型番に合わせて設定してください。

設定の参考：[PlatformIO IDE](https://docs.platformio.org/en/latest/integration/ide/vscode.html)、[ライブラリの自動取得](https://docs.platformio.org/en/latest/projectconf/sections/env/options/library/lib_deps.html)。

## 資料リンク

- [Adafruit BNO085基板ガイド（PDF）](docs/adafruit-bno085-guide.pdf)
- [BNO085の機能・通信・Arduino動作確認（日本語）](docs/bno085-summary-ja.md)
