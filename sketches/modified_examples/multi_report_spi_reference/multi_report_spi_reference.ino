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
  if (!bno08x.enableReport(SH2_ACCELEROMETER, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable accelerometer");
  }
  if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable gyroscope");
  }
  if (!bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable magnetic field calibrated");
  }
  if (!bno08x.enableReport(SH2_LINEAR_ACCELERATION, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable linear acceleration");
  }
  if (!bno08x.enableReport(SH2_GRAVITY, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable gravity vector");
  }
  if (!bno08x.enableReport(SH2_ROTATION_VECTOR, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable rotation vector");
  }
  if (!bno08x.enableReport(SH2_GEOMAGNETIC_ROTATION_VECTOR, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable geomagnetic rotation vector");
  }
  if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable game rotation vector");
  }
  if (!bno08x.enableReport(SH2_STEP_COUNTER, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable step counter");
  }
  if (!bno08x.enableReport(SH2_STABILITY_CLASSIFIER, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable stability classifier");
  }
  if (!bno08x.enableReport(SH2_RAW_ACCELEROMETER, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable raw accelerometer");
  }
  if (!bno08x.enableReport(SH2_RAW_GYROSCOPE, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable raw gyroscope");
  }
  if (!bno08x.enableReport(SH2_RAW_MAGNETOMETER, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable raw magnetometer");
  }
  if (!bno08x.enableReport(SH2_SHAKE_DETECTOR, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable shake detector");
  }
  if (!bno08x.enableReport(SH2_PERSONAL_ACTIVITY_CLASSIFIER, REPORT_INTERVAL_US)) {
    Serial.println("Could not enable personal activity classifier");
  }
}

void printActivity(uint8_t activity_id) {
  switch (activity_id) {
    case PAC_UNKNOWN:
      Serial.print("Unknown");
      break;
    case PAC_IN_VEHICLE:
      Serial.print("In Vehicle");
      break;
    case PAC_ON_BICYCLE:
      Serial.print("On Bicycle");
      break;
    case PAC_ON_FOOT:
      Serial.print("On Foot");
      break;
    case PAC_STILL:
      Serial.print("Still");
      break;
    case PAC_TILTING:
      Serial.print("Tilting");
      break;
    case PAC_WALKING:
      Serial.print("Walking");
      break;
    case PAC_RUNNING:
      Serial.print("Running");
      break;
    case PAC_ON_STAIRS:
      Serial.print("On Stairs");
      break;
    default:
      Serial.print("NOT LISTED");
  }
  Serial.print(" (");
  Serial.print(activity_id);
  Serial.print(")");
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
    case SH2_ACCELEROMETER:
      Serial.print("Accelerometer - x: ");
      Serial.print(sensorValue.un.accelerometer.x);
      Serial.print(" y: ");
      Serial.print(sensorValue.un.accelerometer.y);
      Serial.print(" z: ");
      Serial.println(sensorValue.un.accelerometer.z);
      break;
    case SH2_GYROSCOPE_CALIBRATED:
      Serial.print("Gyro - x: ");
      Serial.print(sensorValue.un.gyroscope.x);
      Serial.print(" y: ");
      Serial.print(sensorValue.un.gyroscope.y);
      Serial.print(" z: ");
      Serial.println(sensorValue.un.gyroscope.z);
      break;
    case SH2_MAGNETIC_FIELD_CALIBRATED:
      Serial.print("Magnetic Field - x: ");
      Serial.print(sensorValue.un.magneticField.x);
      Serial.print(" y: ");
      Serial.print(sensorValue.un.magneticField.y);
      Serial.print(" z: ");
      Serial.println(sensorValue.un.magneticField.z);
      break;
    case SH2_LINEAR_ACCELERATION:
      Serial.print("Linear Acceration - x: ");
      Serial.print(sensorValue.un.linearAcceleration.x);
      Serial.print(" y: ");
      Serial.print(sensorValue.un.linearAcceleration.y);
      Serial.print(" z: ");
      Serial.println(sensorValue.un.linearAcceleration.z);
      break;
    case SH2_GRAVITY:
      Serial.print("Gravity - x: ");
      Serial.print(sensorValue.un.gravity.x);
      Serial.print(" y: ");
      Serial.print(sensorValue.un.gravity.y);
      Serial.print(" z: ");
      Serial.println(sensorValue.un.gravity.z);
      break;
    case SH2_ROTATION_VECTOR:
      Serial.print("Rotation Vector - r: ");
      Serial.print(sensorValue.un.rotationVector.real);
      Serial.print(" i: ");
      Serial.print(sensorValue.un.rotationVector.i);
      Serial.print(" j: ");
      Serial.print(sensorValue.un.rotationVector.j);
      Serial.print(" k: ");
      Serial.println(sensorValue.un.rotationVector.k);
      break;
    case SH2_GEOMAGNETIC_ROTATION_VECTOR:
      Serial.print("Geo-Magnetic Rotation Vector - r: ");
      Serial.print(sensorValue.un.geoMagRotationVector.real);
      Serial.print(" i: ");
      Serial.print(sensorValue.un.geoMagRotationVector.i);
      Serial.print(" j: ");
      Serial.print(sensorValue.un.geoMagRotationVector.j);
      Serial.print(" k: ");
      Serial.println(sensorValue.un.geoMagRotationVector.k);
      break;

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

    case SH2_STEP_COUNTER:
      Serial.print("Step Counter - steps: ");
      Serial.print(sensorValue.un.stepCounter.steps);
      Serial.print(" latency: ");
      Serial.println(sensorValue.un.stepCounter.latency);
      break;

    case SH2_STABILITY_CLASSIFIER: {
      Serial.print("Stability Classification: ");
      sh2_StabilityClassifier_t stability = sensorValue.un.stabilityClassifier;
      switch (stability.classification) {
        case STABILITY_CLASSIFIER_UNKNOWN:
          Serial.println("Unknown");
          break;
        case STABILITY_CLASSIFIER_ON_TABLE:
          Serial.println("On Table");
          break;
        case STABILITY_CLASSIFIER_STATIONARY:
          Serial.println("Stationary");
          break;
        case STABILITY_CLASSIFIER_STABLE:
          Serial.println("Stable");
          break;
        case STABILITY_CLASSIFIER_MOTION:
          Serial.println("In Motion");
          break;
      }
      break;
    }

    case SH2_RAW_ACCELEROMETER:
      Serial.print("Raw Accelerometer - x: ");
      Serial.print(sensorValue.un.rawAccelerometer.x);
      Serial.print(" y: ");
      Serial.print(sensorValue.un.rawAccelerometer.y);
      Serial.print(" z: ");
      Serial.println(sensorValue.un.rawAccelerometer.z);
      break;
    case SH2_RAW_GYROSCOPE:
      Serial.print("Raw Gyro - x: ");
      Serial.print(sensorValue.un.rawGyroscope.x);
      Serial.print(" y: ");
      Serial.print(sensorValue.un.rawGyroscope.y);
      Serial.print(" z: ");
      Serial.println(sensorValue.un.rawGyroscope.z);
      break;
    case SH2_RAW_MAGNETOMETER:
      Serial.print("Raw Magnetic Field - x: ");
      Serial.print(sensorValue.un.rawMagnetometer.x);
      Serial.print(" y: ");
      Serial.print(sensorValue.un.rawMagnetometer.y);
      Serial.print(" z: ");
      Serial.println(sensorValue.un.rawMagnetometer.z);
      break;

    case SH2_SHAKE_DETECTOR: {
      Serial.print("Shake Detector - shake detected on axis: ");
      sh2_ShakeDetector_t detection = sensorValue.un.shakeDetector;
      switch (detection.shake) {
        case SHAKE_X:
          Serial.println("X");
          break;
        case SHAKE_Y:
          Serial.println("Y");
          break;
        case SHAKE_Z:
          Serial.println("Z");
          break;
        default:
          Serial.println("None");
          break;
      }
    }

    case SH2_PERSONAL_ACTIVITY_CLASSIFIER: {
      sh2_PersonalActivityClassifier_t activity =
          sensorValue.un.personalActivityClassifier;
      Serial.print("Activity classification - Most likely: ");
      printActivity(activity.mostLikelyState);
      Serial.println("");

      Serial.println("Confidences:");
      // 活動分類の候補ごとに確信度を出力する。
      for (uint8_t i = 0; i < PAC_OPTION_COUNT; i++) {
        Serial.print("\t");
        printActivity(i);
        Serial.print(": ");
        Serial.println(activity.confidence[i]);
      }
    }
  }
}
