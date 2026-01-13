# NFC Smart Lock System using Arduino & PN532

This project implements an **NFC-based access control system** using an Arduino Uno, PN532 NFC reader, LCD display, servo motor lock, and push buttons to manage authorized cards.  
Users can scan NFC cards to unlock a door, add new authorized cards, or remove existing ones.

---

## 📦 Features

- NFC card authentication using PN532 (I²C)
- Stores up to 10 authorized card UIDs
- Supports 4-byte and 7/8-byte UID cards
- Add and remove cards using dedicated buttons
- Visual feedback with LEDs
- 16x2 LCD status display
- Servo-controlled locking mechanism

---

## 🛠 Hardware Required

- Arduino Uno  
- PN532 NFC Module (I²C mode)  
- 16x2 LCD (parallel interface)  
- SG90 Servo Motor  
- 2 Push Buttons  
- 2 LEDs + 220Ω resistors  
- Breadboard + jumper wires  

---

## 🔌 Wiring

### PN532 (I²C)
| PN532 | Arduino |
|--------|---------|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

### LCD 16x2 (4-bit mode)
| LCD Pin | Arduino |
|---------|---------|
| RS | 12 |
| EN | 11 |
| D4 | 6 |
| D5 | 5 |
| D6 | 4 |
| D7 | 3 |
| VSS | GND |
| VDD | 5V |
| VO | GND (max contrast) |
| A | 5V via 220Ω |
| K | GND |

### Servo
| Servo | Arduino |
|--------|---------|
| Signal | D7 |
| VCC | 5V |
| GND | GND |

### Buttons
| Button | Arduino | Mode |
|---------|---------|------|
| Add Card | D2 | INPUT_PULLUP |
| Remove Card | D8 | INPUT_PULLUP |

### LEDs
| LED | Arduino | Resistor |
|-----|---------|----------|
| Green | D10 | 220Ω |
| Red | D9 | 220Ω |

---

## ⚙️ Installation

1. Install required libraries in Arduino IDE:
   - Adafruit PN532
   - LiquidCrystal
   - Servo
   - Wire

2. Upload the code to the Arduino Uno.

3. Power the circuit.

---

## 🚀 How It Works

### Normal Mode
- LCD displays: "Scan your card"
- If scanned UID matches stored list → Green LED + Servo unlocks
- If not recognized → Red LED + "Access Denied"

### Add Mode
- Press **Add Button**
- Green LED blinks for 5 seconds
- Scan a card to add it to memory

### Remove Mode
- Press **Remove Button**
- Red LED blinks for 5 seconds
- Scan a card to remove it from memory

---

## 🔐 Lock Mechanism

- Servo default closed position: 0°
- On access granted: rotates to 90° for 2 seconds
- Returns to locked position

---

## 💾 Memory Limits

- Stores up to **10 card UIDs**
- UID length automatically handled (4–8 bytes)

---

## ⚠️ Notes

- Cards are stored in RAM only  
  (Power loss clears authorized list)
- For permanent storage, EEPROM can be added

---

## 📄 License

This project is open-source and free to use.
