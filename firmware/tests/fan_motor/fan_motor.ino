int A = 10;
int B = 11;

void setup() {
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);


  digitalWrite(A, HIGH); // 반시계방향 회전 (Counterclockwise rotation)
  digitalWrite(B, LOW);
  
}

void loop() {
}
