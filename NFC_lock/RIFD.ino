#include <Adafruit_PN532.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Wire.h>          

#define PN532_IRQ   (-1)
#define PN532_RESET (-1)
#define ADD_B (2)
#define REM_B (8)
#define RED (9)
#define GREEN (10)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);
Servo lockServo;

uint8_t authorizedUID[10][8];
int numAuthorizedCards = 0;

void printUID(uint8_t *uid, uint8_t uidLength) {
  Serial.print("Card UID: ");
  for (int i = 0; i < uidLength; i++) {
    if (uid[i] < 0x10) Serial.print("0");
    Serial.print(uid[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void printWrapped(String text) {
  lcd.clear();
  
  if (text.length() <= 16) {
    lcd.setCursor(0, 0);
    lcd.print(text);
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print(text.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(text.substring(16, 32));
  }
}

int find_index(uint8_t *uid, uint8_t uidLength)
{
  int i;
  int j;
  
  if (uidLength < 4)
    return (-1);

  j = 0;
  while (j < numAuthorizedCards)
  {
    i = 0;
    while (i < uidLength)
    {
      if (uid[i] == authorizedUID[j][i])
      {
        if (i == (uidLength - 1))
          return (j);
        i++;
      }
      else
        break;
    }
    j++;
  }
  return (-1);
}

bool isAuthorized(uint8_t *uid, uint8_t uidLength)
{      
  if (find_index(uid, uidLength) >= 0)
    return (true);
  return (false);
}

void add_card(uint8_t *uid, uint8_t uidLength)
{
  if (numAuthorizedCards >= 10)
  {
    printWrapped("Memory Full!");
    delay(2000);
    return;
  }
  
  if (find_index(uid, uidLength) >= 0)
  {
    printWrapped("The card AlreadyExists!");
    delay(2000);
    return;
  }
  
  for (int i = 0; i < uidLength; i++)
  {
    authorizedUID[numAuthorizedCards][i] = uid[i];
  }
  numAuthorizedCards++;
  
  printWrapped("Card Added!");
  Serial.print("Card added. Total cards: ");
  Serial.println(numAuthorizedCards);
  delay(2000);
}

void remove_card(uint8_t *uid, uint8_t uidLength)
{
  int index = find_index(uid, uidLength);
  
  if (index < 0)
  {
    printWrapped("Card Not Found!");
    Serial.println("Card not in system");
    delay(2000);
    return;
  }
  
  for (int j = index; j < numAuthorizedCards - 1; j++)
  {
    for (int i = 0; i < uidLength; i++)
    {
      authorizedUID[j][i] = authorizedUID[j + 1][i];
    }
  }
  numAuthorizedCards--;
  
  printWrapped("Card Removed!");
  Serial.print("Card removed. Total cards: ");
  Serial.println(numAuthorizedCards);
  delay(2000);
}

void unlock() {
  lockServo.write(90);
  delay(2000);
  lockServo.write(0);         
}

void setup()
{
  Serial.begin(115200);
  pinMode(RED, OUTPUT);
  pinMode(ADD_B, INPUT_PULLUP);
  pinMode(REM_B, INPUT_PULLUP);
  pinMode(GREEN, OUTPUT);
  lockServo.attach(7);
  lockServo.write(0);
  lcd.begin(16, 2);
  
  Serial.println("PN532 NFC Lock Starting...");
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    printWrapped("PN532 Error! Check wiring!");
    Serial.println("PN532 not found. Check wiring!");
    while (1);
  }
  
  Serial.print("Found PN532, firmware version: ");
  Serial.print((versiondata >> 16) & 0xFF, HEX);
  Serial.print(".");
  Serial.println((versiondata >> 8) & 0xFF, HEX);
  nfc.SAMConfig();
  Serial.println("PN532 ready. Tap your NFC tag.");
  delay(2000);
}

void loop() {
  uint8_t uid[8];
  uint8_t uidLength; 
  unsigned long startTime;
  unsigned long lastBlink = 0;
  bool ledState = false;

  lockServo.write(0);
  digitalWrite(RED, LOW);  
  digitalWrite(GREEN, LOW);
  
  if (digitalRead(REM_B) == LOW)
  {
    startTime = millis();
    lastBlink = millis();
    bool cardFound = false;
    
    printWrapped("Scan to Remove!");
    Serial.println("=== REMOVE MODE - 5 second window ===");
    
    while (millis() - startTime < 5000 && !cardFound)
    {
      if (millis() - lastBlink >= 250)
      {
        ledState = !ledState;
        digitalWrite(RED, ledState ? HIGH : LOW);
        lastBlink = millis();
      }
      
      if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100))
      {
        cardFound = true;
        digitalWrite(RED, HIGH);  
        printUID(uid, uidLength);
        remove_card(uid, uidLength);
      }
      
      int timeLeft = 5 - ((millis() - startTime) / 1000);
      lcd.setCursor(0, 1);
      lcd.print("Time left: ");
      lcd.print(timeLeft);
      lcd.print("s");
      delay(50);
    }
    
    if (!cardFound)
    {
      printWrapped("Timeout!");
      digitalWrite(RED, HIGH);  
      Serial.println("Remove mode timed out");
      delay(1000);
    }
    return;
  }
  
  if (digitalRead(ADD_B) == LOW)
  {
    startTime = millis();
    lastBlink = millis();
    bool cardFound = false;
    
    printWrapped("Scan to Add");
    Serial.println("=== ADD MODE - 5 second window ===");
        
    while (millis() - startTime < 5000 && !cardFound)
    {
      if (millis() - lastBlink >= 250)
      {
        ledState = !ledState;
        digitalWrite(GREEN, ledState ? HIGH : LOW);
        lastBlink = millis();
      }
      
      if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100))
      {
        cardFound = true;
        digitalWrite(GREEN, HIGH);
        printUID(uid, uidLength);
        add_card(uid, uidLength);
      }
      
      int timeLeft = 5 - ((millis() - startTime) / 1000);
      lcd.setCursor(0, 1);
      lcd.print("Time left: ");
      lcd.print(timeLeft);
      lcd.print("s");
      delay(50);
    }
    if (!cardFound)
    {
      digitalWrite(GREEN, HIGH);
      printWrapped("Timeout!");
      Serial.println("Add mode timed out");
      delay(1000);
    }
    return;
  }
  
  printWrapped("Scan your card");
  
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100))
  {
    printUID(uid, uidLength);
    
    if (isAuthorized(uid, uidLength))
    {
      printWrapped("Access granted!");
      Serial.println(">>> ACCESS GRANTED <<<");
      digitalWrite(GREEN, HIGH);  
      unlock();
      delay(1000);
    }
    else
    {
      printWrapped("Access denied!");
      Serial.println(">>> ACCESS DENIED <<<");
      digitalWrite(RED, HIGH);
      delay(2000);
    }
  }
}