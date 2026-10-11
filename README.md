# BNO085の動作確認用のrepo

Arduino IDEを使ってBNO08xセンサーとのSPI通信、取得周期、測定精度を確認するためのリポジトリです。Adafruit BNO08xライブラリの公式exampleを基にしています。

## ファイル・ディレクトリ構成

```text
bno085-bringup/
├── README.md                          このリポジトリの使い方
├── .gitignore                         Git管理から除外する設定
├── .gitmodules                        公式リポジトリのsubmodule設定
├── docs/                              データシート・参考資料
│   ├── adafruit-bno085-guide.pdf
│   ├── bno08x-ceva-datasheet.pdf
│   └── bno085-summary-ja.md
├── sketches/                          動作確認用スケッチ・SPI参考サンプル
│   ├── README.md                      各スケッチの説明・設定方法
│   ├── accelerometer/
│   │   └── accelerometer.ino          加速度
│   ├── gyro/
│   │   └── gyro.ino                   角速度
│   ├── magnetometer/
│   │   └── magnetometer.ino           磁場
│   ├── quaternion/
│   │   └── quaternion.ino             Rotation Vector
│   ├── game_quaternion/
│   │   └── game_quaternion.ino        Game Rotation Vector
│   ├── geomagnetic_quaternion/
│   │   └── geomagnetic_quaternion.ino Geomagnetic Rotation Vector
│   ├── rotation_vector_spi_reference/
│   │   └── rotation_vector_spi_reference.ino
│   ├── quaternion_yaw_pitch_roll_spi_reference/
│   │   └── quaternion_yaw_pitch_roll_spi_reference.ino
│   └── multi_report_spi_reference/
│       └── multi_report_spi_reference.ino
└── third_party/
    └── Adafruit_BNO08x/               Adafruit公式リポジトリ（submodule）
        ├── README.md
        ├── license.txt               公式コードのライセンス
        ├── src/                      公式ライブラリ本体
        ├── examples/                 未変更の公式サンプル
        │   ├── rotation_vector/
        │   │   └── rotation_vector.ino
        │   ├── quaternion_yaw_pitch_roll/
        │   │   └── quaternion_yaw_pitch_roll.ino
        │   ├── more_reports/
        │   │   └── more_reports.ino
        │   └── ...
        └── ...
```

Git内部の管理ファイルとローカル環境用のディレクトリは省略しています。公式リポジトリ内は主要なファイル・ディレクトリのみ記載しています。

## GY-ICM20948V2基板とArduino Uno R4の配線

次の表は、写真の紫色の **GY-ICM20948V2（ICM-20948）基板**をUno R4へSPI接続する場合の配線です。Adafruit BNO085基板とは別のセンサー基板なので、下記のBNO085用スケッチは適用できません。

| GY-ICM20948V2基板の印字（SPI信号名） | Arduino Uno R4 |
|---|---|
| VCC（電源） | 3.3V |
| GND（グラウンド） | GND |
| SCL（SCK） | D13（SCK） |
| SDA（SDI / MOSI） | D11（MOSI） |
| ADO（SDO / MISO） | D12（MISO） |
| NCS（CS） | D10（SS / CS） |
| INT（割り込み） | D2 |
| EDA、ECL、FSYNC | 未接続 |

SPIではSDAがセンサーへのデータ入力（SDI/MOSI）、ADOがセンサーからのデータ出力（SDO/MISO）です。電源電圧は基板の仕様を確認してください。

## スケッチの構成

| フォルダー | 内容 |
|---|---|
| `sketches/` | 加速度・ジャイロ・磁気・クォータニオンをデータごとに単独取得する動作確認用スケッチ。取得周期を変更したときの挙動や測定精度を確認するため、受信値を単位付きで1サンプル1行に表示します。公式exampleをSPI用に変更した参考サンプルも含みます。 |
| `third_party/Adafruit_BNO08x/` | Adafruit公式リポジトリをsubmoduleで管理。未変更の公式サンプルは`examples/`、公式ライセンスは`license.txt`を参照します。 |

## 公式サンプルの取得

