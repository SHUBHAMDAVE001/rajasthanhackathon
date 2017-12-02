  const int valvePin = 8;
  bool isprocessing = false;
  int outputWater = 0;
  String inString = "";
  int litre = 0;
  unsigned long milil = 0;
  byte sensorInterrupt = 0;  // 0 = digital pin 2
  byte sensorPin       = 2;
  float calibrationFactor = 4.5;
  volatile byte pulseCount;
  float flowRate;
  unsigned int flowMilliLitres;
  unsigned long totalMilliLitres;
  
  unsigned long oldTime;
  
  void setup() {
    pinMode(valvePin, OUTPUT);
    pulseCount        = 0;
    flowRate          = 0.0;
    flowMilliLitres   = 0;
    totalMilliLitres  = 0;
    oldTime           = 0;
    pinMode(sensorPin, INPUT);
    digitalWrite(sensorPin, HIGH);
  
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(38400);
     while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  }
  
  void loop() {
    // put your main code here, to run repeatedly:
    if (!isprocessing){
      if (Serial.available() > 0) {
        int inChar = Serial.read();
      
        if (isDigit(inChar)) {
          // convert the incoming byte to a char and add it to the string:
          inString += (char)inChar;
          }
        // if you get a newline, print the string, then the string's value:
        if (inChar == '\n') {
          // Serial.print("Value:");
          Serial.println(inString);
          litre = inString.toInt();
          // Serial.print("String: ");
          // Serial.println(inString);
          // clear the string for new input:
          inString = "";
          }
        }
      if(litre>0){
        // Serial.println(litre); 
        // open valve
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(2000);
  //      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW     
        milil = litre * 1000;
        isprocessing = true;
        attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
        litre = 0; 
        }  
      }
  
      if (isprocessing){
        if((millis() - oldTime) > 1000){ 
          // Disable the interrupt while calculating flow rate and sending 
          detachInterrupt(sensorInterrupt);
          flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
          oldTime = millis();
          flowMilliLitres = (flowRate / 60) * 1000;
          totalMilliLitres += flowMilliLitres;
          if (totalMilliLitres > milil){
            isprocessing = false;
  //          milil = 1;
            litre = 0;
            totalMilliLitres = 0;
            digitalWrite(LED_BUILTIN, LOW);
            }
          unsigned int frac;
           Serial.print("Flow rate: ");
      Serial.print(int(flowRate));  // Print the integer part of the variable
      Serial.print(".");             // Print the decimal point
      // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
      frac = (flowRate - int(flowRate)) * 10;
      Serial.print(frac, DEC) ;      // Print the fractional part of the variable
      Serial.print("L/min");
      // Print the number of litres flowed in this second
      Serial.print("  Current Liquid Flowing: ");             // Output separator
      Serial.print(flowMilliLitres);
      Serial.print("mL/Sec");
  
      // Print the cumulative total of litres flowed since starting
      Serial.print("  Output Liquid Quantity: ");             // Output separator
      Serial.print(totalMilliLitres);
      Serial.println("mL"); 
          pulseCount = 0;
          attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
          }
      }
  }
  
  void pulseCounter()
  {
    // Increment the pulse counter
    pulseCount++;
  }
  
  
  //digitalWrite(valvePin, HIGH);
