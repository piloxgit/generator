// Generátor funkcí AD9833

// připojení potřebné knihovny
#include <Arduino.h>
#include <Wire.h>
#include <AD9833.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>

#define pinFsync 10
#define Light 5 
int encoderPin_A = 4;
int encoderPin_B = 3;
int PushButt = 2;

long czastota = 500000;      // ustawienie czestotliwosci startowej
String czestotliwosc;
long krok = 10;             // krok poczatkowy
int sinus = 0x2000;          
int trojkat = 0x2002;
int prostokat = 0x2020;
int typprzebiegu;
int znacznik = 10;


AD9833 gen(pinFsync);
LiquidCrystal_I2C lcd(0x27,16,2);

void obsluga_encodera()
{
 
    if (znacznik == 10)   // znacznik 10 zmiana sygnalu sin/prost/trojk
    {
              int dir = encoder_data();     // sprawdz Encoder
              if(dir > 0)                   // sprawdzam tylko w prawo
              {
                      if(typprzebiegu == sinus)    // jesli jest sin to zmien
                          {typprzebiegu = trojkat; // na trojk
                           delay(80);}             // odczekaj - drgania
 
                      else if (typprzebiegu == trojkat) //jesli trojk ustaw prost
                          {typprzebiegu = prostokat;delay(80);}
                               
                      else if (typprzebiegu == prostokat)//jesli prost ustaw sin
                          {typprzebiegu = sinus;delay(80);}
                      else
                      {}   
              }
              else
              {}
    } //*****************koniec warunku dla znacznik == 10
 
 
 
               
    if (znacznik == 20)    // warunek dla znacznik 20 - zmiana kroku
    {
             int dir = encoder_data();     // sprawdz Encoder
             if(dir > 0)                   // jeśli w prawo to ...
               {
                  if (krok < 1000000)      // jesli krok mniejszy niz 1MHz
                  {krok = krok * 10;delay(100);} // zwieksz krok-mnozenie razy 10
                   
                  else                     // jesli 1MHz nie rob nic
                  {}
               }
             else if (dir <0 )             // jeśli w lewo to ...
                  {
                    if (krok >= 10)        // dodatkowy warunek
                                           // sprawdzenie czy nie dziele 0
                       {krok = krok / 10;  // zmniejsz krok - dzielenie przez 10
                        delay(100); 
                       } 
                        
                    else
                      {krok = 1;delay(100);}          // ustaw krok 1 Hz
                       
                  }
              else
                  {}
       
    } //****************koniec warunku dla znacznik == 20
     
 
    else if (znacznik == 30) // warunek dla znacznik 30 - zmiana czestotliwosci
    {
                 
           int dir = encoder_data();              // sprawdz Encoder
           if(dir > 0)                            // jeśli w prawo to ...
                  {czastota = czastota + krok;}   // zwieksz f o krok
           else if (dir <0 )                      // jeśli w lewo to ...
                if ((czastota-krok) >= 0)
                  {
                  {czastota = czastota - krok;}   // zmniejsz f o krok
                  }
                  else
                  {}
                   
           else
                  {}
     } //********************koniec warunku dla znacznik == 30
 
    else
        {}
   
} //****************koniec procedury obslugi encodera
 
 
 
void obsluga_buttona()
{
   if (digitalRead(PushButt))
   {}                              // nie wcisniety - wyjscie
    
   else
   {                               // wcisniety - ustalam w ktorej
                                   //  pozycji menu i co zmienic 
    if (znacznik == 10)            // jesli znacznik = 10 ...
      {znacznik = 20;delay(120);}  // ... zmien na 20
    else if (znacznik == 20)       // jesli znacznik = 20 ...
      {znacznik = 30;delay(120);}  // ... zmien na 30
    else
      {znacznik = 10;delay(120);}  // jesli nie 10 i nie 20 to musi 
                                   // byc 30 wiec ustaw znow 10
   }
}
 
 
 
 
 
