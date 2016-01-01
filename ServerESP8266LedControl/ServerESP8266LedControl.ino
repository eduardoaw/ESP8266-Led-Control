/*
  Server ESP8266 Control LED
 
 Turns on and off a light emitting diode(LED) connected to digital  
 pin 13, when pressing a pushbutton attached to pin 2. 
 
 
 The circuit:
 * LED attached from pin 13 to ground 
 * Module WiFi ESP8266 
 * 1K Ohms resistor attached to CH_PD ESP8266 and 3,3v
 * 10K Ohms resistor attached to RX ESP8266 and ground
 * External power supply 3,3v - 5v YwRobot
 
 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.
 
 created 2015
 by Eduardo Adams Wohlfahrt <http://eduardoaw.github.io>
 
 This code is used in this article http://eduardoaw.github.io/2015-12-20-ESP-8266-Primeiros-Passos
 Visit ;)
 */

#include <SoftwareSerial.h>
#include <TextFinder.h>

SoftwareSerial ESP(2, 3); //RX | TX
TextFinder FINDER(ESP);  

const int LED_PIN =  13;

String lenght = "";
String response = "";

//String nameWiFi = "Your WiFi";
//String passwdWiFi = "Password WiFi";
//String yourIP = "Your IP Address";

//String addrServer = "Your Server";
//int portServer = 0; // Port on your server

void setup() { 
                      //baud rate = 9600, 57600 or 115200
  Serial.begin(9600);
  ESP.begin(9600); 
  pinMode(LED_PIN, OUTPUT);
  
  delay(1000);
  
  Serial.println("Booting...");

  getRet(); //Checks whether something in the buffer

  ESP.print("AT+RST\r\n");  //Reset by software
  getRet();
  delay(10000);
  
  ESP.print("AT+CWMODE=3\r\n"); //Access point and station
  getRet();
  delay(5000);
  
  String cmdConnectWiFi = "AT+CWJAP=\"";
  cmdConnectWiFi += nameWiFi;
  cmdConnectWiFi += "\",\"";
  cmdConnectWiFi += passwdWiFi;
  cmdConnectWiFi += "\"\r\n";
  
  ESP.print(cmdConnectWiFi); //Connect to WiFi network
  getRet();
  delay(5000);

  String cmdFixYourIP = "AT+CIPSTA=\"";
  cmdFixYourIP += yourIP;
  cmdFixYourIP += "\"\r\n";
  
  ESP.print(cmdFixYourIP); //Fix your IP
  getRet();
  delay(5000);

  ESP.print("AT+CIPMUX=1\r\n"); //Multiple connections
  getRet();
  delay(5000);

  String cmdConnectServer = "AT+CIPSERVER=1,";
  cmdConnectServer += portServer;
  cmdConnectServer += "\r\n";
  
  ESP.print(cmdConnectServer); //Up server
  getRet();
  
  Serial.println("You are now in Simple Socket IO can select Client Mode \r\nand enter the IP/Port given to the ESP266 and connect");
  
}

void loop() {
  
  if(FINDER.find("+IPD,")) {
     
     char aux = ESP.read(); 
     Serial.print("ID: "); 
     Serial.println(aux);

     aux = ESP.read();
     while(aux != ':') {
       if(aux != ',')
          lenght += aux;
        aux = ESP.read(); 
     }
     
     Serial.print("Size of message: ");
     Serial.println(lenght);
     for(int i = 0; i < lenght.toInt(); i++) {
       aux = ESP.read();
       response += aux;
     }
     Serial.print("Message: ");
     Serial.println(response);
     
     handleCommand(response, lenght.toInt());
     
     lenght = "";
     response = "";
  }  
}

void getRet() {
  
    delay(1000);
    while(ESP.available()) {
      char response = ESP.read();
      Serial.print(response);
    }
}

void handleCommand(String response, int lenght) {

   if(response == "L") {
     digitalWrite(LED_PIN, HIGH);
     ESP.print("AT+CIPSEND=0,9\r\n"); 
     while(!FINDER.find(">")) {
     }
     ESP.print("LED IS ON"); //If you change the return message here, remember to adjust the message size in the previous command "AT+CIPSEND=0,9" (LED IS ON = 9) ;)
   }
   else if(response == "D") {
     digitalWrite(LED_PIN, LOW);
     ESP.print("AT+CIPSEND=0,10\r\n");
     while(!FINDER.find(">")) {
     }
     ESP.print("LED IS OFF"); //If you change the return message here, remember to adjust the message size in the previous command "AT+CIPSEND=0,10" (LED IS OFF = 10) ;)
   }
   else {
     
     String command = "AT+CIPSEND=0,";
     command += 18 + lenght;
     command += "\r\n";
     
     ESP.print(command);
     while(!FINDER.find(">")) {
     }
     
     String ret = "COMMAND "; //If you change the return message here, remember to adjust the message size in the previous command "command += 18 + lenght;" (COMMAND  NOT FOUND = 18) ;)
     ret += response;
     ret += " NOT FOUND";
     
     ESP.print(ret); 
   }
}
