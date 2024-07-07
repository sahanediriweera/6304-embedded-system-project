# Distance-Based Bus Ticketing System

This is an embedded system project for a distance-based bus ticketing system. The system utilizes RFIDs for passenger identification and GPS for distance detection. Passengers are charged based on the distance traveled during their bus journey.

## Functionality

The distance-based bus ticketing system provides the following functionality:

- **Passenger Identification**: RFIDs are used to uniquely identify passengers and associate them with their ticketing information.
- **Distance Tracking**: GPS technology is employed to track the distance covered by the bus during the journey.
- **Fare Calculation**: Passengers are charged based on the distance traveled, with the fare automatically calculated by the system.
- **Ticketing Management**: The system manages ticket issuance, validation, and cancellation.
- **Reporting and Analytics**: Generate reports and analyze data related to passenger travel patterns, revenue, and bus performance.

## Technologies Used

- **Embedded Systems**: Hardware and software integration for developing a standalone system.
- **RFID Technology**: Utilized for passenger identification and tracking.
- **GPS Technology**: Used to measure the distance traveled by the bus.
- **Microcontroller**: Controls the system operations and interfaces with the hardware components.
- **Programming Languages**: C
- **Sensors and Peripherals**: RFIDs, GPS module, display unit, input/output devices, etc.

## Pin Configuration

### Microcontroller Connections

1. **RFID Module (RC522)**
    - **SDA** (SS): Pin 9
    - **SCK**: Pin 13
    - **MOSI**: Pin 11
    - **MISO**: Pin 12
    - **IRQ**: Not connected
    - **GND**: GND
    - **RST**: Pin 8
    - **3.3V**: 3.3V

2. **GPS Module**
    - **TX**: Pin RX of Serial1 (usually Pin 19 on some microcontrollers)
    - **RX**: Pin TX of Serial1 (usually Pin 18 on some microcontrollers)
    - **GND**: GND
    - **VCC**: 5V

3. **LCD Display**
    - **RS**: Pin 2
    - **E**: Pin 5
    - **D4**: Pin 10
    - **D5**: Pin 11
    - **D6**: Pin 12
    - **D7**: Pin 13

### Circuit Diagram

