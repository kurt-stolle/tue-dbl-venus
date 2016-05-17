void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int A0 = analogRead(0);
  int A1 = analogRead(1);
  Serial.println("A0:");
  Serial.println(A0);
  Serial.println("A1:");
  Serial.println(A1);
  delay(500);
}
