#include <RFduinoBLE.h>

const int table[54] = {0, 6, 11, 17, 23, 28, 34, 72, 91, 109, 147, 204, 260, 313, 366, 419, 472, 525, 578, 605, 632, 660, 687, 714, 726, 738, 750, 762, 774, 786, 791, 797, 808, 818, 824, 829, 834, 839, 845, 850, 854, 858, 863, 867, 870, 874, 878, 881, 883, 886, 889, 891, 894 };
const int tableLength = 53;
const int maxADC = 894;

//Mux control pins
int s0 = 3;
int s1 = 4;
int s2 = 5;
int s3 = 6;

//Mux in "SIG" pin
int IO1 = 2;
int IO2 = 1;
bool isConnected = false;


void setup() {
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(IO1,INPUT);
  pinMode(IO2,INPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  // this is the data we want to appear in the advertisement
  // (if the deviceName and advertisementData are too long to fix into the 31 byte
  // ble advertisement packet, then the advertisementData is truncated first down to
  // a single byte, then it will truncate the deviceName)
  RFduinoBLE.advertisementData = "muxpsi";

  // start the BLE stack
  RFduinoBLE.begin();

  //Serial.begin(9600);
}

//Arrays to store analog values after recieving them
int IO1AnalogVals[16];
int IO2AnalogVals[16];
//int IO3AnalogVals[16];

void loop()
{
  if (isConnected) {
    for (int i = 0; i < 16; i++)
    {
      //Analog read on all 16 inputs on IO1, IO2, and IO3
      IO1AnalogVals[i] = analogReadMS(1, i);
      IO2AnalogVals[i] = analogReadMS(2, i);
    }

    //Print IO1 values for inspection
    //Serial.print("IO1 analog values: ");
    RFduinoBLE.sendInt(11);
    for (int i = 0; i < 16; i++)
    {
      //RFduinoBLE.sendInt(i);
      printReading(IO1AnalogVals[i]);
      delay(10);
      //Serial.print('\t');
    }
    //Serial.println();
    //Serial.print("IO2 analog values: ");
    RFduinoBLE.sendInt(12);
    for (int i = 0; i < 16; i++)
    {
      //RFduinoBLE.sendInt(i);
      printReading(IO2AnalogVals[i]);
      delay(10);
      //Serial.print('\t');
    }
    //Serial.println();
    RFduinoBLE.sendInt(99);
    delay(200);
  }
}

void RFduinoBLE_onConnect()
{
  //digitalWrite(connection_led, HIGH);
  isConnected = true;
  delay(100);
}

void RFduinoBLE_onDisconnect()
{
  //digitalWrite(connection_led, LOW);
  isConnected = false;
}

int analogReadMS(int mux, int chan)
{
  //digitalWrite(_OUTMD,LOW);
  int val;

  digitalWrite(s0, (chan & 1));
  digitalWrite(s1, (chan & 3) >> 1);
  digitalWrite(s2, (chan & 7) >> 2);
  digitalWrite(s3, (chan & 15) >> 3);
  delay(1);
  switch (mux) {
    case 1:
      val = analogRead(IO1);
      break;
    case 2:
      val = analogRead(IO2);
      break;
    default:
      break;
  }
  return val;
}
void printReading(int fsrReading) {
  if (fsrReading > table[tableLength - 1]) {
    float psi = ((tableLength - 1) * 0.1) + 0.5;
    //Serial.print("OVER ");
    RFduinoBLE.sendFloat(psi);
    //Serial.print(psi);
    //Serial.print(" PSI");
  } else {
    for (int i = 1; i < tableLength - 1; i++) {
      if (table[i] > fsrReading) {
        float psi = ((i - 1) * 0.1) + 0.5;
        //      Serial.print("ADC ");
        //      Serial.print(fsrReading);
        //      Serial.print(" ");
        //Serial.print(psi);
        RFduinoBLE.sendFloat(psi);
        //Serial.print(" PSI");
        break;
      }
    }
  }
}
