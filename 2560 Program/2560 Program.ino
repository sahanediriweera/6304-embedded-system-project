String data="";
int mark = 0;
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

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println(0);
  delay(1000);
}

void loop(){
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
        //Serial.println(4);
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

      /*
      Serial.print("Date:");
      Serial.println(Date);
      Serial.print("UTCtime:");
      Serial.print(RMCUTCtime);
      Serial.print("   ");
      Serial.println(GGAUTCtime);
      */
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
        if(dist_calc>100.0){
        total_distance += dist_calc;
        prelong = nowlong;
        prelatt = nowlatt;
        }
        Serial.print("Now Distance   ");        
        Serial.println(dist_calc);        
        Serial.print("total distance   ");              
        Serial.println(total_distance);
                             
      /*
      Serial.print("GPStatus:");
      Serial.println(GPStatus);
      Serial.print("SatelliteNum:");
      Serial.println(SatelliteNum);
      Serial.print("HDOPfactor:");
      Serial.println(HDOPfactor);
      Serial.print("Height:");
      Serial.println(Height);
      Serial.print("Speed:");
      Serial.println(Speed);
      Serial.print("Direction:");
      Serial.println(Direction);
      Serial.print("Declination:");
      Serial.println(Declination);
      Serial.print("Mode:");
      Serial.println(Mode);
      */     
      valid=false;
    }
    if(Serial1.find("$")){
      Mark_Start=true;
    }
  }

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
        //Serial.println("meaningful message");
        return value;
      }
      else {
        //Serial.println("empty");
        return "";
      }     
    }
    else if(temp=='$'){
      //Serial.println("failure");
      Mark_Start=false;
    }
    else{
      //Serial.println("add");
      value+=char(temp);
    }
  }
  while (!(Serial1.available()>0)){
  }
  goto startover;
}
