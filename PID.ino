
#include<Wire.h>
#include<Servo.h>

Servo m1;
Servo m2;
int sig_mi1=1000;
int sig_mi2=1000;
int dormant=1000;
int on_off_switch=0;
int pin=13;
int flag=0;
float Kp=3,Ki=1,Kd=1;
double dt,error,errSum,lastErr;
double setpoint=0;



const int MPU_addr=0x68;
double AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; 
uint32_t timer;
double compAngleX, compAngleY; 
#define degconvert 57.2957786 
void setup(){
  flag=0;
  pinMode(pin,INPUT);
  m1.attach(2);
  m2.attach(4);
  m1.writeMicroseconds(1000);
  m2.writeMicroseconds(1000);

  Wire.begin();
  #if ARDUINO >= 157
  Wire.setClock(400000UL);
  #else
    TWBR = ((F_CPU / 400000UL) - 16) / 2;
  #endif
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);
  Serial.begin(115200);
  delay(100);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();       
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read();  
  Tmp=Wire.read()<<8|Wire.read(); 
  GyX=Wire.read()<<8|Wire.read();  
  GyY=Wire.read()<<8|Wire.read(); 
  GyZ=Wire.read()<<8|Wire.read(); 

  double roll = atan2(AcY, AcZ)*degconvert;
  double pitch = atan2(-AcX, AcZ)*degconvert;

  double gyroXangle = roll;
  double gyroYangle = pitch;
  double compAngleX = roll;
  double compAngleY = pitch;
  timer = micros();
}
void loop(){
 
  error= setpoint-compAngleX;    
  errSum=errSum+(error*dt);              //jagah badal sakti hai
  double dErr= (error-lastErr)/dt; 
 
  sig_mi1=1100-Kp*error-Kd*dErr-Ki*errSum;
  sig_mi2=1100+Kp*error+Kd*dErr+Ki*errSum;
  
  if(digitalRead(pin)==1){
   m1.writeMicroseconds(sig_mi1);
   m2.writeMicroseconds(sig_mi2);
   }
   else{
   m1.writeMicroseconds(1000);
   m2.writeMicroseconds(1000);
   }
lastErr=error;
  
  //
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true); 
  //ch11=pulseIn(ch1,HIGH,50000);
  //ch22=pulseIn(ch2,HIGH,50000);
  AcX=Wire.read()<<8|Wire.read();  
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read();  
  Tmp=Wire.read()<<8|Wire.read();  
  GyX=Wire.read()<<8|Wire.read();  
  GyY=Wire.read()<<8|Wire.read();  
  GyZ=Wire.read()<<8|Wire.read();  
  
  dt = (double)(micros() - timer) / 1000000; 
  timer = micros();

  
  double roll = atan2(AcY, AcZ)*degconvert;
 // double pitch = atan2(-AcX, AcZ)*degconvert;

  double gyroXrate = GyX/131.0;
 // double gyroYrate = GyY/131.0;

  compAngleX = 0.99 * (compAngleX + gyroXrate * dt) + 0.01 * roll; 
//  compAngleY = 0.99 * (compAngleY + gyroYrate * dt) + 0.01 * pitch; 
 


  

 Serial.print(compAngleX);
 Serial.print('\n');
 delayMicroseconds(10);
 
// Serial.print("                ");
// Serial.println(digitalRead(pin));
//  Serial.print("                ");
//  Serial.print(ch22);
 // Serial.println("       ");
}

