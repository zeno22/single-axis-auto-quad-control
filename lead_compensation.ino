
#include<Wire.h>
#include<Servo.h>

Servo m1;
Servo m2;
int sig_mi1=1100;
int sig_mi2=1100;
int dormant=1000;
int on_off_switch=0;
int pin=13;
int flag=0;
double Kp=3,Ki=1,Kd=1;
double dt,errSum,lastErr,a,b,c;
double error[6];
double setpoint=0;



const int MPU_addr=0x68;
double AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ; 
uint32_t timer;
uint32_t timer1;
uint32_t dt1;
double compAngleX, compAngleY; 
#define degconvert 57.2957786 
void setup(){
  error[0]=0;//cycle space1
  error[1]=0;//cycle space 2
  error[2]=0; //3rd error e(k-2)
  error[3]=0;//2nd error e(k-1)
  error[4]=0;//1st error  e(k)
  error[5]=0;
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
 dt = (double)(micros() - timer) / 1000000; 
 for(int i=2;i>=0;i++){
  error[i]= setpoint-compAngleX;  
  //errSum=errSum+((error+lastErr)*dt/2);              //jagah badal sakti hai
 // double dErr= (error-lastErr)/dt; 
 
  sig_mi1=sig_mi1-a*error[i]-b*error[i-1]-c*error[i-2];
  sig_mi2=sig_mi2+a*error[i]+b*error[i-1]+c*error[i-2];
  
  if(digitalRead(pin)==1){
   m1.writeMicroseconds(sig_mi1);
   m2.writeMicroseconds(sig_mi2);
   }
   else{
   m1.writeMicroseconds(1000);
   m2.writeMicroseconds(1000);
   }
  
  //
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
  
   dt1 = (double)(micros() - timer1) / 1000000; 
  timer1 = micros();
  
  
  double roll = atan2(AcY, AcZ)*degconvert;
 // double pitch = atan2(-AcX, AcZ)*degconvert;

  double gyroXrate = GyX/131.0;
 // double gyroYrate = GyY/131.0;

  compAngleX = 0.99 * (compAngleX + gyroXrate * dt1) + 0.01 * roll; 
//  compAngleY = 0.99 * (compAngleY + gyroYrate * dt) + 0.01 * pitch; 
 

if(i>=4){
  i=2;
  error[1]=error[4];
  error[0]=error[3];
}
  

 Serial.print(compAngleX);
 Serial.print('\n');
 delayMicroseconds(10);
 
 }
a=Kp+Ki*dt/2+Kd/dt;
b=-Kp+Ki*dt/2-2*Kd/dt;
c=Kd/dt;
timer = micros();
 
 
// Serial.print("                ");
// Serial.println(digitalRead(pin));
//  Serial.print("                ");
//  Serial.print(ch22);
 // Serial.println("       ");
}

