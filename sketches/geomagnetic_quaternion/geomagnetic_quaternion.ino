// 加速度・磁気による姿勢クォータニオンをSPIで単独取得する。既存サンプルとは独立したスケッチ。
// ===== 変更する設定（周期は単位に注意） =====
#define SERIAL_BAUD_RATE 115200
#define REPORT_INTERVAL_US 5000UL  // センサーへの要求周期: 5 ms = 200 Hz
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
  if (!bno08x.enableReport(SH2_GEOMAGNETIC_ROTATION_VECTOR, REPORT_INTERVAL_US)) {
    Serial.println(F("ERROR: could not enable geomagnetic_quaternion report"));
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) { delay(10); }
  Serial.println(F("SPI / geomagnetic_quaternion / unit: unitless"));
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
      sensorValue.sensorId != SH2_GEOMAGNETIC_ROTATION_VECTOR) {
    return;
  }
  const uint32_t nowMs = millis();
  if (hasPrinted && nowMs - lastPrintMs < PRINT_INTERVAL_MS) { return; }
  lastPrintMs = nowMs;
  hasPrinted = true;
  Serial.print(F("t[ms]="));
  Serial.print(nowMs);
  Serial.print(F("\tw[unitless]="));
  Serial.print(sensorValue.un.geoMagRotationVector.real, DECIMAL_PLACES);
  Serial.print(F("\tx[unitless]="));
  Serial.print(sensorValue.un.geoMagRotationVector.i, DECIMAL_PLACES);
  Serial.print(F("\ty[unitless]="));
  Serial.print(sensorValue.un.geoMagRotationVector.j, DECIMAL_PLACES);
  Serial.print(F("\tz[unitless]="));
  Serial.print(sensorValue.un.geoMagRotationVector.k, DECIMAL_PLACES);
  Serial.print(F("\taccuracy[0-3]="));
  Serial.print(sensorValue.status);
  Serial.println();
}
