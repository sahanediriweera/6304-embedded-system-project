#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 9  
#define RST_PIN 8  
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;          
int blockNum = 2;
int rem=0;
byte blockData [16] = {"1111111111111111"};
byte bufferLen = 18;
byte readBlockData[18];
MFRC522::StatusCode status;

int cardIDs[100];
float passengerdistances[100];
int passengercount = 0;
int tempIdPosition;
float cost = 10;

byte bytearray[16] = {"0000111100001111"};


#include <LiquidCrystal.h>
String data="";
int mark = 0;
bool flag =true;
boolean Mark_Start=false;
boolean valid=false;
String GGAUTCtime,GGAlatitude,GGAlongitude,GPStatus,SatelliteNum,HDOPfactor,Height,
PositionValid,RMCUTCtime,RMClatitude,RMClongitude,Speed,Direction,Date,Declination,Mode;

bool initialized_coordinates = 0;
double prelong = -1.0;
double prelatt = -1.0;
double nowlong = -1.0;
double nowlatt = -1.0;
double total_distance = 0.0;
//                RS,E,D4,D5,D6,D7
LiquidCrystal lcd(6,7,10,11,12,13);
void setup() 
{
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  initCardIDs();
  Serial1.begin(9600);
  delay(1000);
  lcd.begin(16, 2);
 
}

void loop()
{
  while (Serial1.available()> 0){
    if(Mark_Start){
      data=reader();
      if(data.equals("GPGGA")){
        //Serial.println(1);
        GGAUTCtime=reader();
        GGAlatitude=reader();
        GGAlatitude+=reader();
        GGAlongitude=reader();
        GGAlongitude+=reader();
        GPStatus=reader();
        SatelliteNum=reader();
        HDOPfactor=reader();
        Height=reader();
        Mark_Start=false;
        valid=true;
        data="";

      }
      else if(data.equals("GPGSA")){
        Mark_Start=false;
        data="";
      }
      else if(data.equals("GPGSV")){
        Mark_Start=false;
        data="";
      }
      else if(data.equals("GPRMC")){
        RMCUTCtime=reader();
        PositionValid=reader();
        RMClatitude=reader();
        RMClatitude+=reader();
        RMClongitude=reader();
        RMClongitude+=reader();
        Speed=reader();
        Direction=reader();
        Date=reader();
        Declination=reader();
        Declination+=reader();
        Mode=reader();
        valid=true;
        Mark_Start=false;
        data="";
      }
      else if(data.equals("GPVTG")){
        Mark_Start=false;
        data="";
      }
      else{
        Mark_Start=false;
        data="";
      }
    }
    if(valid){
      if(PositionValid=="A"){
      }
      else{
        Serial.println("Your position is not valid.");
      }
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

        if(!initialized_coordinates){
          if (prelong == -1.0 && prelatt == -1.0 )
          {
            prelong = atof(RMClongitude.c_str());
            prelatt = atof(RMClatitude.c_str());
            initialized_coordinates = true;
            delay(1000);
          }
        }
                    
        float flat1 = prelatt;
        float flon1 = prelong;
        float dist_calc = 0;
        float dist_calc2 = 0;
        float dist_calcKM = 0;
        float diflat = 0;
        float diflon = 0;
        float x2lat = nowlatt;
        float x2lon = nowlong;
  
        diflat = radians(x2lat - flat1);
        flat1 = radians(flat1);
        x2lat = radians(x2lat);
        diflon = radians((x2lon) - (flon1));
        dist_calc = (sin(diflat / 2.0) * sin(diflat / 2.0));
        dist_calc2 = cos(flat1);
        dist_calc2 *= cos(x2lat);
        dist_calc2 *= sin(diflon / 2.0);
        dist_calc2 *= sin(diflon / 2.0);
        dist_calc += dist_calc2;
        dist_calc = (2 * atan2(sqrt(dist_calc), sqrt(1.0 - dist_calc)));
        dist_calc *= 6371000.0;
        dist_calcKM = dist_calc / 1000;
        if(dist_calc>100.0 && dist_calc <500.0){
        total_distance += dist_calc;
        prelong = nowlong;
        prelatt = nowlatt;
        }
        if(dist_calc>2000.0){
          prelong = nowlong;
          prelatt = nowlatt;
        }        
        Serial.print("Now Distance   ");        
        Serial.println(dist_calc);        
        Serial.print("total distance   ");              
        Serial.println(total_distance);       
        valid=false;

        
        if(dist_calc >0.0 || total_distance >0.0){
            flag= false;
            lcd.clear();           
                         
        } 
        if(flag){
            lcd.clear();           
            lcd.print("Calibrting");
            lcd.blink();
                        
        }        
        for (byte i = 0; i < 6; i++)
        {
          key.keyByte[i] = 0xFF;
        }
        if ( ! mfrc522.PICC_IsNewCardPresent())
        {
        }
        if ( ! mfrc522.PICC_ReadCardSerial()) 
        {
        }
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        ReadDataFromBlock(blockNum, readBlockData);
        Serial.print("Data :");
        Serial.println(readBlockData[0]);                
        if (mfrc522.PICC_IsNewCardPresent())
        {
          if(readBlockData[0] == 65){
            tempIdPosition = 0;
          }else if(readBlockData[0] == 66){
            tempIdPosition = 1;            
          }else if(readBlockData[0] == 67){
            tempIdPosition = 2;           
          }else if(readBlockData[0] == 68){
            tempIdPosition = 3;            
          }else{
            tempIdPosition = 4;
          }

          Serial.print("Temp Id Position  ");
          Serial.println(tempIdPosition);          
          if((tempIdPosition == 0) || (tempIdPosition == 1) || (tempIdPosition == 2) || (tempIdPosition == 3) ){
            if(passengerdistances[tempIdPosition]==-1){
              passengerdistances[tempIdPosition] = total_distance;
              Serial.println("Card Accepted");
              lcd.clear();
              lcd.print("Enter to the Bus");
              delay(5000);
              lcd.clear();              
            }
            else{
              float price = cost*(total_distance-passengerdistances[tempIdPosition]);
              passengerdistances[tempIdPosition] = -1.0;
              Serial.print("Price .............");
              Serial.println(price);
              lcd.print("Total Amount:Rs."); 
              lcd.print(price);             
              delay(7000);
              lcd.clear();
              lcd.print("Exit from the Bus");
              delay(5000); 
              lcd.clear();                                         
              }
          }                             
        }
        

        
    }
    if(Serial1.find("$")){
      Mark_Start=true;
    }
  }
    lcd.clear();  
}



