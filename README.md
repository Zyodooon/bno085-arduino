# BNO085 bring-up

Arduino IDEでBNO085のSPI通信と各種センサーレポートを確認するためのリポジトリです。Adafruit BNO08xライブラリを使います。

## ファイル構成

```text
bno085-bringup/
├── docs/                         データシートと日本語資料
├── sketches/
│   ├── README.md                 スケッチ一覧と設定説明
│   ├── custom/                   自作の単独取得スケッチ（6種類）
│   └── modified_examples/        公式exampleをSPI用に変更したサンプル（3種類）
└── third_party/Adafruit_BNO08x/  Adafruit公式ライブラリ（submodule）
```

## スケッチ

- `sketches/custom/`にはaccelerometer、gyro、magnetometer、quaternion、game_quaternion、geomagnetic_quaternionがあります。
- `sketches/modified_examples/`にはrotation vector、Yaw/Pitch/Roll、複数レポートの参考サンプルがあります。
- 各スケッチの説明と設定項目は[sketches/README.md](sketches/README.md)を参照してください。

## Arduino Uno R4とAdafruit BNO085のSPI配線

| BNO085基板の端子 | Uno R4 |
|---|---|
| SCL / SCK | D13（SCK） |
| SDA / SO | D12（MISO） |
| DI / SI | D11（MOSI） |
| CS | D10 |
| INT | D2 |
| RST | D7 |
| VIN | 5V |
| GND | GND |
| P0 / P1 | 3.3V（SPIモード） |

SCK/MISO/MOSIはUno R4の標準SPI端子です。自作スケッチのCS/INT/RST初期値は10/2/7です。別のピンに接続する場合は、スケッチ先頭の定義も変更してください。P0/P1をSPIモードに設定してからセンサーへ給電します。電源端子は使用するブレークアウト基板の仕様を確認してください。

## GY-ICM20948V2基板の参考配線

次の配線は、写真で確認した紫色のGY-ICM20948V2（ICM-20948）基板向けです。BNO085とは別のセンサーで、このリポジトリのBNO085用スケッチでは動作しません。

| GY-ICM20948V2の印字（SPI信号） | Uno R4 |
|---|---|
| VCC（電源） | 3.3V |
| GND（グラウンド） | GND |
| SCL（SCK） | D13（SCK） |
| SDA（SDI / MOSI） | D11（MOSI） |
| ADO（SDO / MISO） | D12（MISO） |
| NCS（CS） | D10（SS / CS） |
| INT（割り込み） | D2 |
| EDA、ECL、FSYNC | 未接続 |

SPIモードではSDAがセンサーへの入力（SDI/MOSI）、ADOがセンサーからの出力（SDO/MISO）です。電源電圧は基板の仕様を確認してください。

## Arduino IDEで実行する

1. Arduino IDEのボードマネージャーでArduino Uno R4に対応するボードを選び、接続したボードとポートを指定します。
2. ライブラリマネージャーから`Adafruit BNO08x`と依存ライブラリ`Adafruit BusIO`をインストールします。
3. 使うスケッチフォルダー内の同名`.ino`を開きます。例えばジャイロは`sketches/custom/gyro/gyro.ino`です。
4. 配線とスケッチ冒頭の設定を確認して「検証」し、「書き込み」を実行します。
5. シリアルモニタを115200 baudで開きます。

公式ライブラリをsubmodule込みでcloneする場合:

```bash
git clone --recurse-submodules <このリポジトリのURL>
```

すでにclone済みなら、リポジトリのルートで実行します。

```bash
git submodule update --init --recursive
```

## スケッチの主な設定

自作スケッチでは、先頭の`#define`でセンサーへの要求周期や表示を設定できます。

| 設定 | 内容 | 初期値 |
|---|---|---|
| `REPORT_INTERVAL_US` | センサーへのレポート要求間隔（µs）。5000は5 ms、要求値であり実受信周期はレポート種別にもよります | 5000 |
| `PRINT_INTERVAL_MS` | センサーデータをシリアルモニタに表示する最短間隔 | 100 |
| `PLOT_INTERVALS` | `0`はセンサー値を表示、`1`は受信間隔（µs）をSerial Plotterへ出力 | 0 |
| `BNO08X_CS` / `BNO08X_INT` / `BNO08X_RESET` | SPIのCS・INT・RSTに接続したArduinoピン | 10 / 2 / 7 |

`PLOT_INTERVALS`は6つの自作スケッチにあります。通常は初期値`0`のまま書き込み、シリアルモニタでセンサー値を確認します。受信周期を調べるときは`1`に変更して書き込み、シリアルプロッタを開きます。出力される`interval_us`はArduinoがレポートを受け取った間隔で、センサー内部の測定時刻を直接測った値ではありません。`REPORT_INTERVAL_US`で要求した周期と実際の受信周期は一致しない場合があります。

## 資料

- [各スケッチの説明・設定](sketches/README.md)
- [BNO085の機能・通信・Arduino動作確認（日本語）](docs/bno085-summary-ja.md)
- [Adafruit BNO085基板ガイド](docs/adafruit-bno085-guide.pdf)
- [CEVA BNO08xデータシート](docs/bno08x-ceva-datasheet.pdf)
- [Adafruit BNO08xライブラリ](https://github.com/adafruit/Adafruit_BNO08x)

公式ライブラリの著作権表示と利用条件は[third_party/Adafruit_BNO08x/license.txt](third_party/Adafruit_BNO08x/license.txt)を参照してください。
