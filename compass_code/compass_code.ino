#include <LiquidCrystal.h>
#include <QMC5883LCompass.h>
#include <Servo.h>

//Crear el objeto LCD con los números correspondientes (rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(8, 3, 4, 5, 6, 7);
#define potenciometro A1
Servo motor;
QMC5883LCompass compass;
float declinacion = -4.48;
int anguloServo = 90;
int contador = 0;
bool error = false;

void setup() {
  Serial.begin(9600);
  compass.init();
  // Inicializar el LCD con el número de  columnas y filas del LCD
  lcd.begin(16,3);
  lcd.print("Barco V1.0");
  compass.setCalibrationOffsets(-158.00, -129.00, 396.00);
  compass.setCalibrationScales(1.29, 0.91, 0.89);
  delay(5000);  
  motor.attach(10);
  motor.write(0);
  delay(1000);
  motor.write(90);
  delay(1000);
}

void loop() {
  int x, y, z;
  // Read compass values
  compass.read();

  // Return XYZ readings
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  //Determinacion norte magnetico
  float northMag = -((atan2(-x,-y) * 180)/ M_PI); 
  int nortGeo = int(northMag + declinacion);
  if(nortGeo < 0)
  nortGeo +=360;
  Serial.print("Brujula: ");
  Serial.println(int(nortGeo)); 
  int input = analogRead(potenciometro);
  input = map(input, 0, 1000, 0, 359);
  if(input > 359){
    input = 359;
  }
  if(contador%100 == 0){
    lcd.clear();
    lcd.print("Bru:");
    lcd.print(nortGeo);
    lcd.print("        ");
    lcd.setCursor(0,1);
    lcd.print("Ent:");
    lcd.print(input);
    if(error){
      lcd.setCursor(10, 0);
      lcd.print("E");
    }
  }
  int distancia1 = distanciaHoraria(nortGeo,input);
  int distancia2 = 360 - distancia1;
  bool giroHorario = (distancia1 <= distancia2)? true:false;
  if(giroHorario){
    anguloServo-=1;
    if(anguloServo < 0){
      error = true;
      anguloServo+=1;
    }else if(nortGeo == input){
      error = false;
      anguloServo+=1;
    }else{
      error = false;
      motor.write(anguloServo);
    }
    
  }else{
    anguloServo+=1;
    if(anguloServo >180 ){
      error = true;
      anguloServo-=1;
    }else if(nortGeo == input){
      error = false;
      anguloServo-=1;
      
    }else{
      error = false;
      motor.write(anguloServo);
    }
  }
  delay(100);
  contador+=10;


}
int distanciaHoraria(int sensor, int input){
  int indice = 0;
  while(indice < 360){
    int info = indice + sensor;
    if(info >=360)
      info-=360;
    if(info == input)
      break;
    indice +=1;
  }
  return indice;
   
    
}
