#include <wiringPi.h>
#include <stdlib.h>

int main (void)
{
  wiringPiSetup () ;
  
  pinMode (21, OUTPUT) ;
  
  digitalWrite (21, LOW);
  delay(3000);
  digitalWrite (21,  HIGH);
  delay(3000);
  
  return 0 ;
}