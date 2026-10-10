# SPI動作確認スケッチ

各フォルダー内の同名 `.ino` をArduino IDEで開きます。Windowsで使う場合はフォルダーごとコピーしてください。各スケッチは単独で動作し、Adafruit BNO08xと依存ライブラリが必要です。データを単独取得する6スケッチと、公式exampleをSPI用に変更した3つの参考サンプルを同じ階層に配置しています。

## 単独取得スケッチ

| スケッチ | 取得するデータ | 単位 |
|---|---|---|
| [accelerometer](accelerometer/accelerometer.ino) | 校正済み加速度（重力を含む） | m/s^2 |
| [gyro](gyro/gyro.ino) | 校正済み角速度 | rad/s |
| [magnetometer](magnetometer/magnetometer.ino) | 校正済み磁場 | uT |
| [quaternion](quaternion/quaternion.ino) | 磁気を使う姿勢クォータニオン | unitless |
| [game_quaternion](game_quaternion/game_quaternion.ino) | 磁気を使わない姿勢クォータニオン（Yawはドリフトする） | unitless |
| [geomagnetic_quaternion](geomagnetic_quaternion/geomagnetic_quaternion.ino) | 加速度・磁気による姿勢クォータニオン | unitless |

加速度・角速度・磁場はセンサー内部で校正されたレポートを取得します。クォータニオンは無次元で、w=real、x=i、y=j、z=kです。

各ファイルの先頭で次を変更できます。

| define | 内容 | 初期値 |
|---|---|---|
| `SERIAL_BAUD_RATE` | シリアル通信速度。IDE側の設定も合わせる | 115200 |
| `REPORT_INTERVAL_US` | センサーへのレポート要求周期（µs） | 5000（200Hz） |
| `PRINT_INTERVAL_MS` | 受信した値を表示する最短間隔（ms） | 100（最大10行/秒） |
| `DECIMAL_PLACES` | 小数点以下の表示桁数 | 4 |
| `BNO08X_CS` / `BNO08X_INT` / `BNO08X_RESET` | SPI制御ピン | 10 / 2 / 7 |
| `PLOT_INTERVALS` | 6つの単独取得スケッチで受信間隔をSerial Plotterへ出す（1）／通常のセンサーデータ表示（0） | 1 |

6つの単独取得スケッチ（加速度、ジャイロ、磁場、3種類のクォータニオン）では、既定で受信間隔を出力します。確認したいスケッチを書き込み、Arduino IDEの「ツール」→「シリアルプロッタ」を開いてください。`interval_us`の線が受信ごとの間隔（µs）です。200Hz要求時の目安は約5000µsです。最初のサンプルは前回時刻がないため出力しません。これはスケッチがデータを受け取った間隔で、センサー内部の測定時刻そのものではありません。通常のセンサーデータをシリアルモニタに表示するには、そのスケッチの`PLOT_INTERVALS`を`0`にします。公式exampleをSPI用に変更した3つの参考サンプルにはこの計測出力を追加していません。

受信処理には固定の待ち時間を入れず、表示だけを間引きます。表示間隔は厳密なタイマーではなく、受信したサンプルを基準にするため設定より長くなる場合があります。データ行は`データ名 [単位] : 各成分の値`の形式で、経過時間は表示しません。

表示例（gyro）:

```text
gyro [rad/s] : x=0.0010    y=-0.0020    z=0.0000
```

静止時の確認目安:

- accelerometer: 3軸の合成値が約9.8m/s²（重力を含む）。
- gyro: 各軸がおおむね0rad/s。
- quaternion: w²+x²+y²+z²がおおむね1。磁気を使う姿勢とgame_quaternionのYawは同じ基準とは限りません。

初期化後のリセット通知は `RESET count : ...` として表示し、レポートを再設定します。繰り返される場合は通信・電源などを確認してください。

SCK/MISO/MOSIはArduinoの型番に応じた標準SPI端子へつなぎます。PS0/PS1はSPI起動時に両方Highである必要があります。基板上の設定も確認してください。

## SPI参考サンプル

公式exampleを基に、SPI接続やピン設定、出力を変更したサンプルです。各フォルダー内の同名`.ino`をArduino IDEで開いてください。設定は各ファイル冒頭を参照してください。

| サンプル | 内容 |
|---|---|
| [rotation_vector_spi_reference](rotation_vector_spi_reference/rotation_vector_spi_reference.ino) | Game Rotation Vectorのクォータニオン出力 |
| [quaternion_yaw_pitch_roll_spi_reference](quaternion_yaw_pitch_roll_spi_reference/quaternion_yaw_pitch_roll_spi_reference.ino) | クォータニオンからYaw・Pitch・Rollへ変換 |
| [multi_report_spi_reference](multi_report_spi_reference/multi_report_spi_reference.ino) | 複数のセンサーレポートを出力 |
