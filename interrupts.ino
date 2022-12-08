

// ISR - increment button action
void ICACHE_RAM_ATTR  isrIncrement() 
{
  DS3231 rtc;
  int8_t hour, minute, second;
  getRTCTime(&hour, &minute, &second);

  if (digitalRead(BTN_MODE) == LOW) {
    hour += 1;
    if (hour > 12) {
      hour -= 12;
    }
  } else {
    minute += 1;
    minute %= 60;
  }
  setRTCTime(hour,  minute,  second);

  // Update the display
  displayTime12Hour();
  nop();
}


// ISR - decrement button action
void ICACHE_RAM_ATTR  isrDecrement() 
{
  DS3231 rtc;
  int8_t hour, minute, second;
  getRTCTime(&hour, &minute, &second);
  
  if (digitalRead(BTN_MODE) == LOW) {
    hour -= 1;
    hour %= 12;
    if (hour == 0) {
      hour = 12;
    }

    Serial.println(hour);
  } else {
    minute -= 1;
    minute %= 60;
    if (minute < 0) {
      minute = 59;
    }
  }
  setRTCTime(hour,  minute,  second);

  // Update the display
  displayTime12Hour();
  nop();
}


void tick_025() 
{
  // If the portal is active, blink the LED
  if (wifiManager.getConfigPortalActive())
    ledWifiStatus = !ledWifiStatus;
  else {
    if (wifiConnected)
      ledWifiStatus = true;
    else
      ledWifiStatus = false;
  }

  // Handle snooze button
  bool snoozeBtnPressed = !digitalRead(BTN_MODE);
  if (snoozeBtnPressed && alarmShouldPlay) {
    alarmSnoozed = true;
  }
}

// Updates the display every 0.5 seconds
void tick_050() 
{
  uint32_t currentTime = getRTCTime12Hour();
  checkAlarm(currentTime);
  displayTime12Hour();
}


// Checks WiFi status every 3 seconds
void  tick_300() 
{
  wifiConnected = WiFi.status() == WL_CONNECTED; 
}
