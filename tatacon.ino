#include <Keyboard.h>

#define THRESHOLD 12

#define BUFFER_SIZE 10

class Buffer {
  private:
    int data[BUFFER_SIZE];
    int index;
    float avg;
  public:
    Buffer();
    float update(int next);
};
Buffer::Buffer() {}
float Buffer::update(int next) {
  index = (index + 1) % BUFFER_SIZE;
  avg += (float) (next - data[index]) / (float) BUFFER_SIZE;
  data[index] = next;
  return avg;
}


static int pins[] = {A0, A1, A2, A3};
static char keys[] = {'d', 'f', 'j', 'k'};
int last_read[] = {0, 0, 0, 0};
float last_avg[] = {0, 0, 0, 0};
float current_hit_power = 0;
Buffer buffers[4];

unsigned long int frame_time;


void setup() {
  Serial.begin(9600);
  Keyboard.begin();
}

void loop() {
  unsigned long int ft = micros();
  unsigned int dt = ft - frame_time;
  frame_time = ft;
  if (dt < 1000) {
    delayMicroseconds(1000 - dt);
  }

  bool hit = false;
  for (short i = 0; i < 4; i++) {
    int reading = analogRead(pins[i]);
    last_avg[i] = buffers[i].update(abs(reading - last_read[i]));
    last_read[i] = reading;
    
    switch (i) {
      case 1:
        last_avg[i] *= 1.4;
        break;
      case 2:
        last_avg[i] *= 1.8;
        break;
    }

    if (last_avg[i] > THRESHOLD) {
      if (last_avg[i] > current_hit_power) {
        current_hit_power = last_avg[i];
      }
      hit = true;
    }
  }
  
  if (hit) {
    for (short i = 0; i < 4; i++) {
      if (last_avg[i] > current_hit_power * 0.8) {
        Keyboard.press(keys[i]);
      } else {
        Keyboard.release(keys[i]);
      }
    }
  } else {
    current_hit_power = 0;
    Keyboard.releaseAll();
  }
}
