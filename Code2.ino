#include <EEPROM.h>

// ===== إعدادات الأجهزة =====
// بنات التمكين والتحكم للمحركات
#define R_EN1 A0
#define L_EN1 A1
#define RPWM1 5
#define LPWM1 6
#define R_EN2 A2
#define L_EN2 A3
#define RPWM2 9
#define LPWM2 10

// حساسات الخصم (JS40F Digital)
#define SENSOR_FRONT 2
#define SENSOR_RIGHT 3
#define SENSOR_LEFT  4

// حساسات الخط (Digital)
#define LINE_FRONT_RIGHT 8
#define LINE_FRONT_LEFT  7
#define LINE_BACK_RIGHT  12
#define LINE_BACK_LEFT   11

// ===== إعدادات الروبوت =====
const float LEARNING_RATE = 0.01;
const float EPSILON_MIN = 0.05;
const float EDGE_THRESHOLD = 0.8;
const unsigned long AVOID_TIMINGS[] = {50, 200, 300};
const unsigned long START_DELAY = 5000;

// ===== متغيرات الذكاء الاصطناعي =====
// تقليل الأوزان لتوفير الذاكرة
float weights[12];        // 4 حساسات رئيسية × 3 اتجاهات
float sensorsValues[5];   // تقليل عدد الحساسات
float epsilon = 0.2;
int wins = 0, battles = 0;

// ===== حالة البداية =====
unsigned long startTime;
bool started = false;

// ===== حالة تجنب الحافة =====
struct EdgeAvoid {
  bool active = false;
  int step = 0;
  unsigned long start = 0;
} edgeAvoid;

// ===================== الوظائف =====================

// ضبط المحركات
void setMotors(int left, int right) {
  // المحرك الأول (يسار)
  if (left > 0) {
    analogWrite(RPWM1, left);
    analogWrite(LPWM1, 0);
  } else if (left < 0) {
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, abs(left));
  } else {
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, 0);
  }

  // المحرك الثاني (يمين)
  if (right > 0) {
    analogWrite(RPWM2, right);
    analogWrite(LPWM2, 0);
  } else if (right < 0) {
    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, abs(right));
  } else {
    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, 0);
  }
}

void stopMotors() { 
  setMotors(0, 0); 
}

// قراءة الحساسات
void readSensors() {
  // حساسات الخصم (JS40F)
  sensorsValues[0] = digitalRead(SENSOR_FRONT);
  sensorsValues[1] = digitalRead(SENSOR_RIGHT);
  sensorsValues[2] = digitalRead(SENSOR_LEFT);
  
  // حساسات الخط (Digital - أي حساس يكتشف الخط)
  int frontEdge = digitalRead(LINE_FRONT_RIGHT) || digitalRead(LINE_FRONT_LEFT);
  int backEdge = digitalRead(LINE_BACK_RIGHT) || digitalRead(LINE_BACK_LEFT);
  
  sensorsValues[3] = frontEdge;
  sensorsValues[4] = backEdge;
}

// اتخاذ القرار باستخدام التعلم المعزز (مبسط)
int aiDecision() {
  if (random(1000) < epsilon * 1000) return random(0, 5);

  float scores[5] = {0};
  for (int action = 0; action < 5; action++) {
    for (int i = 0; i < 4; i++) {
      scores[action] += sensorsValues[i] * weights[i*3 + (action%3)];
    }
  }

  int best = 0;
  for (int i = 1; i < 5; i++) 
    if (scores[i] > scores[best]) best = i;
  return best;
}

// تنفيذ الحركة
void executeAction(int action) {
  switch(action) {
    case 0: stopMotors(); break;
    case 1: setMotors(255, 255); break;      // أمام
    case 2: setMotors(150, 255); break;      // يمين
    case 3: setMotors(255, 150); break;      // يسار
    case 4: setMotors(-200, -200); break;    // خلف
  }
}

// حساب المكافأة
float getReward(int action) {
  float reward = sensorsValues[0] * 10; // الأمامي
  
  // مكافأة على الدوران نحو العدو
  if ((action == 2 || action == 3) && sensorsValues[0] > 0.5) 
    reward += 15;

  // عقوبة على الاقتراب من الحافة
  if (sensorsValues[3] > 0.5 || sensorsValues[4] > 0.5) 
    reward -= 25;

  return reward;
}