void prezentacja()                 // wyswietlenie wszystkich wartosci
{
  lcd.clear();                     // wyczysc LCD
  if(znacznik == 10)               // znacznik = 10
    {lcd.print("*");}              // ... wyswietl "*" przed 
  else
    {lcd.print(" ");}              // znacznik <> 10 - spacja
 
  if(typprzebiegu == sinus)        // jaki aktualnie mamy przebieg
    {lcd.print("sinus");}            // jesli sinus - wyswietl
  else if (typprzebiegu == prostokat)
    {lcd.print("prost");}            // jesli prostokat - wyswietl
   else if (typprzebiegu == trojkat)
    {lcd.print("trojk");}            // jesli trojkat - wyswietl
   else
    {}
 
  if(znacznik == 20)               // jesli znacznik = 20
    {
    lcd.print(" *");}
  else
    {lcd.print("  ");}
     
  lcd.print("K=");
 
  if(krok == 1000000)
    {
      lcd.print(krok / 1000000);
      lcd.print("MHz");
    }
  else if(krok >= 1000)
    {
      lcd.print(krok / 1000);
      lcd.print("KHz");
    }
  else
  {
    lcd.print(krok);
    lcd.print("Hz");
  }
   
  lcd.setCursor(0, 1);
 
 
  if(znacznik == 30)               // jesli znacznik = 30
    {
        lcd.print("*");
    }
  else
    {
        lcd.print(" ");
    }
 
// ponizej troche zakrecone zabiegi aby na LCD wyswietlana czestotliwosc
// wygladala przyzwoicie - "F=xx.xxx.xxx Hz"
 
 
  czestotliwosc = String(czastota); //zamiana typu danych na string
  int dlugosc = czestotliwosc.length(); // ustalam dlugosc ciagu
   
  if (dlugosc == 6) // jesli dlugosc 6 - mniej niz 1 MHz
     {
       czestotliwosc = "0" + czestotliwosc; // to dopisz zero przed
     }
   else if(dlugosc == 5) // jesli dlugosc 5 - dziesiatki KHz
     {
       czestotliwosc = "00" + czestotliwosc;
     }
   else if(dlugosc == 4) // jesli dlugosc 4 - jednostki KHz
     {
       czestotliwosc = "000" + czestotliwosc;
     }
 
    else if(dlugosc == 3) // jesli dlugosc 3 - setki Hz
     {
       czestotliwosc = "0000" + czestotliwosc;
     }
    else if(dlugosc == 2) // jesli dlugosc 2 - dziesiatki Hz
     {
       czestotliwosc = "00000" + czestotliwosc;
     }
 
    else if(dlugosc == 1) // jesli dlugosc 1 - jednostki Hz
     {
       czestotliwosc = "000000" + czestotliwosc;
     }
    else
    {}
 
 
      
   
  lcd.print(" F=" + czestotliwosc.substring(0,1) + "." + czestotliwosc.substring(1,4) + "." + czestotliwosc.substring(4,7) + " Hz");
   
   
 
}

void setup() {
  
  Serial.begin(9600);
  gen.Begin();
  lcd.init();                      

  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello, world!");
  delay(3000);
  pinMode(PushButt, INPUT);            // konfiguracja pinu PushButtona
  
  Serial.println("Start programu, sinus 1000 Hz");
  gen.EnableOutput(true);
  

  }

void loop() {
  obsluga_buttona();                  // sprawdx button w encoderze
  obsluga_encodera();                 // sprawdz obrot w encoderze
 //WriteFrequencyAD9833(czastota);     // zapisz/odswiez f w AD9833
  prezentacja();                      // wyswietl wszystko na LCD
  delay(100);

  /* nastavení trojúhelníkového signálu
  // o frekvenci 2000 Hz do REG1
  // a jeho zapnutí na výstup
  Serial.println("Trojuhelnik 2000 Hz");
  gen.ApplySignal(TRIANGLE_WAVE, REG1, 2000);
  delay(4000);
  // změna frekvence v REG1 na 1000 Hz
  Serial.println("Zmena trojuhelniku na 1000 Hz");
  gen.SetFrequency(REG1, 1000);
  delay(4000);
  // přepnutí zpět na REG0
  Serial.println("Prepnuti zpet na 1000 Hz sinus");
  gen.SetOutputSource(REG0);
  delay(4000);
  // nastavení REG0 na obdélníkový signál
  // o frekvenci 1500 Hz
  Serial.println("Zmena na obdelnik 1500 Hz");
  gen.SetWaveform(REG0, SQUARE_WAVE);
  gen.SetFrequency(REG0, 1500);
  delay(4000);
  // nastavení REG0 na půlobdélníkový signál
  // o frekvenci 1000 Hz
  Serial.println("Zmena na pul-obdelnik 1000 Hz");
  gen.SetWaveform(REG0, HALF_SQUARE_WAVE);
  gen.SetFrequency(REG0, 1000);
  delay(4000);
  // nastavení REG0 na sinusový signál
  // o frekvenci 1000 Hz
  Serial.println("Zmena na sinus 1000 Hz");
  gen.SetWaveform(REG0, SINE_WAVE);
  delay(4000);
  // postupné zvyšování fázového posuvu od 0 do 180 stupňů
  Serial.println("Zvysovani frekvence z 1000 na 4000 Hz");
  for (int i = 0; i < 30; i++) {
    gen.IncrementFrequency(REG0, 100);
    delay(100);
  }
  delay(1000);
  // nastavení fázového posuvu na 180 stupňů
  Serial.println("Nastaveni fazoveho posuvu na 180 stupnu");
  gen.SetPhase(REG1, 180);
  delay(4000);
  // reset modulu pro získání výchozího stavu
  Serial.println("Reset modulu");
  gen.Reset();
  delay(4000);*/
}
