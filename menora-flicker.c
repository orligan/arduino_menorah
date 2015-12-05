#define CANDLES 9

int led_pin[CANDLES] = {9,1,2,3,4,5,6,7,8};
int switch_pin = 12;

// Random ranges
int low_min = 130; 
int low_max = 240;

int high_min = 230;
int high_max = 256;

int hold_min = 40;
int hold_max = 80;

int pause_min = 100;
int pause_max = 400; 

// Timers
#define TICK 1

int hold[CANDLES];
int pause[CANDLES];
int t1[CANDLES];
int t2[CANDLES];
int t3[CANDLES];
int t4[CANDLES];
int ts[CANDLES];
int current_time = 0; 

// Volumes
int low_start[CANDLES];
int low_end[CANDLES];
int high[CANDLES];
int volume[CANDLES];

// Switch state
int menorah_day = 1;
int button_state = HIGH;
int last_button_state = LOW;
int last_debounce_time = 0;
int debounce_delay = 50;


void cycle_init(int i) 
{
    if (i >= CANDLES)
      return;
      
    ts[i] = millis();

    low_start[i] = random(low_min, low_max);
    high[i] = random(high_min, high_max);
    low_end[i] = random(low_min, low_max);

    hold[i] = random(hold_min, hold_max);
    pause[i] = random(pause_min, pause_max);

    t1[i] = TICK * (high[i] - low_start[i]);
    t2[i] = t1[i] + hold[i];
    t3[i] = t2[i] + (high[i] - low_end[i]);
    t4[i] = t3[i] + TICK * pause[i];
    
    volume[i] = low_start[i];
}


void switch_read(int current_time) 
{
  int reading = digitalRead(switch_pin);  

  if (reading != last_button_state) {
    last_debounce_time = current_time;
  }
  if ((current_time - last_debounce_time) > debounce_delay) {
    if (reading != button_state) {
      button_state = reading;      
      if (button_state == LOW) {
        menorah_day++;
        if (menorah_day == 9) {
          menorah_day = 0;
          for(int i=0; i<CANDLES; i++) {
            digitalWrite(led_pin[i],LOW);
          }
        }   
      }
    }
  }  
  last_button_state = reading;
}

void setup() 
{
    for(int i=0; i<CANDLES; i++)
    {
      cycle_init(i);
      pinMode(led_pin[i], OUTPUT);
    }
    pinMode(switch_pin, INPUT);
    digitalWrite(switch_pin, HIGH);
}


void loop()
{
    int current_time = millis();

    switch_read(current_time);

    if (menorah_day != 0) {
      for(int i = 0; i <= menorah_day; i++) {
        int t = current_time - ts[i];
      
        if(t < t1[i]) {
          volume[i] = min(volume[i]+1, 256);
        }
        else if(t < t2[i]) {
        }
        else if(t < t3[i]) {
          volume[i] = max(volume[i]-1,0);
        }
        else if(t < t4[i]) {
        }
        else {
          cycle_init(i);
        }
        analogWrite(led_pin[i], volume[i]);
      }
      delay(TICK);
    }
}
