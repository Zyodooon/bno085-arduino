// BNO085からSPIでセンサーレポートを取得するサンプル。
#include <Adafruit_BNO08x.h>
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
#define REPORT_INTERVAL_US 10000  // レポート要求周期: 10,000 µs = 10 ms
#define LOOP_DELAY_MS 10

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

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

  for (int n = 0; n < bno08x.prodIds.numEntries; n++) {
    Serial.print("Part ");
    Serial.print(bno08x.prodIds.entry[n].swPartNumber);
    Serial.print(": Version :");
    Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
    Serial.print(" Build ");
    Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
  }

  setReports();

  Serial.println("Reading events");
  delay(SENSOR_STARTUP_DELAY_MS);
}

// 受信するレポートを有効化する。センサーのリセット後にも呼び出す。
void setReports(void) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable game vector");
  }
}

void loop() {
  delay(LOOP_DELAY_MS);

  if (bno08x.wasReset()) {
    Serial.print("sensor was reset ");
    setReports();
  }

  if (!bno08x.getSensorEvent(&sensorValue)) {
    return;
  }

  switch (sensorValue.sensorId) {
    case SH2_GAME_ROTATION_VECTOR:
      Serial.print("Game Rotation Vector - r: ");
      Serial.print(sensorValue.un.gameRotationVector.real);
      Serial.print(" i: ");
      Serial.print(sensorValue.un.gameRotationVector.i);
      Serial.print(" j: ");
      Serial.print(sensorValue.un.gameRotationVector.j);
      Serial.print(" k: ");
      Serial.println(sensorValue.un.gameRotationVector.k);
      break;
  }
}
