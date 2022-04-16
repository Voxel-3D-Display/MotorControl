#include "Dial.h"
#include "ClockTimer.h"
#include "LCDState.h"
#include "MotorDriver.h"
#include <LiquidCrystal.h>

#define BUTTON_PIN 8
#define LED_PIN 9

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LCDState lcd_state(&lcd);

const int m_pwm = 10, m_dir = 11, encoder = 12;
MotorDriver motor_driver;

Dial dial;
uint8_t dial_delay = 0;
bool button_pressed = 0;

ClockTimerf print_timer(1);
ClockTimerf slow_led(1);
ClockTimerf fast_led(10);
ClockTimerf dial_timer(6);
ClockTimerf button_timer(30);

void setup() {
  Serial.begin(9600);
  dial.begin(A0);
  lcd.begin(16, 2);
  motor_driver.begin(m_pwm, m_dir);

  // Change PWM frequency on pin 10 to 31kHz
  TCCR1B = TCCR1B & B11111000 | B00000001;

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  delay(100);
}

void loop() {
  if (print_timer.ready()){
    if (lcd_state.is_program_start()){
      // 2 MSB is program mode, 6 LSB is program number
      byte start_program = lcd_state.get_program_mode() << 6 || lcd_state.get_program();
      // Send program over serial
      Serial.write(start_program);
    }
//    Serial.println(dial.get_value()); // For calibrating pot
  }
  if (dial_timer.ready()){
    int8_t dial_state = dial.get_state();
    if (abs(dial_state) == 2 || dial_delay >= 4){
      lcd_state.update_selector(dial_state);
      dial_delay = 0;
    } else {
      dial_delay += 1;
    }
    lcd_state.update_display();
  }
  if (fast_led.ready() && lcd_state.is_started()){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    motor_driver.set_power(lcd_state.get_power());
  }
  if (slow_led.ready() && !lcd_state.is_started()){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    motor_driver.stop();
  }
  if (button_timer.ready()){
    // If read logic low (button pressed) and button hasn't been pressed
    if(!digitalRead(BUTTON_PIN) && !button_pressed){
      button_pressed = 1;
      lcd_state.button_press();
    }
    if(digitalRead(BUTTON_PIN)) button_pressed = 0;
  }

}
