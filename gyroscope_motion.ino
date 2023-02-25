
// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int toneFreq = 2000;
const int buzzer = 9; //buzzer to arduino pin 9
const int ledPin = 13; //led to arduino pin 13
bool lightSoundOn = false;
bool alert = false;

// Pins for liquid display
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);
  // tone(buzzer, toneFreq);
  // digitalWrite(ledPin, HIGH);

  // set up the LCD's number of columns and rows:
  lcd.begin(12, 2);

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  mpu.calibrateGyro();

  mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);

  
  mpu.setIntFreeFallEnabled(false);  
  mpu.setIntZeroMotionEnabled(false);
  mpu.setIntMotionEnabled(false);
  
  mpu.setDHPFMode(MPU6050_DHPF_5HZ);

  mpu.setMotionDetectionThreshold(1);
  mpu.setMotionDetectionDuration(1);

  mpu.setZeroMotionDetectionThreshold(1);
  mpu.setZeroMotionDetectionDuration(2);	
  
  checkSettings();
  
}

void checkSettings()
{
  Serial.println();
  
  Serial.print(" * Sleep Mode:                ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Motion Interrupt:     ");
  Serial.println(mpu.getIntMotionEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Zero Motion Interrupt:     ");
  Serial.println(mpu.getIntZeroMotionEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Free Fall Interrupt:       ");
  Serial.println(mpu.getIntFreeFallEnabled() ? "Enabled" : "Disabled");
  
  Serial.print(" * Motion Threshold:          ");
  Serial.println(mpu.getMotionDetectionThreshold());

  Serial.print(" * Motion Duration:           ");
  Serial.println(mpu.getMotionDetectionDuration());

  Serial.print(" * Zero Motion Threshold:     ");
  Serial.println(mpu.getZeroMotionDetectionThreshold());

  Serial.print(" * Zero Motion Duration:      ");
  Serial.println(mpu.getZeroMotionDetectionDuration());
  
  Serial.print(" * Clock Source:              ");
  switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  Serial.print(" * Accelerometer:             ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Accelerometer offsets:     ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());

  Serial.print(" * Accelerometer power delay: ");
  switch(mpu.getAccelPowerOnDelay())
  {
    case MPU6050_DELAY_3MS:            Serial.println("3ms"); break;
    case MPU6050_DELAY_2MS:            Serial.println("2ms"); break;
    case MPU6050_DELAY_1MS:            Serial.println("1ms"); break;
    case MPU6050_NO_DELAY:             Serial.println("0ms"); break;
  }  
  
  Serial.println();
}

void turnAlarmOnOff()
{
  // blink
  lightSoundOn = !lightSoundOn;
  if (alert == true && lightSoundOn == true) {
    tone(buzzer, toneFreq);
    digitalWrite(ledPin, HIGH);
  }  else {
    noTone(buzzer);
    digitalWrite(ledPin, LOW);
  }
}

void loop() {

  Vector rawAccel = mpu.readRawAccel();
  Activites act = mpu.readActivites();
  Vector rawGyro = mpu.readRawGyro();
  Vector normGyro = mpu.readNormalizeGyro();


  lcd.setCursor(0, 0);
  lcd.print(millis() / 1000);
  lcd.setCursor(0, 1);
  String res = "ALERTING";
  
  Serial.print(" Xraw = ");  
  Serial.print(rawGyro.XAxis);
  Serial.print(" Yraw = ");
  Serial.print(rawGyro.YAxis);
  Serial.print(" Zraw = ");
  Serial.println(rawGyro.ZAxis);
  Serial.print(" Xnorm = ");
  Serial.print(normGyro.XAxis);
  Serial.print(" Ynorm = ");
  Serial.print(normGyro.YAxis);
  Serial.print(" Znorm = ");
  Serial.print(normGyro.ZAxis);
  
  Serial.print("-----");
  
  Serial.print(act.isActivity);
  Serial.print(act.isInactivity);

  Serial.print(" ");
  Serial.print(act.isPosActivityOnX);
  Serial.print(act.isNegActivityOnX);
  Serial.print(" ");

  Serial.print(act.isPosActivityOnY);
  Serial.print(act.isNegActivityOnY);
  Serial.print(" ");

  Serial.print(act.isPosActivityOnZ);
  Serial.print(act.isNegActivityOnZ);

  if (act.isActivity || act.isInactivity || act.isPosActivityOnX || act.isPosActivityOnY || act.isPosActivityOnZ ||
    act.isNegActivityOnX || act.isNegActivityOnY || act.isNegActivityOnZ) {
    alert = true;
    lcd.print(res);
    Serial.print("-ALERTING");
  } else {
    alert = false;
    lcd.print("         ");
  }
  Serial.print("\n");

  turnAlarmOnOff();

  // Serial.println("it should display millis");
  delay(100);
}