Refer to the following resources for detailed circuit diagrams:
- [LCD Screen Setup](https://www.youtube.com/watch?v=dpoBwmuE2QE&t=601s)
- [Write Data to RFID Card using RC522](https://www.electronicshub.org/write-data-to-rfid-card-using-rc522-rfid/)

## Installation and Setup

Since this project involves both hardware and software components, the installation and setup process can vary depending on the specific hardware and microcontroller used. Generally, the following steps are involved:

1. Set up the hardware components, including the microcontroller, RFIDs, and GPS module, following the manufacturer's instructions.
2. Install the necessary software development tools, such as the IDE for programming the microcontroller.
3. Write the embedded software code that handles RFID reading, GPS data processing, distance calculation, fare calculation, and other required functionalities.
4. Flash the software code onto the microcontroller.
5. Connect the hardware components to the microcontroller, ensuring proper wiring and connections.
6. Power on the system and test its functionality, verifying that RFIDs are read correctly, GPS data is received accurately, and fare calculation is working as expected.

Please refer to the project's documentation or consult the hardware manufacturer's documentation for detailed instructions on installation and setup.

## Code Example

```cpp
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 9  
#define RST_PIN 8  
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;          
int blockNum = 2;
byte blockData [16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
byte bufferLen = 18;
byte readBlockData[18];
MFRC522::StatusCode status;

int cardIDs[100];
float passengerdistances[100];
int passengercount = 0;
int tempIdPosition;
float cost = 10;

String data="";
boolean Mark_Start=false;
boolean valid=false;
String GGAUTCtime,GGAlatitude,GGAlongitude,GPStatus,SatelliteNum,HDOPfactor,Height,PositionValid,RMCUTCtime,RMClatitude,RMClongitude,Speed,Direction,Date,Declination,Mode;
bool initialized_coordinates = 0;
double prelong = -1.0;
double prelatt = -1.0;
double nowlong = -1.0;
double nowlatt = -1.0;
double total_distance = 0.0;

LiquidCrystal lcd(2, 5, 10, 11, 12, 13);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  initCardIDs();
  Serial1.begin(9600);
  delay(1000);
  lcd.begin(16, 2);
}

void loop() {
  while (Serial1.available() > 0) {
    if (Mark_Start) {
      data = reader();
      if (data.equals("GPGGA")) {
        GGAUTCtime = reader();
        GGAlatitude = reader() + reader();
        GGAlongitude = reader() + reader();
        GPStatus = reader();
        SatelliteNum = reader();
        HDOPfactor = reader();
        Height = reader();
        Mark_Start = false;
        valid = true;
        data = "";
      } else if (data.equals("GPGSA") || data.equals("GPGSV") || data.equals("GPVTG")) {
        Mark_Start = false;
        data = "";
      } else if (data.equals("GPRMC")) {
        RMCUTCtime = reader();
        PositionValid = reader();
        RMClatitude = reader() + reader();
        RMClongitude = reader() + reader();
        Speed = reader();
        Direction = reader();
        Date = reader();
        Declination = reader() + reader();
        Mode = reader();
        valid = true;
        Mark_Start = false;
        data = "";
      } else {
        Mark_Start = false;
        data = "";
      }
    }

    if (valid) {
      if (PositionValid == "A") {
        processGPSData();
      } else {
        Serial.println("Your position is not valid.");
      }
      valid = false;
    }

    if (Serial1.find("$")) {
      Mark_Start = true;
    }
  }
}

void processGPSData() {
  Serial.print("Latitude:");
  Serial.print(RMClatitude);
  Serial.print("   ");
  Serial.println(GGAlatitude);
  Serial.print("Longitude:");
  Serial.print(RMClongitude);
  Serial.print("   ");
  Serial.println(GGAlongitude);

  nowlatt = atof(RMClatitude.c_str());
  nowlong = atof(RMClongitude.c_str());

  if (!initialized_coordinates) {
    if (prelong == -1.0 && prelatt == -1.0) {
      prelong = nowlong;
      prelatt = nowlatt;
      initialized_coordinates = true;
      delay(1000);
    }
  }

  float dist_calc = calculateDistance(prelatt, prelong, nowlatt, nowlong);
  if (dist_calc > 100.0) {
    total_distance += dist_calc;
    prelong = nowlong;
    prelatt = nowlatt;
  }

  Serial.print("Now Distance: ");
  Serial.println(dist_calc);        
  Serial.print("Total Distance: ");              
  Serial.println(total_distance);
  lcd.setCursor(0, 1);
  lcd.print(dist_calc);          

  handlePassengerData();
}

float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
  float diflat = radians(lat2 - lat1);
  float diflon = radians(lon2 - lon1);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  float dist_calc = (sin(diflat / 2.0) * sin(diflat / 2.0)) + cos(lat1) * cos(lat2) * (sin(diflon / 2.0) * sin(diflon / 2.0));
  return (2 * atan2(sqrt(dist_calc), sqrt(1.0 - dist_calc))) * 6371000.0;
}

void handlePassengerData() {
  takeCardtype();
  ReadDataFromBlock(blockNum, readBlockData);

  if (checkEmpty()) {
    bool exist = true;
    int num;
    while (exist) {
      num = generateRandomNumber();
      exist = checKExist(num);
    }
    cardIDs[++passengercount] = num;
    passengerdistances[passengercount] = total_distance;
  } else {
    float distance_travelled = total_distance - passengerdistances[tempIdPosition];
    float price = distance_travelled * cost;
  }
}

void WriteDataToBlock(int blockNum, byte blockData[]) {
  status = mfrc522.PCD_Authenticate(MFRC522::

PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Write() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  Serial.println("Block was written");
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  byte byteCount = 16;
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &byteCount);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  Serial.println("Block was read");
  Serial.println(readBlockData[0]);
}

int generateRandomNumber() {
  return random(1000, 1000000);
}

boolean checkEmpty() {
  if (readBlockData[0] == 1) {
    return true;
  } else {
    tempIdPosition = 0;
    for (int i = 1; i < 16; i++) {
      tempIdPosition = tempIdPosition * 10 + readBlockData[i];
    }
    return false;
  }
}

void takeCardtype() {
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);

  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  if (mfrc522.PICC_RequestA(bufferATQA, &bufferSize) == MFRC522::STATUS_OK) {
    if (mfrc522.PICC_GetType(mfrc522.uid.sak) == MFRC522::PICC_TYPE_MIFARE_1K) {
      Serial.println(F("Type: MIFARE 1K"));
    } else if (mfrc522.PICC_GetType(mfrc522.uid.sak) == MFRC522::PICC_TYPE_MIFARE_UL) {
      Serial.println(F("Type: MIFARE Ultralight"));
    } else {
      Serial.println(F("Card Type not recognized"));
    }
  }

  mfrc522.PICC_HaltA();
}

void initCardIDs() {
  for (int i = 0; i < 100; i++) {
    cardIDs[i] = -1;
    passengerdistances[i] = 0.0;
  }
}

boolean checKExist(int n) {
  for (int i = 0; i < 100; i++) {
    if (cardIDs[i] == n) {
      return true;
    }
  }
  return false;
}

String reader() {
  String temp = "";
  while (Serial1.available() > 0) {
    char ch = Serial1.read();
    if (ch == ',' || ch == '*') {
      break;
    }
    temp += ch;
  }
  return temp;
}
```

## Acknowledgements

- Reference projects and tutorials for LCD and RFID integration.
- GPS data processing and calculations from available resources.

This project provides a comprehensive solution for a distance-based bus ticketing system using embedded systems, RFID, and GPS technology. It can be extended and customized based on specific requirements and hardware components.
