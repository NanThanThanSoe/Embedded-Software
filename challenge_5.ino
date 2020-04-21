void setup() {
  // set pin 11 as output
  pinMode(11, OUTPUT); //servo 1

  TCCR1A = 0; // clear content of the timer control register A
  TCCR1B = 0; // clear content of the timer control register B
  TCNT1 = 0;

  TCCR1A |= (1 << COM1A1);  // clear OC1A on match, set on bottom
  TCCR1A |= (1 << WGM11); // set timer to mode 14 (fast PWM)
  TCCR1B |= (1 << WGM12) | (1 << WGM13);
  TCCR1B |= (1 << CS10) | (1 << CS12); // use prescaler = 1024

  ICR1 = 312; // preload the top value to get 1 Hz frequency
  OCR1A = 8; // preload the compare value to get 50% duty cycle

  //set pin 5 as output
  pinMode(5, OUTPUT); //servo 2

  TCCR3A = 0; // clear content of the timer control register A
  TCCR3B = 0; // clear content of the timer control register B
  TCNT3 = 0;

  TCCR3A |= (1 << COM3A1);  // clear OC1A on match, set on bottom
  TCCR3A |= (1 << WGM31); // set timer to mode 14 (fast PWM)
  TCCR3B |= (1 << WGM32) | (1 << WGM33);
  TCCR3B |= (1 << CS30) | (1 << CS32); // use prescaler = 1024

  ICR3 = 312; // preload the top value to get 1 Hz frequency
  OCR3A = 8; // preload the compare value to get 50% duty cycle

  //ADC initialization 
  ADMUX = 0;  // Clear ADMUX register
  // Choose reference voltage
  ADMUX = (1<<REFS0); // Use VCC as reference voltage
  // Set ADC prescaler
  ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);  // Use 128 as prescaler. This results in the sampling frequency of ca. 125 KHz 
  // Select an input pin, i.e. ADC0
  ADMUX |= 0;
  //Initialize serial communication 
  Serial.begin(9600);
  
}

void loop() {
  int clockwise;
  int counterclockwise;
  ADCSRA |= (1<<ADSC);  

  Serial.println(ADC);
  clockwise = map(ADC, 0, 1023, 8, 37);
 
  OCR1A=clockwise;
  counterclockwise=37-clockwise+8;
  OCR3A=counterclockwise;
}
