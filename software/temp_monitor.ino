
#include <Wire.h>   //implementation of I2C




const int I2C_ADDRESS = 0x42;    // I2C address

const int PIN_A   = 2;           // multiplexer pin A
const int PIN_B   = 3;           // multiplexer pin B
const int PIN_C   = 4;           // multiplexer pin C
const int PIN_LED = 6;           // LED pin
const int PIN_SENSOR = A0;       // multiplexer input


// sensor data table implementation
const float tableTemp[33] =
{
  -40, -35, -30, -25, -20, -15, -10,  -5, 0,   5,  10,  15,  20,  25,  30,  35, 40,  45,  50,  55,  60,  65,  70,  75, 80,  85,  90,  95, 100, 105, 110, 115, 120
};

const float tableVolts[33] =
{
  2.44, 2.42, 2.40, 2.38, 2.35, 2.32, 2.27, 2.23, 2.17, 2.11, 2.05, 1.99, 1.92, 1.86, 1.80, 1.74, 1.68, 1.63, 1.59, 1.55, 1.51, 1.48, 1.45, 1.43, 1.40, 1.38, 1.37, 1.35, 1.34, 1.33, 1.32, 1.31, 1.30
};


// max, min, and avg temp variables
float temps[8];        // temperature data from sensors
float maxTemp = 0;
float minTemp = 0;
float avgTemp = 0;

// max, min, average, and sensors 0-7 are sent through I2C
int i2cData[3 + 8];

// vars used for turning LED on/off for a certain amount of time
bool ledOn = false;
unsigned long lastBlinkTime = 0;
unsigned long lastPrintTime = 0;


// ---------------- Helper functions ----------------

// determines which sensor to read by writing sensor number in binary across C B A
void selectChannel(int channel)
{
  digitalWrite(PIN_A, bitRead(channel, 0));
  digitalWrite(PIN_B, bitRead(channel, 1));
  digitalWrite(PIN_C, bitRead(channel, 2));
}

// Read the voltage on A0. Multiple readings are taken to filter out some noise
float readVolts()
{
  long total = 0;
  for(int i = 0; i < 4; i++)
  {
    total = total + analogRead(PIN_SENSOR);
  }
  float average = total / 4.0;
  return average * 5.00 / 1024.0;     // converts to volts to use data table
}

// converts voltage from previous step into temperature
float voltsToTemp(float volts)
{
  // handling extreme cases
  if(volts >= tableVolts[0])
  {
    return tableTemp[0];
  }
  if(volts <= tableVolts[33 - 1])
  {
    return tableTemp[33 - 1];
  }

  // goes through table to find two points the voltage is in between
  for(int i = 0; i < 33 - 1; i++)
  {
    float vHigh = tableVolts[i];
    float vLow  = tableVolts[i + 1];
  // linear interpolation to estimate temp
    if(volts <= vHigh && volts >= vLow)
    {
      float fraction = (vHigh - volts) / (vHigh - vLow);
      return tableTemp[i] + fraction * (tableTemp[i + 1] - tableTemp[i]);
    }
  }
  return tableTemp[0];
}

  //sending data through I2C
void sendI2CData()
{
  for(int i = 0; i < 3 + 8; i++)
  {
    Wire.write(lowByte(i2cData[i]));
    Wire.write(highByte(i2cData[i]));
  }
}


void setup()
{
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  // setup of I2C so it is ready when the data is called for
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(sendI2CData);
  // serial monitor testing
  Serial.begin(115200);
  Serial.println("bankai");
}

void loop()
{

  // loops through all sensors
  for(int ch = 0; ch < 8; ch++)
  {
    selectChannel(ch);
    delayMicroseconds(200);       // slight delay bc of the small capacitor (first reading may be off)
    float volts = readVolts();
    temps[ch] = voltsToTemp(volts);
  }

  // simple logic to find max min and avg
  maxTemp = temps[0];
  minTemp = temps[0];
  float sum = 0;
  for(int ch = 0; ch < 8; ch++)
  {
    if(temps[ch] > maxTemp)
    {
      maxTemp = temps[ch];
    }
    if(temps[ch] < minTemp)
    {
      minTemp = temps[ch];
    }
    sum = sum + temps[ch];
  }
  avgTemp = sum / 8;

  // updating numbers for I2C
  noInterrupts();
  //converting temperature decimals to integers to send over I2C more easily
  i2cData[0] = round(maxTemp * 100);
  i2cData[1] = round(minTemp * 100);
  i2cData[2] = round(avgTemp * 100);
  for(int ch = 0; ch < 8; ch++)
  {
    i2cData[3 + ch] = round(temps[ch] * 100);
  }
  interrupts();


  // blinks LED while the temp from any sensor is above 60C
  if(maxTemp > 60.0)
  {
    if(millis() - lastBlinkTime >= 250)
    {
      lastBlinkTime = millis();
      ledOn = !ledOn;
      digitalWrite(PIN_LED, ledOn);
    }
  }
  else
  {
    ledOn = false;
    digitalWrite(PIN_LED, LOW);
  }

  // printing to serial monitor for testing purposes
  if(millis() - lastPrintTime >= 500)
  {
    lastPrintTime = millis();
    Serial.print("Max: ");   Serial.print(maxTemp);
    Serial.print("  Min: "); Serial.print(minTemp);
    Serial.print("  Avg: "); Serial.print(avgTemp);
    Serial.print("  |");
    for(int ch = 0; ch < 8; ch++)
    {
      Serial.print("  T"); Serial.print(ch); Serial.print("=");
      Serial.print(temps[ch]);
    }
    Serial.println();
  }
}
