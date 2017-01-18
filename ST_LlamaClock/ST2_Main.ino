//*******************************************************************************************************************
// 								                                         Main Loop 
//*******************************************************************************************************************

unsigned char bigDigits[] = 
{
  0x3e, 0x41, 0x41, 0x3e, // 0 
  0x00, 0x00, 0x00, 0x7F, // 1
  0x71, 0x49, 0x49, 0x46, // 2
  0x41, 0x49, 0x49, 0x36, // 3
  0x0F, 0x08, 0x08, 0x7E, // 4
  
  0x4F, 0x49, 0x49, 0x31, // 5
  0x3e, 0x49, 0x49, 0x31, // 6
  0x01, 0x01, 0x01, 0x7F, // 7
  0x36, 0x49, 0x49, 0x36, // 8
  0x06, 0x49, 0x49, 0x3e, // 9
};

unsigned char smallDigits[] = 
{
  0x0E, 0x11, 0x0E, // 0
  0x12, 0x1F, 0x10, // 1
  0x19, 0x15, 0x12, // 2
  0x11, 0x15, 0x0E, // 3
  0x07, 0x04, 0x1E, // 4

  0x17, 0x15, 0x09, // 5
  0x0E, 0x15, 0x09, // 6
  0x01, 0x19, 0x07, // 7
  0x1F, 0x15, 0x1F, // 8
  //0x16, 0x15, 0x0E, // 9
  0x17, 0x15, 0x1F, // 9
};

void printDigitSmall( int x, int val )
{
  for( int i = 0 ; i < 3 ; i++ )
  {
    if( (x+i) >= 0 && (x+i) < 20 ) {
      LEDMAT[ x+i ] = smallDigits[ (val*3) + i ] << 1;
    }
  }
}
void printDigitBig( int x, int val )
{
  for( int i = 0 ; i < 4 ; i++ )
  {
    if( (x+i) >= 0 && (x+i) < 20 ) {
      LEDMAT[ x+i ] = bigDigits[ (val*4) + i ];
    }
  }
}


void loop()
{
  char buf[16];
  static unsigned long lastSec = 0;

  checktime();
    
  if( SecOnes != lastSec ) {

    beepsound( (SecOnes & 0x01)? 880 : 220 , 5 );
  
    printDigitSmall(  2, MinTens );
    printDigitSmall(  6, MinOnes );
    LEDMAT[ 10 ] = 0x14;
    printDigitSmall( 12, SecTens );
    printDigitSmall( 16, SecOnes );
  }
  
  lastSec = SecOnes;
}

void oldLoop()
{
  // Test for Sleep ------------------------------------------------*

  currentMillis = millis();
  OptionModeFlag = false;

  if(((currentMillis - SleepTimer) > SleepLimit) && SleepEnable)
  {
    STATE= 99; 
  }

  // Test for Mode Button Press ------------------------------------* 

  bval = !digitalRead(MODEBUTTON);
  if(bval)
  {
    if(ALARMON)
    {
    CheckAlarm();
    }

    if(ALARM1FLAG)
    {
      ALARM1FLAG = false;
      ALARMON = false;
      EnableAlarm1(false);
      STATE = 90;
      JustWokeUpFlag = false;
    }
    else
    {
      if(JustWokeUpFlag)
      {
        JustWokeUpFlag = false;
        JustWokeUpFlag2 = true;                                    // Used to supress "Time" text from showing when waking up.
      }
      else
      {
      NextStateRequest = true;
      }
   //   SUBSTATE = 99;

      while(bval)
      {
          bval = !digitalRead(SETBUTTON);
          if(bval)
           {
            OptionModeFlag = true;
            NextStateRequest = false;
            NextSUBStateRequest = false;
            displayString("SPEC");
            delay(300);
           }      
        bval = !digitalRead(MODEBUTTON);
      }

      delay(100);
      SleepTimer = millis();
    }
  }

  // Test for SET Button Press ------------------------------------*
  
  bval = !digitalRead(SETBUTTON);
  if(bval && !OptionModeFlag)
  {
    NextSUBStateRequest = true;

    while(bval)
    {
      
          bval = !digitalRead(MODEBUTTON);
          if(bval)
           {
            OptionModeFlag = true;
            NextStateRequest = false;
            NextSUBStateRequest = false;
            displayString("SPEC");
            delay(300);
           }   
      
      
      bval = !digitalRead(SETBUTTON);
    }
    delay(100);
    SleepTimer = millis();
  }
 
   // Running Blink counter ------------------------------------* 
  if(blinkFlag)
  {
    blinkCounter = blinkCounter +1;
    if(blinkCounter >blinkTime)                                        // was 150
    {
      blinkON = !blinkON;
      blinkCounter = 0;
    }
  }
  else
  {
    blinkON = true;                                                    // Not blinking, just leave the LEDs lit
  }  
  
//*******************************************************************************************************************
// 								                        Main Loop - State Machine 
//*******************************************************************************************************************

  switch (STATE) 
  {
  case 0:                                                                  // Set-Up
    STATE = 1;
    break;

  case 1:                                                                  // Display Time
    DisplayTimeSub(); 
    break;

  case 2:                                                                  // Set Time
    setTimeSub();
    break; 

  case 3:                                                                  // Config Alarm
   setAlarmSub();
    break;
 
   case 4:                                                                 // Stop Watch
    StopWatch();
    break;
 
    
  case 5:                                                                 // Serial Display                                 
  DisplaySerialData();
  break;

  case 6:                                                                 // Graphic Demo                                 
  graphican();
  break;

   // ---------------------------------------------------------------   
  
  case 90:                                                                  // Alarm Triggered
  
    blinkFlag = true;
    displayString("Beep");

  if(blinkON)
  {
    pinMode(SETBUTTON, OUTPUT);
    tone(SETBUTTON,4000) ;
    delay(100);
    noTone(SETBUTTON);
    digitalWrite(SETBUTTON, HIGH);
  }

    #if ARDUINO >= 101 
    pinMode(SETBUTTON, INPUT_PULLUP);
//    digitalWrite(SETBUTTON, HIGH);
     #else
//    digitalWrite(SETBUTTON, HIGH);
    pinMode(SETBUTTON, INPUT);
     #endif
    delay(250);

//    bval = !digitalRead(SETBUTTON);
    if(NextSUBStateRequest | NextStateRequest)
    {
      STATE = 0;
      SUBSTATE = 0;
 //     NextStateFlag = true;
      NextStateRequest = false;
      NextSUBStateRequest = false;      
      blinkFlag = false;
    }    
    break;

    // --------------------------------------------------------------- 

  case 99:                                                                    // Sleep
    displayString("Nite");
    delay(500);
    clearmatrix();
    GoToSleep();
    SleepTimer = millis();
    STATE = 0;
    SUBSTATE = 0;
    break;

    // --------------------------------------------------------------- 

  }
}


//*******************************************************************************************************************
// 								                                 End of Main Loop
//*******************************************************************************************************************