公式リポジトリ [Adafruit_BNO08x](https://github.com/adafruit/Adafruit_BNO08x) をsubmoduleとして取り込み、使用するコミットを固定しています。cloneするときは次のように取得してください。

```bash
git clone --recurse-submodules <このリポジトリのURL>
```

すでにclone済みの場合は、リポジトリのルートで実行します。

```bash
git submodule update --init --recursive
```

公式サンプルは [third_party/Adafruit_BNO08x/examples/](third_party/Adafruit_BNO08x/examples/) にあります。`rotation_vector`、`quaternion_yaw_pitch_roll`、`more_reports`の各フォルダー内の同名`.ino`をArduino IDEで開けます。ライブラリと依存ライブラリはArduino IDEのライブラリマネージャーでインストールしてください。

公式コードの著作権表示・利用条件は同梱の [license.txt](third_party/Adafruit_BNO08x/license.txt) を参照してください。自分用に変更するスケッチは`sketches/`で管理します。

`sketches/`には校正済みの加速度・角速度・磁場と、3種類のクォータニオンの計6スケッチと、公式exampleをSPI接続用に変更した3つの参考サンプルがあります。単独取得の6スケッチでは校正と単位換算はセンサー・ライブラリが行い、角度への変換などは追加しません。

個別のセンサーデータを見やすく確認する新しいスケッチは、[sketches/README.md](sketches/README.md)を参照してください。各スケッチはArduino IDE用の同名フォルダーに入っており、取得周期・表示周期・baud rateを先頭の`#define`で変更できます。

## SPI参考サンプル

- [rotation_vector_spi_reference.ino](sketches/rotation_vector_spi_reference/rotation_vector_spi_reference.ino)：Game Rotation Vectorのクォータニオン出力
- [quaternion_yaw_pitch_roll_spi_reference.ino](sketches/quaternion_yaw_pitch_roll_spi_reference/quaternion_yaw_pitch_roll_spi_reference.ino)：クォータニオンからYaw/Pitch/Rollへ変換
- [multi_report_spi_reference.ino](sketches/multi_report_spi_reference/multi_report_spi_reference.ino)：複数のセンサーレポートを出力

Arduino IDEでは使用するサンプルのフォルダー内にある同名の`.ino`を開いてください。各サンプルは単独で使用します。ライブラリは`Adafruit BNO08x`とその依存ライブラリをインストールします。

各スケッチは標準の`SPI`を使います。SCK/MISO/MOSIは選択したボードのデフォルトSPI端子に接続してください。

| BNO085（Adafruit基板） | マイコン側 |
|---|---|
| SCL / SCK | SCK |
| SDA / SO | MISO |
| DI / SI | MOSI |
| CS | `BNO08X_CS`（初期値10） |
| INT | `BNO08X_INT`（初期値2） |
| RST | `BNO08X_RESET`（初期値7） |
| GND | GND |

CS/INT/RSTの初期値はサンプル用です。使用するボードと実際の配線に合わせて、各スケッチ冒頭の定義を変更してください。電源は使用基板の仕様に従って接続します。Adafruit基板ではP0（PS0）とP1（PS1）を両方HighにしてSPIモードを選び、設定後に電源を入れ直してください。シリアルモニタは115200 baudです。

## Windows + Arduino IDE

1. リポジトリをWindows側へコピーまたはcloneします。WSL側にある場合は、使うスケッチのフォルダーをWindows側へコピーします。
2. `sketches/`から確認したいデータのフォルダーを選び、中の同名`.ino`をArduino IDEで開きます。例えばジャイロなら`sketches/gyro/gyro.ino`です。
3. ライブラリマネージャーで`Adafruit BNO08x`をインストールし、依存ライブラリもインストールします。
4. 実際のボード型番と接続先のCOMポートを選択します。必要に応じてボードマネージャーから対応するボードパッケージをインストールします。
5. 配線を確認し、スケッチ先頭のピン設定・取得周期・表示周期・baud rateを合わせます。
6. 「検証」→「書き込み」を実行します。
7. シリアルモニタを開き、通信速度を`SERIAL_BAUD_RATE`と同じ値（既定115200 baud）にします。

SPI参考サンプルも`sketches/`内の同名フォルダーから開けます。Windowsへコピーする場合はフォルダーごとコピーしてください。

配線や動作の調整は、選んだスケッチ冒頭の`#define`で行います。

| 定義 | 設定内容 |
|---|---|
| `BNO08X_CS` / `BNO08X_INT` / `BNO08X_RESET` | 実際に配線したGPIO番号 |
| `SERIAL_BAUD_RATE` | シリアル通信速度。Arduino IDEのシリアルモニタの通信速度も合わせる |
| `REPORT_INTERVAL_US` | `sketches/`とrotation_vector / multi_reportのレポート要求周期（µs） |
| `PRINT_INTERVAL_MS` | `sketches/`の表示する最短間隔（ms）。取得周期とは別に設定 |
| `DECIMAL_PLACES` | `sketches/`の小数点以下の表示桁数 |
| `LOOP_DELAY_MS` | rotation_vector / multi_reportのループ待ち時間（ms） |
| `FAST_MODE` | Yaw/Pitch/Rollで0なら安定化姿勢、1なら高速姿勢 |
| `STABILIZED_REPORT_INTERVAL_US` / `FAST_REPORT_INTERVAL_US` | 各姿勢モードのレポート要求周期（µs） |
| `OUTPUT_IN_DEGREES` | Yaw/Pitch/Rollで`true`なら度、`false`ならラジアン |

SCK/MISO/MOSIは標準の`SPI`を使うため、ボード標準の端子へ配線します。PS0/PS1はセンサー基板側の配線で設定します。CS/INT/RSTは全スケッチで10/2/7に揃えています。実際の配線に合わせて冒頭の定義を確認してください。

シリアルモニタは既定115200 baudです。ポートが見つからない場合は、WindowsのデバイスマネージャーでCOM番号を確認し、Arduino IDEで該当ポートを選択します。別アプリが同じポートを使用している場合は、そのアプリのシリアルモニタを閉じてください。

`BNO08x Found!`が出ればセンサー認識成功です。`Failed to find BNO08x chip`の場合はSPI配線、CS/INT/RST、電源、PS0/PS1の設定を確認します。

同梱のAdafruit資料ではUno/LeonardoはRAM不足とされています。Uno R3でこのライブラリを使う前提にはできません。Uno R4は別のボードなので、正確な型番に合わせて設定してください。



## 資料リンク

- [CEVA BNO08xデータシート（PDF）](docs/bno08x-ceva-datasheet.pdf)
- [Adafruit BNO085基板ガイド（PDF）](docs/adafruit-bno085-guide.pdf)
- [BNO085の機能・通信・Arduino動作確認（日本語）](docs/bno085-summary-ja.md)
