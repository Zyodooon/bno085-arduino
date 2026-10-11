# BNO085の機能・通信・Arduino動作確認

[Adafruit BNO085基板ガイド（PDF）](adafruit-bno085-guide.pdf)の日本語要約です。配線と電源の説明はAdafruit基板を対象とします。

## 概要

BNO085は、加速度計・ジャイロ・地磁気センサーの値を読み取るだけでなく、内部のHillcrest SH-2ファームウェアがセンサー融合を行い、姿勢（クォータニオンなど）や動作判定を出力するIMUです。Arduino側では通常、Adafruitのライブラリを使い、必要なレポートを有効化してデータを読みます。

- **姿勢をしっかり使いたい**：I2C/UART/SPIの通常モード＋`Adafruit_BNO08x`ライブラリ。`Rotation Vector`などを選びます。
- **簡単な角度・加速度だけでよい**：UART-RVCモード。115200 baudでYaw/Pitch/Rollと加速度を受信します。
- 「コマンドを1つ送って値を読む」単純なセンサーではありません。通常モードではSH-2/SHTPというパケット通信をライブラリが扱います。アプリ側が主に行う操作は、接続、レポート有効化、イベント読み出しです。

## 何ができるか

### 基本センサー値

| レポート | 内容 | 単位・備考 |
|---|---|---|
| 加速度ベクトル | 重力＋移動による加速度、X/Y/Z | m/s² |
| 角速度ベクトル | X/Y/Z軸まわりの回転速度 | rad/s |
| 磁場ベクトル | X/Y/Zの磁場 | µT |
| 線形加速度 | 重力成分を除いた加速度 | m/s² |
| 重力ベクトル | 動きの成分を除いた重力方向 | 重力ベクトルレポート |

### 姿勢（センサー融合）

| レポート | 使うセンサー | 特徴 |
|---|---|---|
| Rotation Vector（絶対姿勢） | 加速度＋ジャイロ＋磁気 | クォータニオン。磁北と重力を基準にした姿勢。精度重視の標準候補 |
| Game Rotation Vector | 加速度＋ジャイロ | 磁気センサーを使わず、磁気補正による急なジャンプを避けたい用途向け |
| Geomagnetic Rotation Vector | 加速度＋磁気 | 低消費電力を重視。応答性・精度は通常のRotation Vectorより妥協がある |
| AR/VR向けRotation Vector | 用途に応じた融合 | 低遅延・低電力などの用途最適化レポート |

クォータニオンは4成分で姿勢を表し、角度の単純な足し引きで起きる特異点を避けやすい表現です。Euler角（Yaw/Pitch/Roll）が必要なら、受信した姿勢から変換するか、UART-RVCを利用します。

### 動作判定など

安定状態分類（On table / Stable / Motion）、タップ、歩数検出・歩数カウンター、活動分類、シェイク検出、Significant Motionなどのレポートがあります。利用できるレポートとライブラリ対応状況は個別に確認してください。

## 「コマンド」はある？

通常のArduino利用者が直接送るASCIIコマンド群は、Adafruit資料では示されていません。BNO085はSH-2センサー・ハブのプロトコルで動作し、そのパケット層にSHTPを使います。通信には初期化・機能設定・センサーレポート受信などの制御メッセージがありますが、通常はAdafruitライブラリに任せます。

Arduinoコード上の主な操作は以下です。

1. `begin_I2C()` / `begin_UART()` / `begin_SPI()`で接続を開始
2. `enableReport(SH2_GAME_ROTATION_VECTOR)`などで必要なレポートを有効化
3. `getSensorEvent(&sensorValue)`で受信イベントを読む
4. `sensorValue.sensorId`を見て対象の値を取り出す

つまり、最初に決めるのは「通信モード」と「どのレポートを使うか」です。生のプロトコルパケットを自作するのは、ライブラリを使う通常のArduino利用では不要です。

## 最初に選ぶなら

- **手早く角度を見たい**：UART-RVC、115200 baud、RVCライブラリ。
- **クォータニオンを使って姿勢推定したい**：通常モード＋Adafruit BNO08x。対応ボードならI2C、ESP32系ならUART/SPIを検討。
- **ロボットの移動・傾き制御**：Game Rotation Vectorまたは加速度/ジャイロ/線形加速度を試し、実際の応答と座標系を確認。

## 通信方式

Adafruit基板のP0（PS0）/P1（PS1）でモードを選びます。以下の電源・端子の説明はAdafruit基板向けです。他社基板やIC単体では、その回路図とデータシートを確認してください。

| PS1 | PS0 | モード | Arduino側での扱い |
|---|---|---|---|
| Low | Low | I2C | `begin_I2C()` |
| Low | High | UART-RVC | `Adafruit_BNO08x_RVC`。角度・加速度を簡単に取得 |
| High | Low | UART | `begin_UART()`。通常のSH-2レポートをシリアルで取得 |
| High | High | SPI | `begin_SPI()`。CS/INT/RSTを接続 |

### I2C

- 初期アドレスは`0x4A`。DIをHighにすると`0x4B`。
- 配線：VIN、GND、SCL、SDA。ブレークアウト基板上に電圧レギュレータとレベル変換があり、VINは3～5Vに対応します。5VロジックのArduinoならVINに5Vを供給する説明です。
- Adafruit資料はBNO08xのI2C実装にプロトコル上の問題があり、ESP32/ESP32-S3やI2Cマルチプレクサで不安定になりうると注意しています。SAMD51、RP2040、STM32F4、nRF52840では比較的信頼性が高いとの記載です。ESP系ではUARTまたはSPIを検討してください。

