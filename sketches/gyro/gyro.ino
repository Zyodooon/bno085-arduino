// 校正済み角速度をSPIで単独取得する。既存サンプルとは独立したスケッチ。
//
// 値が表示されるまでの処理:
// 1. BNO08x内部でジャイロの偏り（バイアス）などを補正し、角速度を求める。
//    概念的には「測定角速度 - 推定した偏り」。実際の係数・内部計算はセンサーに任せる。
// 2. SH2_GYROSCOPE_CALIBRATEDレポートは、校正済み角速度をQ9形式の符号付き整数で送る。
// 3. Adafruitライブラリが各軸を「レポート整数 / 512.0」でrad/sに換算する。
//    例: レポート整数が1024なら、1024 / 512 = 2 rad/s。
//    この整数はRaw GyroのADCカウントとは別で、Raw値を512で割る意味ではない。
// 4. このスケッチは換算済みのsensorValue.un.gyroscope.x/y/zをそのまま表示する。
//    度/秒への変換や角速度の積分による角度計算はしない。
// 換算処理の参照: https://github.com/adafruit/Adafruit_BNO08x/blob/master/src/sh2_SensorValue.c
//
// ===== 変更する設定（周期は単位に注意） =====
#define SERIAL_BAUD_RATE 115200
#define REPORT_INTERVAL_US 10000UL  // センサーへの要求周期: 10 ms = 100 Hz
#define PRINT_INTERVAL_MS 100UL     // 画面への表示周期: 100 ms = 最大10行/秒
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
uint32_t resetCount = 0;
bool hasPrinted = false;

void setReports() {
  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, REPORT_INTERVAL_US)) {
    Serial.println(F("ERROR: could not enable gyro report"));
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) { delay(10); }
  Serial.println(F("SPI / gyro / unit: rad/s"));
  Serial.print(F("Requested report interval [us]: "));
  Serial.println(REPORT_INTERVAL_US);
  Serial.print(F("Print interval [ms]: "));
  Serial.println(PRINT_INTERVAL_MS);
  if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT, &SPI)) {
    Serial.println(F("ERROR: failed to find BNO08x chip"));
    while (true) { delay(10); }
  }
  Serial.println(F("BNO08x Found!"));
  // 初期化に伴うリセット通知を消費。以後の通知はカウントして表示する。
  bno08x.wasReset();
  setReports();
}

void loop() {
  if (bno08x.wasReset()) {
    ++resetCount;
    Serial.print(F("RESET / t[ms]="));
    Serial.print(millis());
    Serial.print(F(" / count="));
    Serial.println(resetCount);
    hasPrinted = false;
    setReports();
  }
  if (!bno08x.getSensorEvent(&sensorValue) ||
      sensorValue.sensorId != SH2_GYROSCOPE_CALIBRATED) {
    return;
  }
  const uint32_t nowMs = millis();
  if (hasPrinted && nowMs - lastPrintMs < PRINT_INTERVAL_MS) { return; }
  lastPrintMs = nowMs;
  hasPrinted = true;
  Serial.print(F("t[ms]="));
  Serial.print(nowMs);
  Serial.print(F("\tx[rad/s]="));
  Serial.print(sensorValue.un.gyroscope.x, DECIMAL_PLACES);
  Serial.print(F("\ty[rad/s]="));
  Serial.print(sensorValue.un.gyroscope.y, DECIMAL_PLACES);
  Serial.print(F("\tz[rad/s]="));
  Serial.print(sensorValue.un.gyroscope.z, DECIMAL_PLACES);
  Serial.print(F("\taccuracy[0-3]="));
  Serial.print(sensorValue.status);
  Serial.println();
}
