#include <PlayRtttl.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

float vers = 1.0;

SoftwareSerial mySoftwareSerial(5, 6); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

const int Itel1 = 2;    // Telefon 1 Auswertung Gabel und Wählpulse
const int OAudio = 4; // Signalausgabe auf die unbeschaltenen Telefone (nicht, wenn auf internen AB Leitungen)

bool forkup = false; //ist die Gabel abgehoben
bool last_forkstate = HIGH; // letzter Zustand der Gabel
bool action_completed = true; // Prüfen, ob eine fertigstellung der Auswertung offen ist (Wählen / Gabel)
int dial_counter = 0;

// Timings
int pulse = 60; //ms
int pulse_min = pulse*0.9; //ms
int pulse_max = pulse*1.1; //ms
int pause = 40; //ms
int pause_min = pause*0.9; //ms
int pause_max = pause*1.1; //ms
int period_max = 10* (pulse_max + pause_max);

//DF Player Settings
int df_vol = 10;

unsigned long lastchange = 0; //last time fork changed


void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Telefonzentrale ");
  Serial.println (vers);
  Serial.print("_|‾|_ Pulsmin: ");
  Serial.print (pulse_min);
  Serial.print(" Pulsmax: ");
  Serial.print (pulse_max);
  Serial.print("       ‾|_|‾ Pausemin: ");
  Serial.print (pause_min);
  Serial.print(" Pausemax: ");
  Serial.print (pause_max);
  Serial.print(" Periodemax: ");
  Serial.println (period_max);

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  myDFPlayer.volume(df_vol);
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);

  pinMode(Itel1, INPUT);
}


void forkstatechange(bool state){
  // Alle vorgängigen Aktionen sind abgeschlossen. Also wird die Gabel neu abgehoben.
  if (forkup == false && state == LOW && action_completed == true){
    forkup = true;
    last_forkstate = LOW;
    Serial.println("Hörer abgehoben");
    signal_line();
  }

  // Der Hörer war abgenommen, also wurde entweder aufgelegt oder es wird gewählt
  if (forkup == true){
    
    // Alle vorgängigen Aktionen waren beendet, entweder ist es der erste Wählimpuls oder es wurde aufgelegt
    if (state == HIGH && action_completed == true){
      // DF Player reset - entweder will jemand einen neuen Track oder hat aufgehängt.
      myDFPlayer.reset();
      // Falls ein Signalton ausgegeben wurde, beenden
      noTone(OAudio);
      lastchange = millis();
      last_forkstate = HIGH;
      action_completed = false;
      //Serial.print("Gabel aufgelegt oder Wählvorgang, weiter beobachten ...");
    }
    //Aktion ist nicht beendet und es folgt ein neuer Impuls, also ist ein Wählvorgang im Gange
    else if (state == LOW && action_completed == false){
      lastchange = millis();
      last_forkstate = LOW;
      action_completed = false;
      // Zähler wird auf fallender Flanke erhöht um das auflegen detektieren zu können
      dial_counter += 1; // Addiere eins zum Zähler
      //Serial.print(" ");
      //Serial.print(dial_counter);
    }   
    //Aktion ist nicht beendet und es folgt ein neuer Impuls, also ist ein Wählvorgang im Gange
    else if (state == HIGH && action_completed == false){
      lastchange = millis();
      last_forkstate = HIGH;
      action_completed = false;
    } 
  }
}

void check_action(){
  // Hier lande ich nur, wenn innert 1 Pulslänge nichts mehr passiert ist.
  // Entweder wurde also die Gabel aufgehängt wurde oder beim letzten Wählimpuls 

  //Steht der Counter noch bei 0 so wurde die Gabel aufgehängt
  if (dial_counter == 0){
    stopPlayRtttl();
    forkup = false;
    last_forkstate = HIGH;
    Serial.println("Hörer aufgelegt");
  }
  else{
    if(dial_counter==10){dial_counter=0;}
    play_sound(dial_counter);
    Serial.print("Nummer: ");
    Serial.println(dial_counter);
    dial_counter = 0;
  }
  action_completed = true;    
  lastchange = 0;
}

void play_sound(int track){
  switch(track){
    case 0: myDFPlayer.play(10);break;
    case 1: myDFPlayer.play(1);break;
    case 2: myDFPlayer.play(2);break;
    case 3: myDFPlayer.play(3);break;
    case 4: myDFPlayer.play(4);break;
    case 5: myDFPlayer.play(5);break;
    case 6: myDFPlayer.play(6);break;
    case 7: myDFPlayer.play(7);break;
    case 8: myDFPlayer.play(8);break;
    case 9: myDFPlayer.play(9);break;
//    case 1: playRtttlBlockingPGM(OAudio, Bond);break;
//    case 2: playRtttlBlockingPGM(OAudio, YMCA);break;
//    case 3: playRtttlBlockingPGM(OAudio, Smurfs);break;
//    case 4: playRtttlBlockingPGM(OAudio, Short);break;
//    case 5: playRtttlBlockingPGM(OAudio, YMCA);break;
//    case 6: playRtttlBlockingPGM(OAudio, YMCA);break;
//    case 7: playRtttlBlockingPGM(OAudio, YMCA);break;
//    case 8: playRtttlBlockingPGM(OAudio, YMCA);break;
//    case 9: playRtttlBlockingPGM(OAudio, YMCA);break;
//    case 0: playRtttlBlockingPGM(OAudio, YMCA);break;
  }
}

void signal_nocard(){
  tone(OAudio, 880, 1000);
}

void signal_nofile(){
  tone(OAudio, 880, 3000);
}

void signal_line(){
  tone(OAudio,440);
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          signal_nocard();
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          signal_nofile();
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}
void loop() {
  bool forkstate = digitalRead(Itel1);

  //Serial.print(forkstate);
  if (forkstate != last_forkstate){
    forkstatechange(forkstate);
  }
  if (action_completed == false && millis() > (lastchange + pulse_max)){
    check_action();
  }
    
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  delay(1);
  }
