#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

// MasterSwapRoles
// code for the nano 

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

#define CE_PIN   7
#define CSN_PIN 8 

const byte slaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

struct dataToSend {
  int steeringval;
};
dataToSend data1;

float dataReceived[2];   


bool newData = false;

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000;
unsigned long lastReceiveTime = 0;

// send once per second

//============

void setup() {

    Serial.begin(9600);

    Serial.println("MasterSwapRoles Starting");

    radio.begin();
    radio.setDataRate( RF24_250KBPS );

    radio.openWritingPipe(slaveAddress);
    radio.openReadingPipe(1, masterAddress);
    radio.setPALevel(RF24_PA_MIN);
    radio.setRetries(3,5); // delay, count
    send(); // to get things started
    prevMillis = millis(); // set clock
    lcd.init(); // set LCD
  lcd.backlight();
}

//=============

void loop() {
    currentMillis = millis();
    if (currentMillis - prevMillis >= txIntervalMillis) {
        send();
        prevMillis = millis();
    }
    getData();
    showData();
}

//====================

void send() {

        radio.stopListening();
            data1.steeringval = map(analogRead(A1),0,1023,0,255);
            bool rslt;
            rslt = radio.write( &data1, sizeof(dataToSend) );
        radio.startListening();
        Serial.print("Data Sent ");
        Serial.print(data1.steeringval);
        if (rslt) {
            Serial.println("  Acknowledge received");
            
        }
        else {
            Serial.println("  Tx failed");
        }
}

void getData() {

    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        lastReceiveTime = millis();
        newData = true;
    }
    currentMillis = millis();
  if (currentMillis - lastReceiveTime > 3000) {
    lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("Connection Lost!");
  delay(1000);
  }
}

//================

void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        Serial.print(dataReceived[0]);
        Serial.print(", ");
        Serial.print(dataReceived[1]);
        Serial.print(", ");
        Serial.println(sizeof(dataReceived));
        Serial.println();
    lcd.setCursor(2,1);
    lcd.print("Temp: ");
    lcd.print(dataReceived[0]);
    lcd.print(" Deg C");
    lcd.setCursor(2,2);
    lcd.print("Humidity: ");
    lcd.print(dataReceived[1]);
    lcd.print(" %");
        newData = false;
    }
}

//================
