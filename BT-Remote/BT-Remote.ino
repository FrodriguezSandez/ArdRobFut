#include <SoftwareSerial.h>// import the serial library

#define MESSAGE_MAX_LEN 15
#define BUFFER_MIN_LEN 5



#define MOT_IZQ 1
#define MOT_DER 2


//min 150
//max 255

int MOT_DER_A;
int MOT_DER_B;
int MOT_IZQ_A;
int MOT_IZQ_B;

SoftwareSerial bt(11, 12); // RX, TX
char serialData[MESSAGE_MAX_LEN];

void setup() {

  bt.begin(9600);
  Serial.begin(9600);
  Serial.println("ok");

  setMotores(5,6,9,10);
  parar(0);

  //analogWrite(MOT_DER_B, 1);


  // put your setup code here, to run once:
  
}

void setMotores(int mot_der_a,int mot_der_b,int mot_izq_a,int mot_izq_b)
{
  MOT_DER_A = mot_der_a;
  MOT_DER_B = mot_der_b;
  MOT_IZQ_A = mot_izq_a;
  MOT_IZQ_B = mot_izq_b;
  
  pinMode(mot_der_a,OUTPUT);
  pinMode(mot_der_b,OUTPUT);
  pinMode(mot_izq_a,OUTPUT);
  pinMode(mot_izq_b,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly
  
  int longitud = bt.available();
  
   if(longitud > 0) {

      for(int i = 0; i < MESSAGE_MAX_LEN; i++) serialData[i] = '\0';
      
      // Leer el el buffer y guardar en variable
      bt.readBytesUntil('\n', serialData, MESSAGE_MAX_LEN - 1);

      if(serialData[0]=='\0' || serialData[0]=='\n')
          Serial.println("null");
      else
        {
            Serial.println(serialData);
            executeCommand(serialData);
        }

      bt.println(0);
    }
}

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}   


void adelante ()
{

  digitalWrite(MOT_DER_A,LOW);
  digitalWrite(MOT_DER_B,HIGH);
  digitalWrite(MOT_IZQ_A,HIGH);
  digitalWrite(MOT_IZQ_B,LOW);  

}

void atras ()
{
  digitalWrite(MOT_DER_A,HIGH);
  digitalWrite(MOT_DER_B,LOW);
  digitalWrite(MOT_IZQ_A,LOW);
  digitalWrite(MOT_IZQ_B,HIGH);  

}

void derecha ()
{

  digitalWrite(MOT_DER_A,HIGH);
  digitalWrite(MOT_DER_B,LOW);
  digitalWrite(MOT_IZQ_A,HIGH);
  digitalWrite(MOT_IZQ_B,LOW); 

}

void izquierda ()
{
  digitalWrite(MOT_DER_A,LOW);
  digitalWrite(MOT_DER_B,HIGH);
  digitalWrite(MOT_IZQ_A,LOW);
  digitalWrite(MOT_IZQ_B,HIGH); 
}

void parar(int motor)
{
 
    digitalWrite(MOT_DER_A,LOW);
    digitalWrite(MOT_DER_B,LOW);
    digitalWrite(MOT_IZQ_A,LOW);
    digitalWrite(MOT_IZQ_B,LOW);
  
}

//int limite x +-30
//int limite y +-20



void executeCommand(char* command)
{
  int values[2];
  parseCommand(command,values,2);

  Serial.println(values[0]);
  Serial.println(values[1]);
  
  
  if(values[1] >= 20)
  {
    //va hacia adelante
    Serial.println("adelante");
    adelante();
    return;

  }else if(values[1] < -20)
  {
    //va hacia atras
    Serial.println("atras");
    atras();
    return;
  }else if(values[1] < 20 && values[1] > -20)
  {
    //va a rotar
    if (values[0]>30) {
      //va a rotar a la derecha
      Serial.println("rotar Der");
      derecha();
      return;

    }
    if (values[0]<-30) {
      //va a rotar a la izquierda
      Serial.println("rotar Izq");
      izquierda();
      return;

  }
  else
  {
    parar(0);
    return;

  }
    
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