void WriteDataToBlock(int blockNum, byte blockData[])
{
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Data was written into Block successfully");
  }
  
}

void ReadDataFromBlock(int blockNum, byte readBlockData[])
{
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK)
  {
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  else
  {
    Serial.println("Authentication success");
    lcd.print("Successfull");
    delay(500);  
    lcd.clear();
           
  }

  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Block was read successfully");  
    for (int j=0 ; j<16 ; j++)
    {
     Serial.write(readBlockData[j]);
    }
  Serial.print("\n"); 
  }
}

bool checkEmpty(){

  bool present = true;
    if ((bytearray[8]== readBlockData[8]) && (bytearray[9]== readBlockData[9]) && (bytearray[10]== readBlockData[10]) && (bytearray[11]== readBlockData[11]))
    {
      present = false;
    }    
  return present;    
  }
  

  

void initCardIDs(){
  for(int i = 0;i<100;i++){
    cardIDs[i] = 0;
    passengerdistances[i] = -1.0;
  }
}

int generateRandomNumber(){
  int randomNumber = rand();
  return randomNumber % 10000;
}

String reader(){
  String value="";
  int temp;
  startover:
  while (Serial1.available() > 0){
    delay(2);
    temp=Serial1.read();
    if((temp==',')||(temp=='*')){
      if(value.length()){
        return value;
      }
      else {
        return "";
      }     
    }
    else if(temp=='$'){
      Mark_Start=false;
    }
    else{
      value+=char(temp);
    }
  }
  while (!(Serial1.available()>0)){
  }
  goto startover;
}

void takeCardtype(){

}

bool checKExist(int){
    bool present = false;
    for (int i = 0; i < passengercount; i++)
    {
    turnintobyte(cardIDs[i]);
    if ((bytearray[0]== readBlockData[0]) && (bytearray[1]== readBlockData[1]) && (bytearray[2]== readBlockData[2]) && (bytearray[3]== readBlockData[3]))
    {
      present = true;
    }
    
  }
  return present;
  } 

  void turnintobyte(int value){
  bytearray[0] = (value >> 24) & 0xFF;
  bytearray[1] = (value >> 16) & 0xFF;
  bytearray[2] = (value >> 8) & 0xFF;
  bytearray[3] = (value >> 0) & 0xFF;
}

void setPosition(){
  tempIdPosition = -1;
    for (int i = 0; i < passengercount; i++)
    {
    turnintobyte(cardIDs[i]);
    if ((bytearray[0]== readBlockData[0]) && (bytearray[1]== readBlockData[1]) && (bytearray[2]== readBlockData[2]) && (bytearray[3]== readBlockData[3]))
    {
      tempIdPosition = i;
    }
  }             
}
