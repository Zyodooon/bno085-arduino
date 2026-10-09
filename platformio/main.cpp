#include <Arduino.h>

// Include one reference sketch, leaving the Arduino IDE sketches intact.
#if BNO085_SAMPLE == 1
#include "../src/bno085_quaternion_yaw_pitch_roll_spi_reference.ino"
#elif BNO085_SAMPLE == 2
// Arduino IDE generates this declaration; C++ compilation needs it explicitly.
void setReports();
#include "../src/bno085_rotation_vector_spi_reference.ino"
#elif BNO085_SAMPLE == 3
void setReports();
#include "../src/bno085_multi_report_spi_reference.ino"
#else
#error "Select BNO085_SAMPLE=1, 2, or 3 in platformio.ini"
#endif
