int ledPin = 7;

void setup() 
{
  pinMode(ledPin, OUTPUT);
}

void loop() 
{
  digitalWrite(ledPin,0);
  delay(1000);

  for (int i = 0; i < 5; i++) 
  {
    digitalWrite(ledPin, HIGH);  
    delay(100);                  
    digitalWrite(ledPin, LOW);  
    delay(100);                  
  }
  
  while (true) 
  {
    digitalWrite(ledPin, 1);
  }
}
