// 加速度（重力を含む）をSPIで単独取得する。既存サンプルとは独立したスケッチ。
//
// 値が表示されるまでの処理:
// 1. BNO08x内部で加速度センサーの測定値を校正する（重力は含んだまま）。
//    校正の詳細な計算式・係数はこのスケッチでは扱わず、センサーに任せる。
// 2. SH2_ACCELEROMETERレポートは、校正済み加速度をQ8形式の符号付き整数で送る。
// 3. Adafruitライブラリが各軸を「レポート整数 / 256.0」でm/s^2に換算する。
//    例: レポート整数が2510なら、2510 / 256 = 9.8046875 m/s^2。
//    この整数はRaw AccelerometerのADCカウントとは別で、Raw値を256で割る意味ではない。
// 4. このスケッチは換算済みのsensorValue.un.accelerometer.x/y/zをそのまま表示する。
//    重力除去や姿勢への変換はしない。
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
  if (!bno08x.enableReport(SH2_ACCELEROMETER, REPORT_INTERVAL_US)) {
    Serial.println(F("ERROR: could not enable accelerometer report"));
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) { delay(10); }
  Serial.println(F("SPI / accelerometer / unit: m/s^2"));
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
      sensorValue.sensorId != SH2_ACCELEROMETER) {
    return;
  }
  const uint32_t nowMs = millis();
  if (hasPrinted && nowMs - lastPrintMs < PRINT_INTERVAL_MS) { return; }
  lastPrintMs = nowMs;
  hasPrinted = true;
  Serial.print(F("t[ms]="));
  Serial.print(nowMs);
  Serial.print(F("\tx[m/s^2]="));
  Serial.print(sensorValue.un.accelerometer.x, DECIMAL_PLACES);
  Serial.print(F("\ty[m/s^2]="));
  Serial.print(sensorValue.un.accelerometer.y, DECIMAL_PLACES);
  Serial.print(F("\tz[m/s^2]="));
  Serial.print(sensorValue.un.accelerometer.z, DECIMAL_PLACES);
  Serial.print(F("\taccuracy[0-3]="));
  Serial.print(sensorValue.status);
  Serial.println();
}
