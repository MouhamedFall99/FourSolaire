#include <LiquidCrystal.h>
#include <Keypad.h>;
#include <Wire.h>
#include <DS3231.h>
#include <SimpleDHT.h>

DS3231 clock;
RTCDateTime dt;

//Temperature
int pinDHT11 = 7;
SimpleDHT11 dht11(pinDHT11);
//Led pour le four
int ledTmpFour = 8;
int relay_signal = 9;

int temps_cuisson, temperature_sup, temperature_inf, temperature_four, delay_past, delay_present;
String cuisson = "Saisissez le temps de cuisson";
String nom = "SMART OVEN"; //On l'affiche sur le lcd

const int rs = 15, en = 14, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int buzz = 6;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {30, 31, 32, 33}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {34, 35, 36, 37}; //connect to the column pinouts of the keypad *les 4 premiers colonnes

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


void setup() {
  pinMode(buzz, OUTPUT);
  pinMode(ledTmpFour, OUTPUT);
  pinMode(relay_signal, OUTPUT);
  digitalWrite(relay_signal, LOW);
  //Wire.begin();
  Serial.begin(9600);
  input_handle();
}

void loop() {
  // put your main code here, to run repeatedly
  Timer(temps_cuisson);

  //temperature_control(temps_cuisson, temperature_sup, temperature_inf);
  input_handle();
}

void input_handle() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("SMART OVEN");
  delay(2000);
  lcd.clear();
  lcd.print("TEMPS CUISSON");
  temps_cuisson = saisi();
    lcd.print("TEMP INF");   
        temperature_inf = saisi();
    lcd.print("TEMP SUP");   
        temperature_sup = (saisi()-temperature_inf)/2;
}

/*void lcd_display(int temperature_four, int temps_cuisson_restant){

  }*/
void Timer(int temps_cuisson) {
  //On commence la cuisson
  digitalWrite(relay_signal, HIGH);
  Serial.println(temps_cuisson);
  clock.begin();
  int minute = 0;
  // Set sketch compiling time
  clock.setDateTime(__DATE__, __TIME__);
  int start1 = 0;
  int start2 = 0;
  while (1) {
    dt = clock.getDateTime();
    Serial.print(dt.minute); Serial.print(" : ");
    Serial.print(dt.second); Serial.println("");
    delay(1000);
    if (start1 == 0) {
      start1 = dt.minute;
      start2 = dt.second;
    }
      //delay(10000);
     // Serial.println('On va contrôler la températrue');
    temperature_control(temps_cuisson, temperature_sup, temperature_inf);

    if (dt.minute == temps_cuisson + start1 &&  dt.second == start2) {
      digitalWrite(6, HIGH);
      delay(2000);
      digitalWrite(6, LOW);
      //La cuisson est terminé
      digitalWrite(relay_signal, LOW);
      digitalWrite(8, LOW);
      Serial.println("END");
      delay(5000);
      return;
    }
  }
  Serial.println("OUT");
}

void temperature_control(int temps_cuisson, int temperature_sup, int temperature_inf) {
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  lcd.setCursor(0, 0);
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
    return;
  }
  while (1) {
    Serial.println(temperature_sup);
    Serial.println(temperature_inf);
    lcd.print((int) temperature); lcd.println(" c");
    //temperature_four = 0; //ON DOIT LIRE LA TEMPERATURE SUR LE CAPTURE
    Serial.print((int)temperature); Serial.println(" *C, "); delay(5);
    if (temperature >= temperature_inf && temperature <= temperature_sup) {
      //C'EST OK
      //Si le relais est déconnecté on le connecte
      digitalWrite(ledTmpFour, HIGH); delay(2000); //break;
      digitalWrite(relay_signal, HIGH);
    }
    else {
      //On doit déconnecter le relais
      if (temperature > temperature_sup) {
        digitalWrite(ledTmpFour, LOW);
        delay(2000); 
        digitalWrite(relay_signal, LOW);
        
      }

      if (temperature <= temperature_inf) {
        digitalWrite(ledTmpFour, HIGH); delay(2000); 
        digitalWrite(relay_signal, HIGH);
      }
    }
    break;
  }
}


int saisi() {
  int temp = 0;
  String mypassword;
  while (true) {
    char key = customKeypad.getKey();

    if (key) {
      Serial.println(key);
      lcd.setCursor(temp, 1);
      lcd.print(key);

      temp = temp + 1;
      if (temp > 15) {
        lcd.clear();
        mypassword = "";
        temp = 0;
        lcd.print("INCORRECT");
        delay(2000);
        lcd.clear();
        lcd.print("REESSAYER");
        //On appelle  à nouveau la fonction
      }

    }
    if (key == '1') {
      mypassword = mypassword + '1';
    }

    if (key == '2') {
      mypassword = mypassword + '2';
    }

    if (key == '3') {
      mypassword = mypassword + '3';
    }

    if (key == '4') {
      mypassword = mypassword + '4';
    }

    if (key == '5') {
      mypassword = mypassword + '5';
    }

    if (key == '6') {
      mypassword = mypassword + '6';
    }

    if (key == '7') {
      mypassword = mypassword + '7';
    }

    if (key == '8') {
      mypassword = mypassword + '8';
    }

    if (key == '9') {
      mypassword = mypassword + '9';
    }

    if (key == '0') {
      mypassword = mypassword + '0';
    }

    if (key == '*') {
      lcd.clear();
      lcd.print("Succes");
      Serial.println(mypassword.toInt());
      delay(500);
      lcd.clear();
      break;
    }
  }
  return mypassword.toInt();
}
