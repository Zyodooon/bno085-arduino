// 加速度・磁気による姿勢クォータニオンをSPIで単独取得する。既存サンプルとは独立したスケッチ。
// ===== 変更する設定（周期は単位に注意） =====
#define SERIAL_BAUD_RATE 115200
#define REPORT_INTERVAL_US 5000UL  // センサーへの要求周期: 5 ms = 200 Hz
#define PRINT_INTERVAL_MS 100UL     // 通常表示の周期: 100 ms = 最大10行/秒
#define PLOT_INTERVALS 0            // 1:受信間隔を出力、0:センサー値を表示（既定）
#define DECIMAL_PLACES 4            // 小数点以下の表示桁数
#define BNO08X_CS 10
#define BNO08X_INT 2
#define BNO08X_RESET 7

#include <Arduino.h>
#include <Adafruit_BNO08x.h>
#include <SPI.h>
#include <math.h>

// SCK/MISO/MOSIはボード標準のSPI端子。PS0/PS1はSPI起動時に両方High。
// 表示周期は受信を止めずに間引く。実際の受信周期はセンサーや通信にも依存する。
#if REPORT_INTERVAL_US == 0 || PRINT_INTERVAL_MS == 0
#error "REPORT_INTERVAL_US and PRINT_INTERVAL_MS must be greater than zero"
#endif

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;  // センサーから受け取ったレポート
uint32_t lastPrintMs = 0;
uint32_t lastSampleUs = 0;      // 前回データを受信した時刻
uint32_t resetCount = 0;
bool hasPrinted = false;
bool hasSample = false;         // 前回時刻があり、間隔を計算できるか

// Geomagnetic Rotation Vectorレポートを指定周期で出すようセンサーに要求する。
void setReports() {
  if (!bno08x.enableReport(SH2_GEOMAGNETIC_ROTATION_VECTOR, REPORT_INTERVAL_US)) {
    Serial.println(F("ERROR: could not enable geomagnetic_quaternion report"));
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) { delay(10); }
#if PLOT_INTERVALS
  Serial.println(F("interval_us"));
#else
  Serial.println(F("SPI / geomagnetic_quaternion / unit: unitless"));
  Serial.print(F("Requested report interval [us]: "));
  Serial.println(REPORT_INTERVAL_US);
  Serial.print(F("Print interval [ms]: "));
  Serial.println(PRINT_INTERVAL_MS);
#endif
  // CS・INTピンを使ってSPI通信を開始する。
  if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT, &SPI)) {
    Serial.println(F("ERROR: failed to find BNO08x chip"));
    while (true) { delay(10); }
  }
#if !PLOT_INTERVALS
  Serial.println(F("BNO08x Found!"));
#endif
  // 初期化に伴うリセット通知を消費。以後の通知はカウントして表示する。
  bno08x.wasReset();
  setReports();
}

void loop() {
  // センサーがリセットされたら、レポート設定をやり直す。
  if (bno08x.wasReset()) {
    ++resetCount;
#if !PLOT_INTERVALS
    Serial.print(F("RESET count : "));
    Serial.println(resetCount);
#endif
    hasPrinted = false;
    setReports();
  }
  if (!bno08x.getSensorEvent(&sensorValue) ||
      sensorValue.sensorId != SH2_GEOMAGNETIC_ROTATION_VECTOR) {
    return;
  }
  // PLOT_INTERVALS=1では受信間隔を出力する。
  const uint32_t nowUs = micros();
#if PLOT_INTERVALS
  if (hasSample) {
    Serial.println(nowUs - lastSampleUs);
  }
  lastSampleUs = nowUs;
  hasSample = true;
#else
  // 通常表示では、受信を続けたまま表示だけを間引く。
  const uint32_t nowMs = millis();
  if (hasPrinted && nowMs - lastPrintMs < PRINT_INTERVAL_MS) { return; }
  lastPrintMs = nowMs;
  hasPrinted = true;
  Serial.print(F("geomagnetic_quaternion [unitless] : "));
  Serial.print(F("w="));
  Serial.print(sensorValue.un.geoMagRotationVector.real, DECIMAL_PLACES);
  Serial.print(F("\tx="));
  Serial.print(sensorValue.un.geoMagRotationVector.i, DECIMAL_PLACES);
  Serial.print(F("\ty="));
  Serial.print(sensorValue.un.geoMagRotationVector.j, DECIMAL_PLACES);
  Serial.print(F("\tz="));
  Serial.print(sensorValue.un.geoMagRotationVector.k, DECIMAL_PLACES);
  Serial.println();
#endif
}
