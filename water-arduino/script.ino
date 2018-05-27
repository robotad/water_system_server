#include <stdio.h>
#include <string.h>

#define WATERPUMP_RELAY_PIN   4
#define MOISTURE_SENSOR_PIN1  0
#define MOISTURE_SENSOR_PIN2  1
#define MOISTURE_SENSOR_PIN3  2
#define MOISTURE_SENSOR_PIN4  3

#define STANDBY_DELAY         1800000
#define WATERING_DELAY        600000

enum State {
  STANDBY = 0,
  WATERING = 1,
};
typedef enum State Systemstate;
Systemstate CurrentSystemState;

float DRY_LIMIT = 10;
float MOISTURE_DIFF_THRESHOLD = 2;

int WATER_FAIL_SETS = 0;
int WATER_FAIL_REPS = 0;
int WATER_FAIL_REP_MAX = 3;
boolean WATER_FAIL_TIMEOUT = false;
int WATER_FAIL_TIMEOUT_REPS = 0;
int WATER_FAIL_TIMEOUT_REP_MAX = 6;


void setup() {

  Serial.begin(9600); 
  
  pinMode(MOISTURE_SENSOR_PIN1, INPUT);
  pinMode(MOISTURE_SENSOR_PIN2, INPUT);
  pinMode(MOISTURE_SENSOR_PIN3, INPUT);
  pinMode(MOISTURE_SENSOR_PIN4, INPUT);
  pinMode(WATERPUMP_RELAY_PIN, OUTPUT);

  CurrentSystemState = STANDBY;
}

float MOISTURE1 = 100.0;
float MOISTURE2 = 100.0;
float MOISTURE3 = 100.0;
float MOISTURE4 = 100.0;

void loop() {
  float prevMoisture1 = MOISTURE1;
  float prevMoisture2 = MOISTURE2;
  float prevMoisture3 = MOISTURE3;
  float prevMoisture4 = MOISTURE4;
      
  char MESSAGE[200];
  char TIMESTAMP[20];
  
  int waterIsFlowing = false;
  
  memset(MESSAGE, 0, sizeof(MESSAGE));
  strcat(MESSAGE, "{");

  memset(TIMESTAMP, 0, sizeof(TIMESTAMP));
  ultoa(millis(), TIMESTAMP, 10);
  makeMessageEntry(MESSAGE, "timestamp", TIMESTAMP);    
    
  switch (CurrentSystemState) {
    case STANDBY:
      makeMessageEntry(MESSAGE, "state", "STANDBY");

      MOISTURE1 = correct(analogRead(MOISTURE_SENSOR_PIN1));
      makeMessageEntry(MESSAGE, "moisture1", MOISTURE1);

      MOISTURE2 = correct(analogRead(MOISTURE_SENSOR_PIN2));
      makeMessageEntry(MESSAGE, "moisture2", MOISTURE2);

      MOISTURE3 = correct(analogRead(MOISTURE_SENSOR_PIN3));
      makeMessageEntry(MESSAGE, "moisture3", MOISTURE3);

      MOISTURE4 = correct(analogRead(MOISTURE_SENSOR_PIN4));
      makeMessageEntry(MESSAGE, "moisture4", MOISTURE4);
      
      makeMessageEntry(MESSAGE, "water_fail_reps", WATER_FAIL_REPS); 
      makeMessageEntry(MESSAGE, "water_fail_sets", WATER_FAIL_SETS);
      
      if(MOISTURE1 < DRY_LIMIT || MOISTURE2 < DRY_LIMIT || MOISTURE3 < DRY_LIMIT || MOISTURE4 < DRY_LIMIT){
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
      makeMessageEntry(MESSAGE, "state", "WATERING");

      MOISTURE1 = correct(analogRead(MOISTURE_SENSOR_PIN1));
      makeMessageEntry(MESSAGE, "moisture1", MOISTURE1);

      MOISTURE2 = correct(analogRead(MOISTURE_SENSOR_PIN2));
      makeMessageEntry(MESSAGE, "moisture2", MOISTURE2);

      MOISTURE3 = correct(analogRead(MOISTURE_SENSOR_PIN3));
      makeMessageEntry(MESSAGE, "moisture3", MOISTURE3);

      MOISTURE4 = correct(analogRead(MOISTURE_SENSOR_PIN4));
      makeMessageEntry(MESSAGE, "moisture4", MOISTURE4);

      if(abs(prevMoisture1 - MOISTURE1) < MOISTURE_DIFF_THRESHOLD &&
        abs(prevMoisture2 - MOISTURE2) < MOISTURE_DIFF_THRESHOLD &&
        abs(prevMoisture3 - MOISTURE3) < MOISTURE_DIFF_THRESHOLD &&
        abs(prevMoisture4 - MOISTURE4) < MOISTURE_DIFF_THRESHOLD){
          waterIsFlowing = false;
      }

      if(!waterIsFlowing){
        WATER_FAIL_REPS++;
        if (WATER_FAIL_REPS >= WATER_FAIL_REP_MAX){
          WATER_FAIL_TIMEOUT = true;
          WATER_FAIL_TIMEOUT_REPS = 0;
          WATER_FAIL_SETS++;
        }
      }
      makeMessageEntry(MESSAGE, "water_fail_reps", WATER_FAIL_REPS);
      makeMessageEntry(MESSAGE, "water_fail_sets", WATER_FAIL_SETS);
 
      delay(WATERING_DELAY);

      if(MOISTURE1 > DRY_LIMIT || MOISTURE2 > DRY_LIMIT || MOISTURE3 > DRY_LIMIT || MOISTURE4 > DRY_LIMIT || !waterIsFlowing){
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

void makeMessageEntry(char message[], char str[], char str2[]) {
  if(strlen(message) > 1){
    strcat(message, ",");
  }
  
  strcat(message, "\"");
  strcat(message, str);
  strcat(message, "\":\"");
  strcat(message, str2);
  strcat(message, "\"");
}

void makeMessageEntry(char message[], char str[], float number) {
  char numberString[5];

  if(strlen(message) > 1){
    strcat(message, ",");
  }
  
  memset(numberString, '\0', sizeof(numberString));

  strcat(message, "\"");
  strcat(message, str);
  strcat(message, "\":\"");
  
  ftoa(number, numberString, 2);
  strcat(message, numberString);
  strcat(message, "\"");
}

void makeMessageEntry(char message[], char str[], int number) {
  char numberString[5];

  if(strlen(message) > 1){
    strcat(message, ",");
  }
  
  memset(numberString, '\0', sizeof(numberString));

  strcat(message, "\"");
  strcat(message, str);
  strcat(message, "\":\"");
  
  itoa(number, numberString, 10);
  strcat(message, numberString);
  strcat(message, "\"");
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
