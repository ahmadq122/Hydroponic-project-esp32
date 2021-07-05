void LCD_PrintStringAtCursor(char * Str, byte column, byte row) {
  lcd.setCursor(column, row);
  lcd.print(Str);
}
void LCD_PrintStringAtCursor(String Str, byte column, byte row) {
  lcd.setCursor(column, row);
  lcd.print(Str);
}
void LCD_PrintCharAtCursor(char Char, byte column, byte row) {
  lcd.setCursor(column, row);
  lcd.print(Char);
}

void LCD_ClearAtRow(byte numberOfSpaces, byte column, byte row) {
  for (byte i = 0; i < numberOfSpaces; i++) {
    lcd.setCursor(column + i, row);
    lcd.print(" ");
  }
}
