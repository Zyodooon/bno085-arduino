# 個別のSPI動作確認スケッチ

各フォルダー内の同名 `.ino` をArduino IDEで開きます。Windowsで使う場合はフォルダーごとコピーしてください。各スケッチは単独で動作し、Adafruit BNO08xと依存ライブラリが必要です。既存の `src` のサンプルはそのままです。

| スケッチ | 取得するデータ | 単位 |
|---|---|---|
| [accelerometer](accelerometer/accelerometer.ino) | 校正済み加速度（重力を含む） | m/s^2 |
| [gyro](gyro/gyro.ino) | 校正済み角速度 | rad/s |
| [magnetometer](magnetometer/magnetometer.ino) | 校正済み磁場 | uT |
| [quaternion](quaternion/quaternion.ino) | 磁気を使う姿勢クォータニオン | unitless |
| [game_quaternion](game_quaternion/game_quaternion.ino) | 磁気を使わない姿勢クォータニオン（Yawはドリフトする） | unitless |
| [geomagnetic_quaternion](geomagnetic_quaternion/geomagnetic_quaternion.ino) | 加速度・磁気による姿勢クォータニオン | unitless |

加速度・角速度・磁場はセンサー内部で校正されたレポートを取得します。クォータニオンは無次元で、w=real、x=i、y=j、z=kです。`accuracy`はレポートの精度ステータス（0: unreliable、1: low、2: medium、3: high）で、数値誤差の大きさを直接表すものではありません。

各ファイルの先頭で次を変更できます。

| define | 内容 | 初期値 |
|---|---|---|
| `SERIAL_BAUD_RATE` | シリアル通信速度。IDE側の設定も合わせる | 115200 |
| `REPORT_INTERVAL_US` | センサーへのレポート要求周期（µs） | 5000（200Hz） |
| `PRINT_INTERVAL_MS` | 受信した値を表示する最短間隔（ms） | 100（最大10行/秒） |
| `DECIMAL_PLACES` | 小数点以下の表示桁数 | 4 |
| `BNO08X_CS` / `BNO08X_INT` / `BNO08X_RESET` | SPI制御ピン | 10 / 2 / 7 |

受信処理には固定の待ち時間を入れず、表示だけを間引きます。表示間隔は厳密なタイマーではなく、受信したサンプルを基準にするため設定より長くなる場合があります。`t[ms]`はArduino起動から表示までの時刻で、センサーのサンプル時刻ではありません。

表示例（gyro）:

```text
t[ms]=1234    x[rad/s]=0.0010    y[rad/s]=-0.0020    z[rad/s]=0.0000    accuracy[0-3]=2
```

静止時の確認目安:

- accelerometer: 3軸の合成値が約9.8m/s²（重力を含む）。
- gyro: 各軸がおおむね0rad/s。
- quaternion: w²+x²+y²+z²がおおむね1。磁気を使う姿勢とgame_quaternionのYawは同じ基準とは限りません。

初期化後のリセット通知は `RESET / t[ms]=... / count=...` として表示し、レポートを再設定します。繰り返される場合は通信・電源などを確認してください。

SCK/MISO/MOSIはArduinoの型番に応じた標準SPI端子へつなぎます。PS0/PS1はSPI起動時に両方Highである必要があります。基板上の設定も確認してください。
