const int pumpRelayPin = 5;
const int soilSensorPin = 1;
const int flowSensorPin = 2;
byte sensorInterrupt = 0;
const float calibrationFactor = 4.5;
volatile byte pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

void setup()
{
  pinMode(pumpRelayPin, OUTPUT);
  pinMode(soilSensorPin, INPUT);
  pinMode(flowSensorPin, INPUT);
  Serial.begin(38400);
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  oldTime = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void loop()
{
  int soilSensorValue = analogRead(soilSensorPin);
  soilSensorValue = constrain(soilSensorValue, 485, 1023);
  int soilMoisture = map(soilSensorValue, 485, 1023, 100, 0);
  Serial.println(soilMoisture);
  checkIfWaterIsNeeded(soilMoisture);
}

void checkIfWaterIsNeeded(int soil)
{
  if(soil <= 40){
    digitalWrite(pumpRelayPin, HIGH);
    delay(3000);
    if(waterIsRunning == false)
      digitalWrite(pumpRelayPin, LOW);
  }
  else
    digitalWrite(pumpRelayPin, LOW);
}

bool waterIsRunning() {
  if((millis() - oldTime) > 1000)
  {
    detachInterrupt(sensorInterrupt);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    unsigned int frac;
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));
    Serial.print(".");
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC);
    Serial.print("L/min");
        Serial.print("  Current Liquid Flowing: ");
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    // Print the cumulative total of litres flowed since starting
    Serial.print("  Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.println("mL");
    
    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
    if(flowMilliLitres > 0)
      return true;
    return false;
  }
}
/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
