#include <Wire.h>
#define buzzer 12
const int alertLed=13;
int jobSwitch=0;
int stripCount=1;
int selectedStrip=0;
int color=0;
//power pin,R pin,G pin,B pin
int stripPins[1][4]={{2,3,5,6}};
int stripVals[1][4]={{0,0,0}};
String infoIn="";
boolean buzz=true;

void setup() {
 pinMode(alertLed,OUTPUT);
 Serial.begin(9600);
 Serial.println("REC READY");
 Wire.begin(1);//Start I2C as a slave w/ address 1
 Wire.onReceive(i2cIn);
 Wire.onRequest(i2cOut);
 //Pin delcarations
 for(int i=0;i<1;i++){
  for(int j=0;j<4;j++){
    pinMode(stripPins[i][j],OUTPUT);
  }
 }
}

void loop() {}

void i2cIn(int howMany){
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

void parseCommand(){
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
    jobSwitch=2;
  }else if(infoIn.indexOf("SET_BRIGHT")>=0){
    Serial.println("BRI");
    jobSwitch=3;
  }else if(infoIn.indexOf("SET_SOUND")>=0){
    Serial.println("SPK");
    jobSwitch=4;
  }else if(infoIn.indexOf("PING")>=0){
    Serial.println("PNG");
    if(buzz)alert(300,100,true);
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
  }
  Serial.println("READY");
  jobSwitch=0;
}

void setLedStrip(char x[]){
  int s=atoi(x);
  if(s>0||s<0){
    Serial.println("ERROR->OOB");
  }else{
    Serial.print("STRIP->");
    Serial.println(s);
    selectedStrip=s;
  }
}

void setColor(char x[]){
  int c=atoi(x);
  if(c>2||c<0){
    Serial.println("ERROR->OOB");
    return;
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
  if(b<0||b>255){
    Serial.println("ERROR:OOB");
    return;
  }
  analogWrite(stripPins[selectedStrip][color+1],255-b);
  Serial.print("BRIGHT->");
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

void alert(int pitch,int sLength,boolean useLED){
  if(useLED)digitalWrite(alertLed,HIGH);
  if(buzz)tone(buzzer,pitch,sLength);
  delay(sLength);
  digitalWrite(alertLed,LOW);
}