// التعلم
void learn(int action, float reward) {
  for (int i = 0; i < 4; i++) {
    weights[i*3 + (action%3)] += LEARNING_RATE * reward * sensorsValues[i];
    weights[i*3 + (action%3)] = constrain(weights[i*3 + (action%3)], -2.0, 2.0);
  }

  epsilon *= 0.9995;
  if (epsilon < EPSILON_MIN) epsilon = EPSILON_MIN;

  static int saveCounter = 0;
  if (++saveCounter > 200) { 
    saveAI(); 
    saveCounter = 0; 
  }
}

// ===== تجنب الحافة =====
bool checkEdge() {
  return (sensorsValues[3] > 0.5 || sensorsValues[4] > 0.5);
}

void startAvoidEdge() {
  edgeAvoid.active = true;
  edgeAvoid.step = 0;
  edgeAvoid.start = millis();
  stopMotors();
}

void handleAvoidEdge() {
  unsigned long now = millis();
  switch(edgeAvoid.step) {
    case 0: // توقف
      if (now - edgeAvoid.start >= AVOID_TIMINGS[0]) { 
        setMotors(-255,-255); 
        edgeAvoid.step=1; 
        edgeAvoid.start=now; 
      }
      break;
    case 1: // رجوع للخلف
      if (now - edgeAvoid.start >= AVOID_TIMINGS[1]) { 
        setMotors(-150,150); 
        edgeAvoid.step=2; 
        edgeAvoid.start=now; 
      }
      break;
    case 2: // دوران
      if (now - edgeAvoid.start >= AVOID_TIMINGS[2]) { 
        stopMotors(); 
        edgeAvoid.active=false; 
      }
      break;
  }
}

// ===== حفظ واسترجاع الذكاء =====
void saveAI() {
  int addr = 0;
  for (int i = 0; i < 12; i++) {
    EEPROM.put(addr, weights[i]);
    addr += sizeof(float);
  }
  EEPROM.put(addr, epsilon);
  addr += sizeof(float);
  EEPROM.put(addr, wins);
  addr += sizeof(int);
  EEPROM.put(addr, battles);
}

void loadAI() {
  int addr = 0;
  for (int i = 0; i < 12; i++) {
    EEPROM.get(addr, weights[i]);
    if (isnan(weights[i])) weights[i] = random(-100,100)/100.0;
    addr += sizeof(float);
  }
  EEPROM.get(addr, epsilon);
  if (isnan(epsilon) || epsilon > 1.0) epsilon = 0.2;
  addr += sizeof(float);
  EEPROM.get(addr, wins);
  addr += sizeof(int);
  EEPROM.get(addr, battles);
}

// ===== Setup و Loop =====
void setup() {
  // تفعيل بنات التمكين للمحركات
  pinMode(R_EN1, OUTPUT);
  pinMode(L_EN1, OUTPUT);
  pinMode(R_EN2, OUTPUT);
  pinMode(L_EN2, OUTPUT);
  digitalWrite(R_EN1, HIGH);
  digitalWrite(L_EN1, HIGH);
  digitalWrite(R_EN2, HIGH);
  digitalWrite(L_EN2, HIGH);
  
  // إعداد PWM للمحركات
  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(RPWM2, OUTPUT);
  pinMode(LPWM2, OUTPUT);
  
  // إعداد حساسات الخصم
  pinMode(SENSOR_FRONT, INPUT);
  pinMode(SENSOR_RIGHT, INPUT);
  pinMode(SENSOR_LEFT, INPUT);
  
  // إعداد حساسات الخط
  pinMode(LINE_FRONT_RIGHT, INPUT);
  pinMode(LINE_FRONT_LEFT, INPUT);
  pinMode(LINE_BACK_RIGHT, INPUT);
  pinMode(LINE_BACK_LEFT, INPUT);
  
  loadAI();
  startTime = millis();
  
  randomSeed(analogRead(A4)); // للعشوائية
}

void loop() {
  readSensors();

  // تأخير البداية
  if (!started && millis() - startTime < START_DELAY) {
    stopMotors();
    return;
  }
  started = true;

  // التعامل مع الحافة
  if (edgeAvoid.active) { 
    handleAvoidEdge(); 
    return; 
  }
  if (checkEdge()) { 
    startAvoidEdge(); 
    return; 
  }

  // القرار والتنفيذ
  int action = aiDecision();
  executeAction(action);
  float reward = getReward(action);
  learn(action, reward);
}
