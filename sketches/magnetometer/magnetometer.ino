// 校正済み磁場をSPIで単独取得する。既存サンプルとは独立したスケッチ。
//
// 値が表示されるまでの処理:
// 1. BNO08x内部で磁場の測定値に校正を適用する。
//    周囲の磁性体などによるずれを補正する処理で、詳細な係数・計算はセンサーに任せる。
// 2. SH2_MAGNETIC_FIELD_CALIBRATEDレポートは、校正済み磁場をQ4形式の符号付き整数で送る。
// 3. Adafruitライブラリが各軸を「レポート整数 / 16.0」でuT（マイクロテスラ）に換算する。
//    例: レポート整数が800なら、800 / 16 = 50 uT。
//    この整数はRaw MagnetometerのADCカウントとは別で、Raw値を16で割る意味ではない。
// 4. このスケッチは換算済みのsensorValue.un.magneticField.x/y/zをそのまま表示する。
//    方位角への変換はしない。
// 換算処理の参照: https://github.com/adafruit/Adafruit_BNO08x/blob/master/src/sh2_SensorValue.c
//
// ===== 変更する設定（周期は単位に注意） =====
#define SERIAL_BAUD_RATE 115200
#define REPORT_INTERVAL_US 5000UL  // センサーへの要求周期: 5 ms = 200 Hz
#define PRINT_INTERVAL_MS 100UL     // 通常表示の周期: 100 ms = 最大10行/秒
#define PLOT_INTERVALS 1            // 1なら受信間隔をSerial Plotterへ出力
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
sh2_SensorValue_t sensorValue;
uint32_t lastPrintMs = 0;
uint32_t lastSampleUs = 0;
uint32_t resetCount = 0;
bool hasPrinted = false;
bool hasSample = false;

void setReports() {
  if (!bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, REPORT_INTERVAL_US)) {
    Serial.println(F("ERROR: could not enable magnetometer report"));
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) { delay(10); }
#if PLOT_INTERVALS
  Serial.println(F("interval_us"));
#else
  Serial.println(F("SPI / magnetometer / unit: uT"));
  Serial.print(F("Requested report interval [us]: "));
  Serial.println(REPORT_INTERVAL_US);
  Serial.print(F("Print interval [ms]: "));
  Serial.println(PRINT_INTERVAL_MS);
#endif
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
      sensorValue.sensorId != SH2_MAGNETIC_FIELD_CALIBRATED) {
    return;
  }
  const uint32_t nowUs = micros();
#if PLOT_INTERVALS
  if (hasSample) {
    Serial.println(nowUs - lastSampleUs);
  }
  lastSampleUs = nowUs;
  hasSample = true;
#else
  const uint32_t nowMs = millis();
  if (hasPrinted && nowMs - lastPrintMs < PRINT_INTERVAL_MS) { return; }
  lastPrintMs = nowMs;
  hasPrinted = true;
  Serial.print(F("magnetometer [uT] : "));
  Serial.print(F("x="));
  Serial.print(sensorValue.un.magneticField.x, DECIMAL_PLACES);
  Serial.print(F("\ty="));
  Serial.print(sensorValue.un.magneticField.y, DECIMAL_PLACES);
  Serial.print(F("\tz="));
  Serial.print(sensorValue.un.magneticField.z, DECIMAL_PLACES);
  Serial.println();
#endif
}
