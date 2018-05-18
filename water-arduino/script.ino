#include <stdio.h>
#include <string.h>

#define WATERPUMP_RELAY_PIN   4
#define WATERFLOW_PIN         3
#define MOISTURE_SENSOR_PIN1  0

#define STANDBY_DELAY         600000
#define WATERING_DELAY        20000

enum State {
  STANDBY = 0,
  WATERING = 1,
};
typedef enum State Systemstate;
Systemstate CurrentSystemState;

float DRY_LIMIT = 10;
char MESSAGE[100];

int WATER_FAIL_SETS = 0;
int WATER_FAIL_REPS = 0;
int WATER_FAIL_REP_MAX = 3;
boolean WATER_FAIL_TIMEOUT = false;
int WATER_FAIL_TIMEOUT_REPS = 0;
int WATER_FAIL_TIMEOUT_REP_MAX = 6;

char TIMESTAMP[20];

void setup() {

  Serial.begin(9600); 
  
  pinMode(WATERFLOW_PIN, INPUT);
  pinMode(MOISTURE_SENSOR_PIN1, INPUT);
  pinMode(WATERPUMP_RELAY_PIN, OUTPUT);

  CurrentSystemState = STANDBY;
}

void loop() {
  float moisture1 = 100;
  int waterIsFlowing = false;
  
  memset(MESSAGE, '\0', sizeof(MESSAGE));
  strcat(MESSAGE, "{");

  memset(TIMESTAMP, '\0', sizeof(TIMESTAMP));
  ultoa(millis(), TIMESTAMP, 10);
  makeMessageEntry("timestamp", TIMESTAMP);    
    
  switch (CurrentSystemState) {
    case STANDBY:
      makeMessageEntry("state", "STANDBY");

      moisture1 = correct(analogRead(MOISTURE_SENSOR_PIN1));
      makeMessageEntry("moisture1", moisture1);

      makeMessageEntry("water_fail_reps", WATER_FAIL_REPS); 
      makeMessageEntry("water_fail_sets", WATER_FAIL_SETS);
      
      if(moisture1 < DRY_LIMIT){
        if (!WATER_FAIL_TIMEOUT) {
          beginWatering();
          break;
        } else {
          WATER_FAIL_TIMEOUT_REPS++;
          if (WATER_FAIL_TIMEOUT_REPS >= WATER_FAIL_TIMEOUT_REP_MAX) {
            WATER_FAIL_TIMEOUT = false;
            WATER_FAIL_REPS = 0;
          }
        }
      }
      
      delay(STANDBY_DELAY);

      break;

   
    case WATERING:
      makeMessageEntry("state", "WATERING");
      
      moisture1 = correct(analogRead(MOISTURE_SENSOR_PIN1));
      makeMessageEntry("moisture1", moisture1);

      waterIsFlowing = digitalRead(WATERFLOW_PIN);
      makeMessageEntry("water_flow", waterIsFlowing);

      if(!waterIsFlowing){
        WATER_FAIL_REPS++;
        if (WATER_FAIL_REPS >= WATER_FAIL_REP_MAX){
          WATER_FAIL_TIMEOUT = true;
          WATER_FAIL_TIMEOUT_REPS = 0;
          WATER_FAIL_SETS++;
        }
      }
      makeMessageEntry("water_fail_reps", WATER_FAIL_REPS);
      makeMessageEntry("water_fail_sets", WATER_FAIL_SETS);
 
      delay(WATERING_DELAY);

      if(moisture1 > DRY_LIMIT || !waterIsFlowing){
        stopWatering(); 
        break;
      }
      
      break;

      
    default:
      break;
  }

  strcat(MESSAGE, "}");
  Serial.println(MESSAGE);
}

void makeMessageEntry(char str[], char str2[]) {
  if(strlen(MESSAGE) > 1){
    strcat(MESSAGE, ",");
  }
  
  strcat(MESSAGE, "\"");
  strcat(MESSAGE, str);
  strcat(MESSAGE, "\":\"");
  strcat(MESSAGE, str2);
  strcat(MESSAGE, "\"");
}

void makeMessageEntry(char str[], float number) {
  char numberString[5];

  if(strlen(MESSAGE) > 1){
    strcat(MESSAGE, ",");
  }
  
  memset(numberString, '\0', sizeof(numberString));

  strcat(MESSAGE, "\"");
  strcat(MESSAGE, str);
  strcat(MESSAGE, "\":\"");
  
  ftoa(number, numberString, 2);
  strcat(MESSAGE, numberString);
  strcat(MESSAGE, "\"");
}

void makeMessageEntry(char str[], int number) {
  char numberString[5];

  if(strlen(MESSAGE) > 1){
    strcat(MESSAGE, ",");
  }
  
  memset(numberString, '\0', sizeof(numberString));

  strcat(MESSAGE, "\"");
  strcat(MESSAGE, str);
  strcat(MESSAGE, "\":\"");
  
  itoa(number, numberString, 10);
  strcat(MESSAGE, numberString);
  strcat(MESSAGE, "\"");
}

void beginWatering(){
  CurrentSystemState = WATERING;
  digitalWrite(WATERPUMP_RELAY_PIN, HIGH);
}

void stopWatering(){
  CurrentSystemState = STANDBY;
  digitalWrite(WATERPUMP_RELAY_PIN, LOW);
}

float correct(float moistureReading) {
  float inverse = 1024 - moistureReading;
  return (inverse / 1024)*100;
}

// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;
 
    // Extract floating part
    float fpart = n - (float)ipart;
 
    // convert integer part to string
    int i = intToStr(ipart, res, 0);
 
    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot
 
        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);
 
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}

void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
