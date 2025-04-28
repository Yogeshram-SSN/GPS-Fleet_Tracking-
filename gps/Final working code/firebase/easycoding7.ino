//easycoding7.ino 20March2025..
#include<HardwareSerial.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
//#define FIREBASE_HOST "https://esp32-data-7a315.firebaseio.com/"
#define FIREBASE_HOST "https://esp32-data-7a315-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "svCZCrXqY9kkGETXFflHdylTnnVMs4Gt0WmvdpWr"
FirebaseData firebaseData;

#define SerialMon Serial       // SerialMon monitor for debug
//#define mySerial Serial2       // SerialMon communication with EC200U-CN
HardwareSerial mySerial(1);

#define MODEM_BAUDRATE 115200  // Baudrate for modem communication

#define APN "jionet"  // Example: "internet" for most cellular networks

//String latitude = String("12.123456");
//float latitude1 = 12.889011;
//float longitude1 = 80.189068;
 String latitude1 = "";
 String longitude1 = "";

void setup()
{
SerialMon.begin(115200);
//-------------------------------------

while (!SerialMon) {}

  // Initialize Serial for EC200U-CN
  //mySerial.begin(MODEM_BAUDRATE);
  mySerial.begin(115200, SERIAL_8N1, 16, 17);  // Serial connection to modem
  delay(1000);

  SerialMon.println("Initializing modem...");
  modem_init();  // Send initialization commands

  SerialMon.println("Initializing GPS...");

  // Turn off GPS (if previously on)
  mySerial.println("AT+QGPSEND");
  delay(2000);  // Ensure GPS is off

  // Start GPS
  mySerial.println("AT+QGPS=1");
  delay(10000);  // Wait for GPS to initialize

     // Initialize GPRS and the internet connection via EC200U
      initializeGPRS();

     // Connect to Wi-Fi using the GPRS connection (via EC200U)
     WiFi.begin("", "");  // Make sure the EC200U is providing internet access
  
       while (WiFi.status() != WL_CONNECTED) {
       delay(1000);
       Serial.println("Connecting to WiFi...");
  }
         Serial.println("WiFi connected");

   //------------------------------------
      /*
    WiFi.disconnect();
    delay(3000);
    Serial.println("START");
    WiFi.begin("Yogeshram R","yogesh/1939");
    while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("..");
     
  }
  */
   //-------------------------------------
  Serial.println("Connected");
  Serial.println("Your IP is");
  Serial.println((WiFi.localIP()));
   //--------------------------------------------------
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}


void loop()
{
  //----------------------------------------------
// Query GPS location
  mySerial.println("AT+QGPSLOC=2");
  delay(2000);  // Short wait for a response

  String response = readATResponse();

  if (response.startsWith("+QGPSLOC:")) {
    parseAndPrintLocation(response);
  } else {
    SerialMon.println("No valid GPS data available yet.");
  }

  delay(5000);  // Wait 5 seconds before the next query
  //-----------------------------------------------
/*
    if ((Firebase.setString(firebaseData, "/GPS/f_latitude", latitude1)) == true) {
      Serial.println("Data Sent");

    } else {
      Serial.println("Error");

    }

    if ((Firebase.setString(firebaseData, "/GPS/f_longitude", longitude1)) == true) {
      Serial.println("Data Sent");

    } else {
      Serial.println("Error");

    }
*/
}

//-----------------------------------
String readATResponse() {
  String response = "";
  long timeout = millis() + 3000;  // 3-second timeout for reading the response

  while (millis() < timeout) {
    while (mySerial.available()) {
      char c = mySerial.read();
      response += c;
    }
    if (response.length() > 0) break;  // Stop reading if response is received
  }
  response = response.substring(2);
  SerialMon.println("Raw Response: " + response);
  return response;
}

void parseAndPrintLocation(const String &response) {
  // Example response: +QGPSLOC: 105824.00,22.5726,N,88.3639,E,10.5,0.0,0.0,230394,03
  int firstComma = response.indexOf(',');                   // Find the first comma
  int secondComma = response.indexOf(',', firstComma + 1);  // Find the second comma
  int thirdComma = response.indexOf(',', secondComma + 1);  // Find the third comma

  if (firstComma != -1 && secondComma != -1 && thirdComma != -1) {
    String latitude = response.substring(firstComma + 1, secondComma);
    String longitude = response.substring(secondComma + 1, thirdComma);

    SerialMon.println("Latitude: " + latitude);
    SerialMon.println("Longitude: " + longitude);

    //float latitude = (latitude);
    //float longitude = (longitude);
    //Serial.println(latString);
    String latitude1 = response.substring(firstComma + 1, secondComma);
    String longitude1 = response.substring(secondComma + 1, thirdComma);
   // float latitude1 = latitude.toFloat();
   // float longitude1 = longitude.toFloat();
    SerialMon.println(latitude1);
    SerialMon.println(longitude1);
       //------------------------------------------------
    if ((Firebase.setString(firebaseData, "/GPS/f_latitude", latitude1)) == true) {
      Serial.println("Data Sent");

    } else {
      Serial.println("Error");

    }

    if ((Firebase.setString(firebaseData, "/GPS/f_longitude", longitude1)) == true) {
      Serial.println("Data Sent");

    } else {
      Serial.println("Error");

    }

       //------------------------------------------------ 

  } else {
    SerialMon.println("Failed to parse GPS data.");
  }
}

void modem_init() {

  mySerial.println("AT");         // Basic AT command to check communication
  mySerial.println("ATE0");       // Disable echo for cleaner responses
  mySerial.println("AT+CPIN?");   // Check SIM status
  mySerial.println("AT+CSQ");     // Check signal quality
  mySerial.println("AT+CREG?");   // Check network registration status
  mySerial.println("AT+CGATT?");  // Check if GPRS is attached
}
/*
void mySerial.println(const char *command) {
  SerialMon.print("Sending: ");
  SerialMon.println(command);
  mySerial.println(command);
  delay(500);  // Short delay for response

  // Read and display the response
  while (mySerial.available()) {
    String response = mySerial.readString();
    SerialMon.print("Response: ");
    SerialMon.println(response);
  }
}
*/
void initializeGPRS() {
  // Start communication with EC200U
   mySerial.println("AT");
  delay(1000);
  mySerial.println("AT+CSQ");  // Check signal quality
  delay(1000);
  
  // Set up the GPRS connection
  mySerial.println("AT+CGATT=1");  // Attach to the GPRS network
  delay(1000);
 mySerial.println("AT+CGDCONT=1,\"IP\",\"" + String(APN) + "\"");  // Set the APN
 // mySerial.println("AT+CGDCONT=1,\"IP\",\"" + jionet + "\"");  // Set the APN
  //   mySerial.println("AT+QICSGP=1,1,\"JIONET\",\"",\"",0");
  delay(1000);
  mySerial.println("AT+CGACT=1,1");  // Activate the GPRS context
  delay(1000);
  mySerial.println("AT+CGPADDR");  // Check IP address
  delay(1000);
}
//------------------------------------


