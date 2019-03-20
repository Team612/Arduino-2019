#include <Wire.h>
#define buzzer 12//Define the buzzer port
const int alertLed=13;//Defines status LED
int jobSwitch=0;//Decides what command to execute
int stripCount=1;
int selectedStrip=0;
int color=0;
//power pin,R pin,G pin,B pin
int stripPins[1][4]={{3,5,6}};
int stripVals[1][4]={{0,0,0}};
String infoIn="";//Stores command
boolean buzz=true;//Decides whether to beep when command received

void setup() {
 pinMode(alertLed,OUTPUT);
 Serial.begin(9600);
 Serial.println("REC READY");
 Wire.begin(1);//Start I2C as a slave with address 1
 Wire.onReceive(i2cIn);
 Wire.onRequest(i2cOut);
 //Pin delcarations
 for(int i=0;i<1;i++){
  for(int j=0;j<4;j++){
    pinMode(stripPins[i][j],OUTPUT);
  }
 }
}
boolean rainbow=false;

void loop() {
  if(rainbow)doRainbow();
}

int red=10;
int green=10;
int blue=10;
int rainbowDelay=15;
int minBright=10;
int maxBright=175;
boolean dir=true;
void doRainbow(){
  delay(rainbowDelay);
  if(dir){
    if(red<maxBright){
      red++;
    }else{
      if(green<maxBright){
        green++;
      }else{
        if(blue<maxBright){
          blue++;
        }else{
          dir=false;
        }
      }
    }
  }else{
    if(red>minBright){
      red--;
    }else{
      if(green>minBright){
        green--;
      }else{
        if(blue>minBright){
          blue--;
        }else{
          dir=true;
        }
      }
    }
  }
  analogWrite(stripPins[selectedStrip][0],red);
  analogWrite(stripPins[selectedStrip][1],green);
  analogWrite(stripPins[selectedStrip][2],blue);
}

void i2cIn(int howMany){//Receives data from I2C bus and parses it
  alert(400,100,true);
  infoIn="";
  while(Wire.available()>=1){
    char c=Wire.read();
    infoIn+=c;
  }
  Serial.print("Received:");
  Serial.println(infoIn);
  parseCommand();
}

void i2cOut(){
  //Might be used later
}

void parseCommand(){//Parses command and sets values
  if(jobSwitch>0){
    alert(350,100,true);
    parseIntInput();
    return;
  }
  if(infoIn.indexOf("SET_STRIP")>=0){
    Serial.println("STR");
    jobSwitch=1;
  }else if(infoIn.indexOf("SET_COLOR")>=0){
    Serial.println("COL");
    rainbow=false;
    jobSwitch=2;
  }else if(infoIn.indexOf("SET_BRIGHT")>=0){
    Serial.println("BRI");
    rainbow=false;
    jobSwitch=3;
  }else if(infoIn.indexOf("SET_SOUND")>=0){
    Serial.println("SPK");
    jobSwitch=4;
  }else if(infoIn.indexOf("PING")>=0){
    Serial.println("PNG");
    if(buzz)alert(300,100,true);
  }else if(infoIn.indexOf("SET_RAIN")>=0){
    Serial.println("BOW");
    jobSwitch=5;
  }else if(infoIn.indexOf("SET_RMAX")>=0){
    Serial.println("RMaxBright");
    jobSwitch=6;
  }else if(infoIn.indexOf("SET_RMIN")>=0){
    Serial.println("RMinBright");
    jobSwitch=7;
  }else if(infoIn.indexOf("SET_RTIME")>=0){
    Serial.println("RTime");
    jobSwitch=8;
  }else{
    Serial.println("E00");
    alert(200,100,true);
  }
  if(jobSwitch>0)alert(300,100,true);
}
 
void parseIntInput(){
  char x[32];
  infoIn.toCharArray(x,32);
  switch(jobSwitch){
    case 1: setLedStrip(x);
    break;
    case 2: setColor(x);
    break;
    case 3: setBrightness(x);
    break;
    case 4: setSpeaker(x);
    break;
    case 5: setRainbow(x);
    break;
    case 6: setMaxRain(x);
    break;
    case 7: setMinRain(x);
    break;
    case 8: setRainTime(x);
    break;
  }
  Serial.println("READY");
  jobSwitch=0;
}

void setLedStrip(char x[]){
  int s=atoi(x);
  if(s>0||s<0){
    s=0;
  }else{
    Serial.print("STRIP->");
    Serial.println(s);
    selectedStrip=s;
  }
}

void setRainTime(char x[]){
  int b=atoi(x);
  if(b<0){
    b=0;
  }
  rainbowDelay=b;
  Serial.print("RAIN_TIME->");
  Serial.println(b);
}

void setColor(char x[]){
  int c=atoi(x);
  if(c<0){
    c=0;
  }else if(c>2){
    c=2;
  }
  color=c;
  Serial.print("COLOR->");
  switch(c){
    case 0: Serial.println("RED");
    break;
    case 1: Serial.println("GREEN");
    break;
    case 2: Serial.println("BLUE");
    break;
  }
}

void setBrightness(char x[]){
  int b=atoi(x);
  if(b<0){
    b=0;
  }else if(b>255){
    b=255;
  }
  analogWrite(stripPins[selectedStrip][color],b);
  Serial.print("BRIGHT->");
  Serial.println(b);
}

void setMaxRain(char x[]){
  int b=atoi(x);
  if(b<0){
    b=0;
  }else if(b>255){
    b=255;
  }
  maxBright=b;
  Serial.print("RAIN_BRIGHT_MAX->");
  Serial.println(b);
}

void setMinRain(char x[]){
  int b=atoi(x);
  if(b<0){
    b=0;
  }else if(b>255){
    b=255;
  }
  minBright=b;
  Serial.print("RAIN_BRIGHT_MIN->");
  Serial.println(b);
}

void setSpeaker(char x[]){
  int s=atoi(x);
  Serial.print("SOUND->");
  if(s<=0){
    buzz=false;
    Serial.println("OFF");
  }else{
    buzz=true;
    Serial.println("ON");
  }
}

void setRainbow(char x[]){
  int s=atoi(x);
  Serial.print("RAINBOW->");
  if(s<=0){
    rainbow=false;
    analogWrite(stripPins[selectedStrip][0],0);
    analogWrite(stripPins[selectedStrip][1],0);
    analogWrite(stripPins[selectedStrip][2],0);
    Serial.println("OFF");
  }else{
    dir=true;
    red=10;
    blue=10;
    green=10;
    rainbow=true;
    Serial.println("ON");
  }
}

void alert(int pitch,int sLength,boolean useLED){
  if(useLED)digitalWrite(alertLed,HIGH);
  if(buzz)tone(buzzer,pitch,sLength);
  delay(sLength);
  digitalWrite(alertLed,LOW);
}
