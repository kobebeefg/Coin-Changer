unsigned char i;
unsigned char j;
/*Port Definitions*/
int Max7219_pinCLK = 8;
int Max7219_pinCS = 7;
int Max7219_pinDIN = 6;

unsigned char disp1[38][8] = {
  {0x00, 0x7e, 0x81, 0x81, 0x81, 0x81, 0x7e, 0x00}, //0
  {0x00, 0x00, 0x01, 0x01, 0xff, 0x41, 0x01, 0x00}, //1
  {0x00, 0x00, 0x71, 0x89, 0x89, 0x89, 0x47, 0x00}, //2
  {0x00, 0x00, 0x66, 0x99, 0x89, 0x81, 0x41, 0x00}, //3
  {0x00, 0x08, 0x08, 0x9f, 0x48, 0x28, 0x18, 0x00}, //4
  {0x00, 0x00, 0x8c, 0x92, 0x91, 0x91, 0xe1, 0x00}, //5
  {0x08, 0x04, 0x62, 0x02, 0x02, 0x62, 0x04, 0x08}, //6
};



void Write_Max7219_byte(unsigned char DATA)
{
  unsigned char i;
  digitalWrite(Max7219_pinCS, LOW);
  for (i = 8; i >= 1; i--)
  {
    digitalWrite(Max7219_pinCLK, LOW);
    digitalWrite(Max7219_pinDIN, DATA & 0x80); // Extracting a bit data
    DATA = DATA << 1;
    digitalWrite(Max7219_pinCLK, HIGH);
  }
}


void Write_Max7219(unsigned char address, unsigned char dat)
{
  digitalWrite(Max7219_pinCS, LOW);
  Write_Max7219_byte(address);           //address，code of LED
  Write_Max7219_byte(dat);               //data，figure on LED
  digitalWrite(Max7219_pinCS, HIGH);
}

void Init_MAX7219(void)
{
  Write_Max7219(0x09, 0x00);       //decoding ：BCD
  Write_Max7219(0x0a, 0x03);       //brightness
  Write_Max7219(0x0b, 0x07);       //scanlimit；8 LEDs
  Write_Max7219(0x0c, 0x01);       //power-down mode：0，normal mode：1
  Write_Max7219(0x0f, 0x00);       //test display：1；EOT，display：0
}

byte moterA = 2;
byte moterB = 3;
byte moterC = 4;
byte moterD = 5;

byte button5 = 13;
byte button4 = 12;
byte button3 = 11;
byte button2 = 10;
byte button1 = 9;


#include "nfc.h"

/** define a nfc class */
NFC_Module nfc;

void setup(void)
{
  pinMode(Max7219_pinCLK, OUTPUT);
  pinMode(Max7219_pinCS, OUTPUT);
  pinMode(Max7219_pinDIN, OUTPUT);
  delay(50);
  Init_MAX7219();


  Serial.begin(115200);
  nfc.begin();
  Serial.println("MF1S50 Reader Demo From Elechouse!");

  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, LOW);

  uint32_t versiondata = nfc.get_version();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");

    pinMode(A0, OUTPUT);
    digitalWrite(A0, HIGH);
    delay(100);
    digitalWrite(A0, LOW);
    while (1); // halt
  }

  pinMode(moterA, OUTPUT);
  pinMode(moterB, OUTPUT);
  pinMode(moterC, OUTPUT);
  pinMode(moterD, OUTPUT);

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);


  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  /** Set normal mode, and disable SAM */
  nfc.SAMConfiguration();

  digitalWrite(A2, LOW);




}

unsigned long times;
int keyNum;
int key = 14173;
int mode = 0;

void loop(void)
{

  if (mode == 1) {
    for (i = 1; i < 9; i++) {
      Write_Max7219(i, disp1[6][i - 1]);
    }

    Serial.println("1");
    if (millis() - times > 3000) {
      mode = 0;
    }
    digitalWrite(A2, HIGH);
    if (digitalRead(button1) == LOW) {
      Serial.println("button1");
      for (i = 1; i < 9; i++) {
        Write_Max7219(i, disp1[1][i - 1]);
      }
      moterTurn(1);
    }
    if (digitalRead(button2) == LOW) {
      Serial.println("button2");
      for (i = 1; i < 9; i++) {
        Write_Max7219(i, disp1[2][i - 1]);
      }
      moterTurn(2);
    }
    if (digitalRead(button3) == LOW) {
      Serial.println("button3");
      for (i = 1; i < 9; i++) {
        Write_Max7219(i, disp1[3][i - 1]);
      }
      moterTurn(3);
    }
    if (digitalRead(button4) == LOW) {
      Serial.println("button4");
      for (i = 1; i < 9; i++) {
        Write_Max7219(i, disp1[4][i - 1]);
      }
      moterTurn(4);
    }
    if (digitalRead(button5) == LOW) {
      Serial.println("button5");
      for (i = 1; i < 9; i++) {
        Write_Max7219(i, disp1[5][i - 1]);
      }
      moterTurn(5);
    }

  } else {
    digitalWrite(A2, LOW);
    u8 buf[32], sta;
    sta = nfc.InListPassiveTarget(buf);
    if (sta && buf[0] == 4) {
      keyNum = 0;
      Serial.print("UUID:");
      nfc.puthex(buf + 1, buf[0]);
      Serial.println();
      if (buf[1] == 123 && buf[2] == 225 && buf[3] == 85 && buf[4] == 5  ) {
        times = millis();
        mode = 1;
      }
    }
  }

}



void moterTurn(int x) {
  mode = 0;
  int speeds = 4;
  int numberLED = x;
  for (int i = 0; i < x; i++) {
    for (int ii = 1; ii < 9; ii++) {
      Write_Max7219(ii, disp1[numberLED][ii - 1]);
    }
    numberLED--;
    for (int iii = 0; iii < 64; iii++) {
      digitalWrite(moterA, HIGH);
      digitalWrite(moterB, LOW);
      digitalWrite(moterC, LOW);
      digitalWrite(moterD, LOW);
      delay(speeds);
      digitalWrite(moterA, LOW);
      digitalWrite(moterB, HIGH);
      digitalWrite(moterC, LOW);
      digitalWrite(moterD, LOW);
      delay(speeds);
      digitalWrite(moterA, LOW);
      digitalWrite(moterB, LOW);
      digitalWrite(moterC, HIGH);
      digitalWrite(moterD, LOW);
      delay(speeds);
      digitalWrite(moterA, LOW);
      digitalWrite(moterB, LOW);
      digitalWrite(moterC, LOW);
      digitalWrite(moterD, HIGH);
      delay(speeds);
    }
  }

  for (int ii = 1; ii < 9; ii++) {
    Write_Max7219(ii, disp1[numberLED][ii - 1]);
  }
  digitalWrite(A2, LOW);
  digitalWrite(moterA, LOW);
  digitalWrite(moterB, LOW);
  digitalWrite(moterC, LOW);
  digitalWrite(moterD, LOW);

}



