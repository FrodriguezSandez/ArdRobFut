#include <SoftwareSerial.h>// import the serial library

#define MESSAGE_MAX_LEN 15
#define BUFFER_MIN_LEN 5

SoftwareSerial bt(10, 11); // RX, TX
char serialData[MESSAGE_MAX_LEN];

void setup() {
  // put your setup code here, to run once:
  bt.begin(9600);
  Serial.begin(9600);
  pinMode(ledpin, OUTPUT);
  Serial.println("ok");
}

void loop() {
  // put your main code here, to run repeatedly
  
  int longitud = bt.available();
  
   if(longitud > 0) {

    if(longitud >= BUFFER_MIN_LEN) {
      // Limpoar la cadena del mensaje
      for(int i = longitud; i < MESSAGE_MAX_LEN; i++) serialData[i] = '\0';
      
      // Leer el el buffer y guardar en variable
      bt.readBytesUntil('\n', serialData, MESSAGE_MAX_LEN - 1);

      serialFlush();

      Serial.println(serialData);

      int speed[2];
      if(parseCommand(serialData, speed, 2))
      {
        setSpeed(speed[0], speed[1]);
      }
      else
      {
        Serial.println("Error while setting new speed");
      }

    }

    // Responder todos los mensajes (pongs)
    bt.println(0);
   }
}

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}   



/**
 * This function makes ints out of the received serial data, the 2 first
 * characters are not counted as they consist of the command character and
 * a comma separating the first variable.
 *
 * @params command The whole serial data received as an address
 * @params returnValues The array where the return values go as an address
 * @params returnNumber The number of values to set inside the returnValues variable
 */
boolean parseCommand(char* command, int* returnValues, byte returnNumber)
{
  // parsing state machine
  byte i = 1, j = 0, sign = 0, ch = 0, number;
  int temp = 0;
  while(i++)
  {
    switch(*(command + i))
    {
    case '\0':
    case ',':
      // set return value
      if(ch != 0)
      {
        returnValues[j++] = sign?-temp:temp;
        sign = 0;
        temp = 0;
        ch = 0;
      }
      else
      {
        return false;
      }
      break;
    case '-':
      sign = 1;
      break;
    default:
      // convert string to int
      number = *(command + i) - '0';
      if(number < 0 || number > 9)
      {
        return false;
      }
      temp = temp * 10 + number;
      ch++;
    }

    // enough return values have been set
    if(j == returnNumber)
    {
      return true;
    }
    // end of command reached
    else if(*(command + i) == '\0')
    {
      return false;
    }
  }
}

