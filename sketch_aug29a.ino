#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

int contador;

double x_offset;
double y_offset;
double z_offset;

double x_acel;
double y_acel;
double z_acel;

double acel_norma;

float temperatura;

double umid;

int valor_lido = 0;

int umidade_p100 = 0;

Adafruit_MPU6050 mpu;

//Threshold variables
double acel_alarm;
double temp_alarm;
double umid_alarm;


// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature temp_sensor(&oneWire);

// Analog pin for soil humidity sensor
const int sensor_umid = A6;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Checkpoint: inicio void setup");

  pinMode(sensor_umid, INPUT);

  temp_sensor.begin();

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // set accelerometer range to +-2G
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);

  // set gyro range to +- 250 deg/s
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);

  delay(100);

  Serial.println("Checkpoint: final void setup");

}

void loop() {
  // put your main code here, to run repeatedly:
  String input = "";

  //TRIGGER CALIBRAçÃO
  if(Serial.available() > 0){
    input = Serial.readStringUntil('\n');
  }
  if(input.equals("calibra")){
    Serial.println("Calibration started");
    Serial.println("..........");
    calibracao();
    input = "";

  }

  //Get Accelerometer Data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  x_acel = a.acceleration.x - x_offset;
  y_acel = a.acceleration.y - y_offset;
  z_acel = a.acceleration.z - z_offset;
  acel_norma = sqrt(pow(x_acel,2)+pow(y_acel,2)+pow(z_acel,2));

  //Get Temperature Data
  temp_sensor.requestTemperatures();
  temperatura = temp_sensor.getTempCByIndex(0);

  //Get Soil Humidity Data
  valor_lido = analogRead(sensor_umid);
  umidade_p100 = map(valor_lido, 600, 250, 0, 100);


  //Print sensor data
  Serial.println("----[ACELEROMETER DATA (m/s^2)]----");
  Serial.print("X = ");
  Serial.println(x_acel);
  Serial.print("Y = ");
  Serial.println(y_acel);
  Serial.print("Z = ");
  Serial.println(z_acel);
  Serial.print("Norma = ");
  Serial.println(acel_norma);

  Serial.println("");

  Serial.println("----[TEMPERATURE DATA]----");
  Serial.print("T(oC): ");
  Serial.println(temp_sensor.getTempCByIndex(0));

  Serial.println("");

  Serial.println("----[HUMIDITY DATA]----");
  Serial.print("Humidity (%): ");
  Serial.println(umidade_p100);

  Serial.println("");

   alarme();

  delay(500);

}


void calibracao(){
  x_offset = 0;
  y_offset = 0;
  z_offset = 0;
  for(int i = 0; i < 200; i++){
    //take 200 readings on each axis
    sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

    x_offset += a.acceleration.x;
    y_offset += a.acceleration.y;
    z_offset += a.acceleration.z;
  }

  x_offset = x_offset/200;
  y_offset = y_offset/200;
  z_offset = z_offset/200;

  Serial.println("Sensor has been calibrated:");
  Serial.print("Offsets changed to (x y z): ");
  Serial.print(x_offset);
  Serial.print("  ");
  Serial.print(y_offset);
  Serial.print("  ");
  Serial.println(z_offset);
}


void alarme(){
  if(acel_norma >= 3){
    Serial.println("Danger! Landslide imminent.");
  }
  else if(acel_norma >= 2){
    if(umid >= 70){
       Serial.println("Danger! Landslide imminent.");
    }
    else if(umid >= 40){
      if(temperatura > 35){
        Serial.println("Danger! Landslide imminent.");
      } 
      else{
        Serial.println("Danger! Landslide imminent.");
      }
    }
    else{
      if(temperatura > 35){
        Serial.println("Caution! Risk of landslide.");
      }
      else{
        Serial.println("Caution! Risk of landslide.");
      }
    }
  }
  
  else if(acel_norma >= 1){
    if(umid >= 70){
       Serial.println("Danger! Landslide imminent.");
    }
    else if(umid >= 40){
      if(temperatura > 35){
        Serial.println("Danger! Landslide imminent.");
      } 
      else{
        Serial.println("Caution! Risk of landslide.");
      }
    }
    else{
      if(temperatura > 35){
        Serial.println("Caution! Risk of landslide.");
      }
      else{
        Serial.println("No risk of landslide.");
      }
    }
  }
  
  else{
    if(umid >= 70){
      if(temperatura > 30){
          Serial.println("Caution! Risk of landslide.");
      } 
      else{
        Serial.println("Caution! Risk of landslide.");
      }
    }
    else if(umid >= 40){
      if(temperatura > 30){
        Serial.println("Caution! Risk of landslide.");
      } 
      else{
        Serial.println("No risk of landslide.");
      }
    }
    else{
      if(temperatura > 30){
        Serial.println("No risk of landslide.");
      }
      else{
        Serial.println("No risk of landslide.");
      }
    }
  }
}
