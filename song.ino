/**
 * @brief Contains code for the alarms
 * 
 */
#define tonePin D4

void mybeep(int buzzerPin, int pitch, int duration)
{
  tone(buzzerPin, pitch);
  delay(duration);
  noTone(buzzerPin);
}

void playAlarm()
{
  track_alarm();
}
void track_alarm()
{
  mybeep(tonePin, 1000, 50);
  delay(40);
  mybeep(tonePin, 1000, 50);
  delay(300);
}

void track_megalovania() {
  mybeep(tonePin, 294, 120);
  delay(10);
  mybeep(tonePin, 294, 120);
  delay(10);
  mybeep(tonePin, 587, 241);
  delay(10);
  mybeep(tonePin, 440, 362);
  delay(10);
  mybeep(tonePin, 415, 241);
  delay(10);
  mybeep(tonePin, 392, 241);
  delay(10);
  mybeep(tonePin, 349, 241);
  delay(10);
  mybeep(tonePin, 294, 120);
  delay(10);
  mybeep(tonePin, 349, 120);
  delay(10);
  mybeep(tonePin, 392, 120);
  delay(10);
  mybeep(tonePin, 262, 120);
  delay(10);
  mybeep(tonePin, 262, 120);
  delay(10);
  mybeep(tonePin, 587, 241);
  delay(10);
  mybeep(tonePin, 440, 362);
  delay(10);
  mybeep(tonePin, 415, 241);
  delay(10);
  mybeep(tonePin, 392, 241);
  delay(10);
  mybeep(tonePin, 349, 241);
  delay(10);
  mybeep(tonePin, 294, 120);
  delay(10);
  mybeep(tonePin, 349, 120);
  delay(10);
  mybeep(tonePin, 392, 120);
  delay(10);
  mybeep(tonePin, 247, 120);
  delay(10);
  mybeep(tonePin, 247, 120);
  delay(10);
  mybeep(tonePin, 587, 241);
  delay(10);
  mybeep(tonePin, 440, 362);
  delay(10);
  mybeep(tonePin, 415, 241);
  delay(10);
  mybeep(tonePin, 392, 241);
  delay(10);
  mybeep(tonePin, 349, 241);
  delay(10);
  mybeep(tonePin, 294, 120);
  delay(10);
  mybeep(tonePin, 349, 120);
  delay(10);
  mybeep(tonePin, 392, 120);
  delay(10);
  mybeep(tonePin, 233, 120);
  delay(10);
  mybeep(tonePin, 233, 120);
  delay(10);
  mybeep(tonePin, 587, 241);
  delay(10);
  mybeep(tonePin, 440, 362);
  delay(10);
  mybeep(tonePin, 415, 241);
  delay(10);
  mybeep(tonePin, 392, 241);
  delay(10);
  mybeep(tonePin, 349, 241);
  delay(10);
  mybeep(tonePin, 294, 120);
  delay(10);
  mybeep(tonePin, 349, 120);
  delay(10);
  mybeep(tonePin, 392, 120);
  delay(10);
}


void track_mm5() 
{
  mybeep(tonePin, 349, 288);
  mybeep(tonePin, 523, 132);
  mybeep(tonePin, 349, 132);
  delay(132);
  mybeep(tonePin, 466, 432);


  mybeep(tonePin, 622, 432);
  mybeep(tonePin, 587, 432);

  mybeep(tonePin, 466, 288);
  mybeep(tonePin, 523, 288);


  mybeep(tonePin, 349, 132);
  mybeep(tonePin, 466, 132);
  delay(132);

  mybeep(tonePin, 349, 132);
  mybeep(tonePin, 523, 288);


  mybeep(tonePin, 622, 432);
  mybeep(tonePin, 587, 432);

  mybeep(tonePin, 466, 288); 
  mybeep(tonePin, 523, 288);

  mybeep(tonePin, 349, 132);
  mybeep(tonePin, 466, 132);
  delay(132);

  mybeep(tonePin, 349, 132);
  mybeep(tonePin, 523, 288);

  mybeep(tonePin, 784, 1440);
  delay(132);

  mybeep(tonePin, 523, 432);
  mybeep(tonePin, 622, 288);
  mybeep(tonePin, 554, 288);

  mybeep(tonePin, 523, 132);
  mybeep(tonePin, 466, 132);
  delay(132);
  
  mybeep(tonePin, 392, 429);
}