### UART（通常モード）

- 配線：センサーSCL端子（UART受信）←Arduino TX、センサーSDA端子（UART送信）→Arduino RX。P1をHighにします（基板のモード表に従う）。
- Adafruit ArduinoライブラリのUART初期化には**300バイトを超えるUART受信バッファ**が必要と記載されています。使用ボードのハードウェアシリアルとバッファを確認してください。
- 通常モードはRVCと違い、クォータニオン等の各種SH-2レポートを選択できます。

### UART-RVC

- 配線が簡単な専用モード。P0/P1をモード表どおり設定し、SCLをArduino TX、SDAをArduino RXに接続します。
- 資料のArduino例は`Serial1.begin(115200)`。受信できる値はYaw/Pitch/RollとX/Y/Z加速度。
- 複雑なSH-2レポートを扱わずに角度を得たい場合に便利です。通常モードで選べる豊富なレポートを使う方法とは別の動作モードです。

### SPI

- 配線：SCL=SCK、SDA=MISO、DI=MOSIに加えてCS、INT、RST。
- INTはLowアクティブの通知線、RSTはLowアクティブのリセット線です。資料は安定したSPI動作のため両方の接続が必要としています。

## Arduinoで動かすときの準備

1. Arduino IDEの **Tools > Manage Libraries...** を開き、`Adafruit BNO08x`を検索してインストールします。
2. 同じ手順で`Adafruit BusIO`もインストールします（依存関係として必要です）。
3. **File > Examples > Adafruit BNO08x > rotation_vector** を開き、まずサンプルを動かします。このサンプルはBNO085に接続し、姿勢レポートを有効化して、受信した姿勢をクォータニオン（real, i, j, k）としてシリアル出力する動作確認用です。センサー認識・レポート受信・値の変化を確認できます。選ばれるレポートはサンプルの版で異なることがあるため、`enableReport(SH2_...)`を確認します。
4. サンプルの接続初期化を、配線に応じて`begin_I2C()` / `begin_UART()` / `begin_SPI()`のいずれかにします。SPIならCS・INTを実配線に合わせて指定します。
5. `setReports()`内の`enableReport(...)`で欲しいレポートを選びます。複数有効化できます。ガイド掲載時のサンプルはRotation Vectorを示していますが、ライブラリの版によってGame Rotation Vectorなどに変わることがあります。実際に有効化されている`SH2_...`を確認してください。
6. コードをコンパイルしてArduinoへ書き込みます。
7. Arduino IDEの **Tools > Serial Monitor** を開き、速度を`115200 baud`に設定するとサンプルの出力を確認できます。これはPCとArduino間の表示用シリアル通信速度で、BNO085とのSPI通信速度ではありません。

このサンプルの目的は、まずBNO085から姿勢データが取れることを確認することです。CANフレームの送信や自動操縦の制御処理は含まれないので、確認後にSTM32側のCAN送信処理へつなぎます。

### 最小構成イメージ（通常モード）

```cpp
#include <Adafruit_BNO08x.h>

Adafruit_BNO08x bno08x(-1);  // I2C/UARTなら通常RESETピンなし
sh2_SensorValue_t sensorValue;

void setup() {
  Serial.begin(115200);  // PCのシリアルモニタ用
  if (!bno08x.begin_I2C()) {
    Serial.println("BNO085が見つかりません");
    while (1) delay(10);
  }
  if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
    Serial.println("レポート設定に失敗");
  }
}

void loop() {
  if (bno08x.getSensorEvent(&sensorValue) &&
      sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR) {
    // sensorValue.un.gameRotationVector の i, j, k, real を利用
  }
}
```

これはI2CでGame Rotation Vectorを受信する流れを示す例です。SPI実装時は、接続初期化とCS・INT・RSTの指定を公式サンプルと実配線に合わせて変更します。PCのシリアルモニタは115200 baudです。

## ボード選び・注意点

- Adafruit資料はライブラリのサイズと複雑さから、SAMD21/SAMD51、nRF52、ESP、Megaなど十分なメモリを持つボードを推奨し、Arduino Uno/LeonardoはRAM不足で動作しないとしています。
- これは資料記載のライブラリ前提の注意です。Uno系でも別の軽量実装やUART-RVCなら条件が違う場合がありますが、まずは資料の推奨範囲のボードが無難です。
- I2Cはボードとの相性に注意。ESP32/ESP32-S3を使うならUART/SPIを優先候補にします。
- SPIではCS/INT/RSTの配線を省略しないでください。
- 回転ベクトルの成分の順序・座標系・軸方向は、取り付け向きやライブラリ定義に依存します。実機を回して軸と符号を確認します。
- 磁気センサーを使う絶対姿勢は周囲の磁性体・電流の影響を受けます。磁気の影響を避けたい用途ではGame Rotation Vectorも候補です。

## 出典

Adafruit「9-DOF Orientation IMU Fusion Breakout - BNO085」。収録PDFは2026年8月19日更新、全41ページです。IC単体のデータシートとは別の資料です。
