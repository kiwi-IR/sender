/*
   Sender des Kiwi Projekts "Infrarot Übertragung"
*/
#include "SevSeg.h"
#include "Keypad.h"
#include "IRremote.h"
SevSeg sevseg;
IRsend irsend;

// Keypad initialisieren
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {6, 12, 11, 8};
byte colPins[COLS] = {7, 5, 10};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Variablen
int i;
int tosend[10] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
const char numbers[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

// Gespeicherte Zahlen ausgeben
void printar(int *arr) {
  Serial.print("Numbers: ");
  for (i = 0; i < 10; i++) {
    if (arr[i] == 10) {
      i = 10;
    } else {
      Serial.print(arr[i]);
      Serial.print("-");
    }
  }
  Serial.println("");
}

// Letzte zwei Zahlen auf dem Display zeigen
void dlast(int *arr) {
  int lastd = 9;
  char disp[2];
  for (i = 0; i < 10; i++) {
    if (arr[i] == 10) {
      lastd = i - 1;
      i = 10;
    }
  }
  if (lastd == 0) {
    disp[0] =  ' ';
    disp[1] = numbers[arr[lastd]];
  } else if (lastd == -1) {
    disp[0] = ' ';
    disp[1] = ' ';
  } else {
    disp[0] = numbers[arr[lastd - 1]];
    disp[1] = numbers[arr[lastd]];
  }
  Serial.print("Display: ");
  Serial.print(disp[0]);
  Serial.println(disp[1]);
  //Serial.println(lastd);
  Serial.println("------------");
  sevseg.setChars(disp);
}

// Sendet die Zahlen über Infrarot zum Empfänger
void infrarot(int *arr) {
  irsend.sendRC5(12, 12);
  delay(20);
  for (i = 0; i < 10; i++) {
    switch(arr[i]) {
      case 1: arr[i] = 21; break;
      case 3: arr[i] = 23; break;
      case 7: arr[i] = 27; break;
      case 0: arr[i] = 20; break;
    }
    irsend.sendRC5(arr[i], 12);
    delay(20);
    Serial.print("TX:");
    Serial.println(arr[i]);
  }
  irsend.sendRC5(13, 12);
}

void setup() {
  // Ausgabe auf der Konsole
  Serial.begin(9600);
  Serial.println("Gestartet");

  // 7 Segment Display initialisieren
  byte numDigits = 2;
  byte digitPins[] = {A5, A4};
  byte segmentPins[] = {A3, A0, 2, 9, 4, A2, A1, 13};
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);
  sevseg.setBrightness(100);

  dlast(tosend);
}

void loop() {
  char key = keypad.getKey();
  if (key != NO_KEY) {
    Serial.print("Taste: ");
    Serial.println(key);
    int k = key % 48;
    if (k == 35) {
      Serial.print("Senden: ");
      printar(tosend);
      infrarot(tosend);
      for (i = 0; i < 10; i++) {
        tosend[i] = 10;
      }
    } else if (k == 42) {
      Serial.println("Löschen");
      for (i = 0; i < 10; i++) {
        tosend[i] = 10;
      }
      printar(tosend);
    } else {
      for (i = 0; i < 10; i++) {
        if (tosend[i] == 10) {
          tosend[i] = k;
          i = 10;
        }
      }
      printar(tosend);
    }
    dlast(tosend);
  }
  sevseg.refreshDisplay();
}
