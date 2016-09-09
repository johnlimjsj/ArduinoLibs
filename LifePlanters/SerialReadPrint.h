#ifndef SerialReadPrint_h
#define SerialReadPrint_h

#include "Arduino.h"

class SerialReadPrint
{
  public:
    void s_print(String message, int result);
    void s_print(String message, unsigned int result);
    void s_print(String message, float result);
    void s_print(String message, char result);
    void s_print(String message, String result);
    
    void s_println(String message, int result);
    void s_println(String message, float result);
    void s_println(String message, char result);
    void s_println(String message, String result);   

 /* ==== To read from Serial buffer ======== */
    void s_read(String, int *);
    void s_read(String, float *);
    void s_read(String, char *);
    void s_read(String, String *);


  private:
    void _read_message(String);
    void _read_clearbuff();
};

#endif