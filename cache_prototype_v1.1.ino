#include <SPI.h>

//MOSI:  pin 11
//MISO:  pin 12
//SCK:   pin 13
//CLKIN: pin 7
//A:     pin 6
//B:     pin 5

byte shift_t, shift_r;
int adrs[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}}; //Memory Locations
int CI = 7; //Clock Inhibit
int A = 6;  //Select line A
int B = 5;  //Select line B

//Write a byte to register
void Write(char a, char temp) {
  Serial.print("Data Written: ");
  ChipSelect(a);
  digitalWrite(CI, HIGH);
  shift_t = temp;
  shift_r = SPI.transfer(shift_t);
  digitalWrite(CI, LOW);
  char t = shift_r;
  Serial.print(t); Serial.print(" "); Serial.println(shift_t, BIN);
  Serial.println("------------------------------");
}

//Read a byte from register
void Read(char a) {
  Serial.print("Data Read: ");
  ChipSelect(a);
  digitalWrite(CI, HIGH);
  shift_r = SPI.transfer(0);
  SPI.transfer(shift_r);
  digitalWrite(CI, LOW);
  char t = shift_r;
  Serial.print(t);  Serial.print(" ");  Serial.println(shift_r);  Serial.print(" ");  Serial.println(shift_r, BIN);
  Serial.println("------------------------------");
}

//Clear the content of register
void Clear(char a) {
  Serial.println("Data Cleared ...");
  ChipSelect(a);
  digitalWrite(CI, HIGH);
  shift_t = NULL;
  shift_r = SPI.transfer(shift_t);
  digitalWrite(CI, LOW);
  Serial.println("------------------------------");
}

//Writes 4 bytes at once
void BufferWrite(char a, char b, char c, char d) {
  MasterReset();
  char x[4] = {a, b, c, d};
  Serial.println("Buffer Write:");
  for (int i = 0; i < 4; i++) {
    digitalWrite(B, adrs[i][0]);
    digitalWrite(A, adrs[i][1]);
    digitalWrite(CI, HIGH);
    SPI.transfer(x[i]);
    Serial.print(x[i]); Serial.print(" "); Serial.println(x[i], BIN);
    digitalWrite(CI, LOW);
  }
  Serial.println("------------------------------");
}

//Reads 4 bytes at once
void BufferRead() {
  Serial.println("Buffer Read:");
  for (int i = 0; i < 4; i++) {
    digitalWrite(B, adrs[i][0]);
    digitalWrite(A, adrs[i][1]);
    digitalWrite(CI, HIGH);
    shift_r = SPI.transfer(0);
    SPI.transfer(shift_r);
    char t = shift_r;
    Serial.print(t); Serial.print(" "); Serial.print(shift_r); Serial.print(" "); Serial.println(shift_r, BIN);
    digitalWrite(CI, LOW);
  }
  Serial.println("------------------------------");
}

//Reset all registers
void MasterReset(void) {
  Serial.println("Master Reset");
  for (int i = 0; i < 4; i++) {
    digitalWrite(A, adrs[i][0]);
    digitalWrite(B, adrs[i][1]);
    digitalWrite(CI, HIGH);
    SPI.transfer(0);
    digitalWrite(CI, LOW);
  }
  Serial.println("------------------------------");
}

//Select lines for MUX and DMUX
void ChipSelect(char a) {
  if (a == '1') {
    digitalWrite(A, LOW);
    digitalWrite(B, LOW);
  }
  else if (a == '2') {
    digitalWrite(A, HIGH);
    digitalWrite(B, LOW);
  }
  else if (a == '3') {
    digitalWrite(A, LOW);
    digitalWrite(B, HIGH);
  }
  else if (a == '4') {
    digitalWrite(A, HIGH);
    digitalWrite(B, HIGH);
  }
}

//Initialize cache
void setup() {
  Serial.begin(9600);
  //  SPI.setClockDivider(SPI_CLOCK_DIV4); //Clock Speed W-8MHz, R-4-MHz
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();
  MasterReset();
  Serial.println("W  -> Write\nR  -> Read\nC  -> Clear\nBW -> Buffer Write\nBR -> Buffer Read\nM  -> Master Rest"); //Commands format
  Serial.println("------------------------------");
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(CI, OUTPUT);
  digitalWrite(CI, LOW);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
}

//Input loop
void loop() {
  if (Serial.available())
  {
    String ch = Serial.readStringUntil('\n');
    if (ch[0] == 'w' or ch[0] == 'W')
    {
      Write(ch[2], ch[4]);
    }
    else if (ch[0] == 'r' or ch[0] == 'R')
    {
      Read(ch[2]);
    }
    else if (ch[0] == 'c' or ch[0] == 'C')
    {
      Clear(ch[2]);
    }
    else if (ch[0] == 'm' or ch[0] == 'M')
    {
      MasterReset();
    }
    else if ((ch[0] == 'b' and ch[1] == 'w') or (ch[0] == 'B' and ch[1] == 'W'))
    {
      BufferWrite(ch[3], ch[4], ch[5], ch[6]);
    }
    else if ((ch[0] == 'b' and ch[1] == 'r') or (ch[0] == 'B' and ch[1] == 'R'))
    {
      BufferRead();
    }
    else
    {
      Serial.println("Invalid Command !!!");
      Serial.println("------------------------------");
    }
  }
}
