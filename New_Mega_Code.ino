#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <DHT.h>
#include <DHT_U.h>
// SlaveSwapRoles

DHT dht(53, DHT11);

#define CE_PIN   24
#define CSN_PIN 22

const byte slaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

struct dataReceived {
  int steeringval;// must match dataToSend in master
} data;

float replyData[2];


bool newData = false;

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000; // send once per second


void setup() {

    Serial.begin(9600);
    dht.begin();
    Serial.println("SlaveSwapRoles Starting");

    radio.begin();
    radio.setDataRate( RF24_250KBPS );

    radio.openWritingPipe(masterAddress); // NB these are swapped compared to the master
    radio.openReadingPipe(1, slaveAddress);
    radio.setPALevel(RF24_PA_MIN);
    radio.setRetries(3,5); // delay, count
    radio.startListening();

}

//====================

void loop() {
    
    getData();
    showData();
   
    send();
}

//====================

void send() {
    if (newData == true) {
        
        radio.stopListening();
            
            bool rslt;
            rslt = radio.write( &replyData, sizeof(replyData) );
        radio.startListening();

        Serial.print("Reply Sent ");
        Serial.print(replyData[0]);
        Serial.print(", ");
        Serial.print(replyData[1]);
        Serial.print(", ");
        Serial.println(sizeof(replyData));
        if (rslt) {
            Serial.println("Acknowledge Received");
            
        }
        else {
            Serial.println("Tx failed");
        }
        Serial.println();
        newData = false;
    }
}

//================

void getData() {

    if ( radio.available() ) {
        radio.read( &data, sizeof(dataReceived) );
        newData = true;
    }
}

//================

void showData() {
    if (newData == true) {
        Serial.print("Data received ");
        Serial.println(data.steeringval);
    }
}
void checkTemp () {
 
  replyData[0] = dht.readTemperature();
 replyData[1] = dht.readHumidity();
}
//================
