#include <MIDI.h>
#include <EEPROM.h>

int numberLDRs;
int boundary = 25;
int Low[8];
int LDRs[] = {A0, A1, A2, A3, A4, A5, A6, A7};
int notes[] = {523, 587, 659, 698, 784, 880, 988, 1047};
int lastPlay[] = {};
long flagUnique = 0;
long flagCount = EEPROM.read(0);
long onCount = EEPROM.read(1);
long cycleCount = 0;

MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  onCount++;
  numberLDRs = sizeof(LDRs);
  for (int i = 0; i < numberLDRs; i++)
  {
    lastPlay[i] = 0;
  }
  Callibrate();
  MIDI.begin();
  Serial.begin(115200);
  tone(9, 700);
  delay(1000);
  noTone(9);
}

void loop() {
  cycleCount++;
  for (int i = 0; i < numberLDRs; i++)
  {
    if (analogRead(LDRs[i]) < Low[i] - boundary)
    {
      Serial.print(i);
      MIDI.sendNoteOn(notes[i], 127, 1);
      tone(9, notes[i]);
      lastPlay[i] = 1;
      delay(100);
    }
    else if (analogRead(LDRs[i]) >= Low[i] - boundary)
    {
      Serial.print("_");
      lastPlay[i] = 0;
    }
  }
  noTone(9);
  Serial.println();
  if (statusCheck())
  {
    flagCount++;
    flagUnique++;
    Callibrate();
  }
  Update(cycleCount, flagCount, onCount);
}

void Callibrate()
{
  Serial.println("*********************Callibrated**********************");
  for (int i = 0; i < numberLDRs; i++)
  {
    pinMode(i, INPUT);
    Low[i] = analogRead(LDRs[i]);
    Serial.print(Low[i]);
    Serial.print("  ");
  }
  Serial.println();
}

boolean statusCheck()
{
  bool flag = 0;
  for (int i = 0; i < numberLDRs; i++)
  {
    if (lastPlay[i] == 1)
    {
      flag = true;
      Serial.println("******************FLAG HIGH*********************");
    }
    else
    {
      flag = false;
    }
  }
  return flag;
}
void Update(long count, long flag, long on)
{
  if (count % 100 == 0)
  {
    EEPROM.write(0, flag);
    EEPROM.write(1, on);
  }
  if (flag / on < flagUnique)
  {
    boundary++;
    Serial.println("************BOUNDARY CHANGED**************");
  }
}
