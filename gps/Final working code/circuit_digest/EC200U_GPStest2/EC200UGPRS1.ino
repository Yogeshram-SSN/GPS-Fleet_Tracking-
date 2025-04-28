// EC200UGPRS1 dt 30 March2025 Working OK
#define SerialMon Serial
#define SerialAT Serial2

#define MODEM_BAUDRATE 115200

String latitude1 = "";
String longitude1 = "";

const char apn[] = "jionet";  // Replace with your APN
const char user[] = "";    // Replace with your APN username (if any)
const char pass[] = "";    // Replace with your APN password (if any)

const char* server = "http://yogeshram.in/api/gpsdata.php?";  // URL to access
//const char* server = "http://yogeshram.in/api/gpsdata.php?lat=12.111112&lng=80.222222";  // URL to access
void setup() {
  SerialMon.begin(115200);         // Serial monitor
  SerialAT.begin(MODEM_BAUDRATE);  // Serial connection to modem

  SerialMon.println("Initializing modem...");

  modem_init();  // Send initialization commands
//----------------------------------------------------
// Turn off GPS (if previously on)
  sendATCommand("AT+QGPSEND");
  delay(2000);  // Ensure GPS is off
  // Start GPS
  sendATCommand("AT+QGPS=1");
  delay(10000);  // Wait for GPS to initialize
//--------------------------------------------------

  // Configure GPRS settings
  sendATCommand(String("AT+QICSGP=1,1,\"") + apn + "\",\"" + user + "\",\"" + pass + "\",1");
  sendATCommand("AT+QIACT=1");  // Activate PDP context

  //sendATCommand("AT+QHTTPCFG=\"contenttype\",2");

  SerialMon.println("Connected to network.");
 // sendHttpGetRequest();
}
void loop() {
// Query GPS location
  SerialAT.println("AT+QGPSLOC=2");
  delay(2000);  // Short wait for a response

  String response = readATResponse();

  if (response.startsWith("+QGPSLOC:")) {
    parseAndPrintLocation(response);
    //-----------------------------------------
          
    //-----------------------------------------
  } else {
    SerialMon.println("No valid GPS data available yet.");
  }
  delay(5000);  // Wait 5 seconds before the next query
//----------------------------
//  sendHttpGetRequest();
  delay(30000);

//-----------------------------  
}
void modem_init() {
  sendATCommand("AT");         // Basic AT command to check communication
  sendATCommand("ATE0");       // Disable echo for cleaner responses
  sendATCommand("AT+CPIN?");   // Check SIM status
  sendATCommand("AT+CSQ");     // Check signal quality
  sendATCommand("AT+CREG?");   // Check network registration status
  sendATCommand("AT+CGATT?");  // Check if GPRS is attached
}
void sendATCommand(const String& command) {
  SerialMon.print("Sending: ");
  SerialMon.println(command);
  SerialAT.println(command);

  delay(1000);  // Allow some time for the modem to respond

  while (SerialAT.available()) {
    String response = SerialAT.readString();
    SerialMon.print("Response: ");
    SerialMon.println(response);
  }
}
//-----------------------------------
String readATResponse() {
  String response = "";
  long timeout = millis() + 3000;  // 3-second timeout for reading the response

  while (millis() < timeout) {
    while (SerialAT.available()) {
      char c = SerialAT.read();
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
      //------------------------------------------

SerialMon.println("Sending HTTP GET request...");

          // Set up HTTP GET request
          sendATCommand("AT+QHTTPCFG=\"contextid\",1");                            // Use PDP context 1
          // sendATCommand(String("AT+QHTTPURL=") + String(strlen(server) + latitude1 + longitude1 + ",80") );  // Set URL
          // sendATCommand(String("AT+QHTTPURL=") + String(strlen(server)) + ",80");  // Set URL
          sendATCommand(String("AT+QHTTPURL=") + 61 + ",80");  // Set URL
          // SerialMon.println(server + String("lat=") + String(latitude1) + String("lng=") + String(longitude1) );
          SerialMon.print(server );
          SerialMon.print("Lat=" + latitude1);
          SerialMon.print("&Lng=" + longitude1);
          SerialMon.println(server + String(latitude1) + String(longitude1) );
          SerialAT.print(server + String("lat=") + String(latitude1) + String("&lng=") + String(longitude1) );
         // SerialAT.print(server + String("lat=") + String(latitude1) + String("&lng=") + String(longitude1) );
          SerialAT.write(0x1A);  // End of input with Ctrl+Z
          delay(1000);

          // Send HTTP GET request
          sendATCommand("AT+QHTTPGET=80");

          // Read HTTP response
          sendATCommand("AT+QHTTPREAD");

          // Disconnect HTTP session
          sendATCommand("AT+QHTTPEND");


      //--------------------------------------
     } else {
    SerialMon.println("Failed to parse GPS data.");
  }
}
