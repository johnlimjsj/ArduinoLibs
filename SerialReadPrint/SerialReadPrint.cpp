#include "SerialReadPrint.h"

void SerialReadPrint::s_print(String message, int result){Serial.print(message); Serial.print(result);}
void SerialReadPrint::s_print(String message, float result){Serial.print(message); Serial.print(result);}
void SerialReadPrint::s_print(String message, char result){ Serial.print(message); Serial.print(result);}
void SerialReadPrint::s_print(String message, String result){Serial.print(message); Serial.print(result);}

void SerialReadPrint::s_println(String message, int result){Serial.print(message); Serial.print(result);Serial.println();}
void SerialReadPrint::s_println(String message, float result){Serial.print(message); Serial.print(result);Serial.println();}
void SerialReadPrint::s_println(String message, char result){Serial.print(message); Serial.print(result);Serial.println();}
void SerialReadPrint::s_println(String message, String result){Serial.print(message); Serial.print(result);Serial.println();}


void SerialReadPrint::s_read(String message, int *reading)
{  
  _read_message(message);
  if(Serial.available()){*reading = Serial.parseInt();Serial.print(*reading);}
  _read_clearbuff();
}

void SerialReadPrint::s_read(String message, float *reading)
{  
  _read_message(message);
  if(Serial.available()>0){*reading = Serial.parseFloat();Serial.print(*reading);}
  _read_clearbuff();
}

void SerialReadPrint::s_read(String message, char *reading)
{  
  _read_message(message);
  if(Serial.available()){*reading = Serial.read();Serial.print(*reading);}  
  _read_clearbuff();
}

void SerialReadPrint::s_read(String message, String *reading)
{  
  _read_message(message);
  if(Serial.available()>0){*reading = Serial.readString();Serial.print(*reading);}
  _read_clearbuff();
}


void SerialReadPrint::_read_message(String message)
{
  Serial.print(message); 
  while(!Serial.available()){}
}

void SerialReadPrint::_read_clearbuff()
{
  while(Serial.available()){Serial.read(); Serial.flush();} // to clear serial buffer
  Serial.println();
}

