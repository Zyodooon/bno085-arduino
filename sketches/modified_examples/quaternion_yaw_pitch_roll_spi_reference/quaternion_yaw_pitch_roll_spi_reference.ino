// SPIで取得したクォータニオンをYaw・Pitch・Roll（度）に変換する。
#include <Adafruit_BNO08x.h>
#include <Arduino.h>
#include <SPI.h>

// CS・INT・RSTは使用ボードの実際の配線に合わせて変更する。
#define BNO08X_CS 10
#define BNO08X_INT 2
#define BNO08X_RESET 7

// 調整する値（時間の単位に注意）。
#define SERIAL_BAUD_RATE 115200
#define SERIAL_WAIT_DELAY_MS 10
#define SENSOR_ERROR_DELAY_MS 10
#define SENSOR_STARTUP_DELAY_MS 100
// SCK/MISO/MOSIはボード標準のSPI端子を使用する。
// ピン番号をここで変更することはできないため、ボードのピン配置を確認する。

// 0: 安定化姿勢、1: 高速なジャイロ積分姿勢。
// ビルド設定の -DFAST_MODE も引き続き使用できる。
#ifndef FAST_MODE
#define FAST_MODE 0
#endif
#define STABILIZED_REPORT_INTERVAL_US 5000
#define FAST_REPORT_INTERVAL_US 2000
#define OUTPUT_IN_DEGREES true  // true: 度、false: ラジアン

struct euler_t {
  float yaw;
  float pitch;
  float roll;
} ypr;

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

// 冒頭の設定に応じて受信するレポートを選ぶ。
#if FAST_MODE
// ジャイロ積分の姿勢レポートを要求する。
sh2_SensorId_t reportType = SH2_GYRO_INTEGRATED_RV;
const uint32_t reportIntervalUs = FAST_REPORT_INTERVAL_US;
#else
// AR/VR向けの安定化姿勢レポートを要求する。
sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
const uint32_t reportIntervalUs = STABILIZED_REPORT_INTERVAL_US;
#endif

void setReports(sh2_SensorId_t reportType, long report_interval) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(reportType, report_interval)) {
    Serial.println("Could not enable stabilized remote vector");
  }
}

void setup(void) {
  Serial.begin(SERIAL_BAUD_RATE);
  // シリアルモニタが開くまで待機する。
  while (!Serial) {
    delay(SERIAL_WAIT_DELAY_MS);
  }

  Serial.println("Adafruit BNO08x test!");

  // SCK・MISO・MOSIはボード標準のSPI端子を使用する。
  if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT, &SPI)) {
    Serial.println("Failed to find BNO08x chip");
    while (1) {
      delay(SENSOR_ERROR_DELAY_MS);
    }
  }
  Serial.println("BNO08x Found!");

  setReports(reportType, reportIntervalUs);

  Serial.println("Reading events");
  delay(SENSOR_STARTUP_DELAY_MS);
}

void quaternionToEuler(float qr, float qi, float qj, float qk, euler_t* ypr,
                       bool degrees = false) {
  float sqr = sq(qr);
  float sqi = sq(qi);
  float sqj = sq(qj);
  float sqk = sq(qk);

  ypr->yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
  ypr->pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
  ypr->roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));

  if (degrees) {
    ypr->yaw *= RAD_TO_DEG;
    ypr->pitch *= RAD_TO_DEG;
    ypr->roll *= RAD_TO_DEG;
  }
}

void quaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, euler_t* ypr,
                         bool degrees = false) {
  quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j,
                    rotational_vector->k, ypr, degrees);
}

void quaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, euler_t* ypr,
                         bool degrees = false) {
  quaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j,
                    rotational_vector->k, ypr, degrees);
}

void loop() {
  if (bno08x.wasReset()) {
    Serial.print("sensor was reset ");
    setReports(reportType, reportIntervalUs);
  }

  if (bno08x.getSensorEvent(&sensorValue)) {
    // FAST_MODEの設定で選択した姿勢レポートを角度に変換する。
    switch (sensorValue.sensorId) {
      case SH2_ARVR_STABILIZED_RV:
        quaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, &ypr, OUTPUT_IN_DEGREES);
        break;
      case SH2_GYRO_INTEGRATED_RV:
        quaternionToEulerGI(&sensorValue.un.gyroIntegratedRV, &ypr, OUTPUT_IN_DEGREES);
        break;
    }
    static long last = 0;
    long now = micros();
    Serial.print(now - last);
    Serial.print("\t");
    last = now;
    Serial.print(sensorValue.status);  // 校正精度（0～3）。
    Serial.print("\t");
    Serial.print(ypr.yaw);
    Serial.print("\t");
    Serial.print(ypr.pitch);
    Serial.print("\t");
    Serial.println(ypr.roll);
  }
}
