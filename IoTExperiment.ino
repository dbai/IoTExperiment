#include "rgb_lcd.h"
rgb_lcd lcd;
const int colorR = 0;
const int colorG = 255;
const int colorB = 0;

const int B = 3975;
const int pinTemp = A0;
const int pinSound = A1;
const int pinLight = A2;
const int TouchPin=5;

boolean connected = false;
//const String ssid = "Applewifi";
//const String password = "Me7W8bR4j2";
const String ssid = "David Pai iPhone";
const String password = "12345678";
//const String ssid = "swan_ap";
//const String password = "19553540";;

//const String host = "api-m2x.att.com";
//const String port = "80";
const String host = "paitest.com";
//const String host = "172.20.10.4";
const String port = "8080";
String startString = "AT+CIPSTART=\"TCP\",\"" + host + "\"," + port;
String dataToSend;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(TouchPin, INPUT);
  
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);  
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:

  lcd.clear();
  
  if (connected == false) {
//    showMode();
//    changeMode();
//    showMode(); 
//    listAPs();
//    showIP();

    connectWifi();
    showIP();
  }   

//  ping(host);
  postToCHT(getTemperature(), getSound(), getLight(), getTouch());      
}

void connectWifi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting Wifi");
  
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  while(1) {
    char c1 = 0;
    if(Serial.available() >= 2) {
        c1 = Serial.read();
        if(c1 == 'O' && 'K' == Serial.read()) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wifi connected");
          delay(1000);
          
          connected = true;
          break;       // OK means over
        }
    }
  } 
  
  delay(2000); 
  serialFlush();
}

void showIP(){
  String ip; 
  Serial.println("AT+CIFSR");
  delay(2000);
  if (Serial.available() > 0) {    
    ip = ip + Serial.readString();
  }  

  //Parse out the IP address
  String tmp = "+CIFSR:STAIP,";
  int i = ip.indexOf(tmp);
  ip = ip.substring(i + tmp.length() + 1, ip.indexOf('\"', i + tmp.length() + 1));
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(0, 1);
  lcd.print(ip); 

  delay(2000); 
  serialFlush();
} 

float getTemperature() {
  float temperature;

  // Get the (raw) value of the temperature sensor.
  int val = analogRead(pinTemp);

  // Determine the current resistance of the thermistor based on the sensor value.
  float resistance = (float)(1023-val)*10000/val;

  // Calculate the temperature based on the resistance value.
  temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: " + String(temperature));
  return temperature;
}

int getSound() {
  return analogRead(pinSound);  
}

int getLight() {
  return analogRead(pinLight);
}

int getTouch() {
  return digitalRead(TouchPin);
}

void postToCHT(float temperature, int sound, int light, int touch) {
  
///////////////////START
  String result = String();
  String startString = "AT+CIPSTART=\"TCP\",\"" + host + "\"," + port;
  Serial.println(startString);
  delay(5000);

  while(1) {
    char c1 = 0;
    if(Serial.available() >= 2) {
        c1 = Serial.read();
        if(c1 == 'O' && 'K' == Serial.read()) {
          lcd.clear();
          lcd.setCursor(0, 0);        
          lcd.print("CONNECT OK");
          delay(1000);
          break;       // OK means over
        }
    }
  }  


  delay(2000);   
  serialFlush();  

///////////////////SEND
//  dataToSend = "PUT /v2/devices/0a1cebafb1e929a7bb4dce7c0ffab0ca/streams/temperature/value HTTP/1.1\r\nHost: api-m2x.att.com\r\nContent-Type: application/json\r\nX-M2X-KEY: 1fcbfc67692a96d0b0180216e4ea8fa4\r\nContent-Length: 12\r\n\r\n{“value”:”30”}\r\n";  
//  dataToSend = "GET /agent.php?temperature=" + String(temperature) + 
//      "&sound=" + String(sound) + "&light=" + String(light) + "&touch=" + String(touch) + 
//      " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  dataToSend = "GET /cht-iot-test.php?temperature=" + String(temperature) + 
      "&sound=" + String(sound) + "&light=" + String(light) + "&touch=" + String(touch) + 
      " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";

  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("Data length: " + String(dataToSend.length()));
  
  Serial.println("AT+CIPSEND=" + String(dataToSend.length()));
  delay(5000); 
  
  while(1) {
    if (Serial.available() > 0) {    
      result = result + Serial.readString();
      if (result.indexOf('>') != -1)
        break;
    }
    delay(10);
  } 
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(">");  

  delay(2000);
  serialFlush();   

///////////////////SEND DATA
  result = String();  
  Serial.println(dataToSend);
  delay(10000);  
  if (Serial.available() > 0) {    
    result = result + Serial.readString();
  }  

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Data was sent.");

  delay(2000);    
  serialFlush();    

///////////////////CLOSE   
  result = String();  
  Serial.println("AT+CIPCLOSE");
  delay(2000);

  while(1) {
    char c1 = 0;
    if(Serial.available() >= 2) {
        c1 = Serial.read();
        if(c1 == 'O' && 'K' == Serial.read()) {
          lcd.clear();
          lcd.setCursor(0, 0);        
          lcd.print("CLOSE OK");
          delay(1000);
          break;       // OK means over
        }
    }
  } 

  delay(2000);
  serialFlush(); 
}

/////////////////////////
void serialFlush(){
  byte w = 0;  
  for (int i = 0; i < 10; i++) {
   while (Serial.available() > 0) {
     char k = Serial.read();
     w++;
     delay(10);
   }
   delay(10);
  }
}
/////////////////////////

void showMode() {
  String mode;
  Serial.println("AT+CWMODE?");
  delay(3000);
  if (Serial.available() > 0) {    
    mode = mode + Serial.readString();
  }  

  lcd.clear(); 
  lcd.setCursor(0, 0);  
  lcd.print("Mode:" + mode); 
  delay(2000); 
  serialFlush();
}

void changeMode() {
  String result;
  Serial.println("AT+CWMODE=1");
  delay(3000);
  if (Serial.available() > 0) {    
    result = result + Serial.readString();
  }  

  lcd.clear(); 
  lcd.setCursor(0, 0);
  lcd.print("Changed:" + result); 
  delay(2000); 
  serialFlush();
}

void listAPs() {
  String result;
  String line;
  Serial.println("AT+CWLAP");
  delay(5000);
  while(1) {
    if (Serial.available() > 0) {    
      line = Serial.readStringUntil('\n');  
      Serial.read();    
      if (!line.startsWith("OK"))
        result = result + line + "\n";
      else
        break;
    }  
  }

  Serial.println("APs: " + result);
  delay(2000); 
  serialFlush();
}

void ping(String ip) {
  Serial.println("AT+PING=\"" + host + "\"");
  delay(5000);

  while(1) {
    char c1 = 0;
    if(Serial.available() >= 2) {
        c1 = Serial.read();
        if(c1 == 'O' && 'K' == Serial.read()) {
          lcd.clear();
          lcd.setCursor(0, 0);        
          lcd.print("PING OK");
          delay(1000);
          break;       // OK means over
        }
    }
  } 

  delay(2000); 
  serialFlush();  
}

