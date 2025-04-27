#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

#define TRIG_PIN A4
#define ECHO_PIN A3
#define MAX_DISTANCE 200
#define MAX_SPEED 150

const int buzzer = A5;

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
Servo myservo;

boolean goesForward = false;
int distance = 100;
int speedSet = 0;
unsigned long lastTime = 0;
unsigned long buzzerBlinkTime = 0;
unsigned long buzzerInterval = 500;

void setup() {
  Serial.begin(9600); // Start Serial Monitor
  myservo.attach(10);
  myservo.write(115);  // Posisi servo normal
  delay(2000);
  distance = readPing(); // Ambil data awal jarak
  
  pinMode(buzzer, OUTPUT);
}

void loop() {
  unsigned long currentTime = millis();

  // Cek apakah sudah waktunya membaca sensor lagi
  if (currentTime - lastTime > 50) {
    distance = readPing();
    lastTime = currentTime; // Update waktu terakhir sensor dibaca
  }

  // Menampilkan pembacaan sensor ke Serial Monitor
  Serial.print("Distance: ");
  if (distance == 250) {
    Serial.println("Out of range");
  } else {
    Serial.println(distance); // Tampilkan jarak yang valid
  }

  // Jika jarak lebih dari 15 cm, motor berjalan terus
  if (distance > 15) {
    moveForward();
    digitalWrite(buzzer, LOW);  // Matikan buzzer jika jarak aman
  }
  // Jika jarak kurang dari 15 cm, mundur dan belok
  else {
    // Menyala dan matikan buzzer setiap interval (kedipan)
    if (currentTime - buzzerBlinkTime >= buzzerInterval) {
      buzzerBlinkTime = currentTime; // Update waktu kedipan berikutnya
      digitalWrite(buzzer, !digitalRead(buzzer));  // Toggle buzzer (blink)
    }

    moveStop();
    delay(100);  // Berhenti sejenak
    moveBackward();
    delay(300);  // Mundur sejenak
    moveStop();
    delay(200);  // Berhenti sejenak
    lookAround();  // Cek kanan dan kiri
    myservo.write(115);  // Kembalikan posisi servo ke default
  }
}

int readPing() {
  int cm = sonar.ping_cm();
  if (cm == 0) {
    cm = 250; // Jika tidak ada objek terdeteksi, kita set ke 250 untuk out of range
  }
  return cm;
}

void moveStop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
}

void moveForward() {
  if (!goesForward) {
    goesForward = true;
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 10) {
      motor1.setSpeed(speedSet);
      motor2.setSpeed(speedSet);
      delay(5);  // Kecepatan motor naik bertahap
    }
  }
}

void moveBackward() {
  goesForward = false;
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 10) {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet);
    delay(5);
  }
}

void lookAround() {
  int distanceR = 0;
  int distanceL = 0;

  // Menggerakkan servo untuk melihat kanan dan kiri
  myservo.write(50);  // Posisi servo kanan
  delay(500);
  distanceR = readPing();
  Serial.print("Look Right: ");
  Serial.println(distanceR);
  myservo.write(170);  // Posisi servo kiri
  delay(500);
  distanceL = readPing();
  Serial.print("Look Left: ");
  Serial.println(distanceL);

  // Tentukan belokan berdasarkan jarak kanan dan kiri
  if (distanceR >= distanceL) {
    turnRight();
  } else {
    turnLeft();
  }
}

void turnRight() {
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  delay(500);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
}

void turnLeft() {
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  delay(500);
  motor1.run(FORWARD);
  motor2.run(FORWARD);
}
