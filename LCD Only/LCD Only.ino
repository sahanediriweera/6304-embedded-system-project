#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
LiquidCrystal lcd(13,8,9,10,11,12);

  int x= 0;
void setup() {
  lcd.begin(16, 2);
   
  lcd.print("Hello, World!");


}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:

  lcd.print(millis() / 1000);

  lcd.setCursor(0,0);

  lcd.print(x++);
}