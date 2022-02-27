/*
   arduino SPI communication to L6470
   Deng Hongchao 20210608
*/
#include<SPI.h>
#include<L6470.h>
#include<MsTimer2.h>  // for running an interrupt function

#define SS1  10  // define the chip select pin
#define SS2  9
#define MOSI_PIN  11 // the SPI serial function
#define MISO_PIN 12
#define SCLK_PIN  13

String inputString = "";
boolean stringComplete;
unsigned long absoluteposition;
int k = 2000;
long buffer;

String cmdcode;
int param;
String param_hex;
unsigned long input_value;
unsigned long output_value;
unsigned char dir;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SS1, OUTPUT);
  //  digitalWrite(SS1,HIGH);
  //  pinMode(SS2,OUTPUT);
  //  digitalWrite(SS2,HIGH);

  pinMode(MISO_PIN, INPUT);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(SCLK_PIN, OUTPUT);


  SPI.begin();
  SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE3));

  L6470_resetdevice(SS1); // we suggest to use 1 Arduino nano to control 1 L6470 
  L6470_init(SS1);
  STEPPING_PUMP_INIT(SS1);
  //  L6470_resetdevice(SS2);
  //  L6470_init(SS2);
  //  STEPPING_PUMP_INIT(SS2);
  //  STEPPING_PUMP_SETUP(SS1);
    SetMAXSPEED(SS1,0x20);

  delay(500);
  Serial.println("start to run");

  //set initial value for drive
  //  L6470_setparam(SS1,KVAL_HOLD , 0xFF);
//    L6470_setparam(SS1,KVAL_RUN , 0xFF);
//    L6470_setparam(SS1,KVAL_ACC , 0xFF);
//    L6470_setparam(SS1,KVAL_DEC , 0xFF);
  //  L6470_setparam(SS1,FS_SPD , 0x3FF);
//  SetSTEPMODE(SS1, 0x07);
  ShowAllParams(SS1);  // output current parameters in L6470
  L6470_hardhiz(SS1);  // hard stop, high Z state
  //  L6470_hardhiz(SS2);

  MsTimer2::set(3000, Currentspeed);
    MsTimer2::start(); // enable the MsTimer2 function to output parameters periodically
  
}

void loop() {
 if (stringComplete) { // identify the received command
    stringComplete = false; // flag bit, turn false to be ready for next serial input 
    Serial.print("the input string is: ");
    Serial.println(inputString);
    cmdcode = inputString.substring(0,3);
     
    if (cmdcode == "nop"){ // empty command
      Serial.println("nop\n");
      Serial.println();
      
    }else if (cmdcode == "spm"){ // set parameter
      Serial.println("setparam");
      param = strtol(inputString.substring(3,5).c_str(),NULL,16); // 必须要通过c_str()函数提取string的地址，但是不建议通过c_str()直接修改原始string
      input_value = strtol(inputString.substring(5).c_str(),NULL,16);
      Serial.print("Address 0x");
      Serial.print(param,HEX);
      Serial.print("; The new value is 0x");
      Serial.println(input_value,HEX);
      L6470_setparam(SS1,param,input_value);
      Serial.println();
      
    }else if (cmdcode == "gpm"){ // get parameter
      Serial.println("getparam");
      param = strtol(inputString.substring(3,5).c_str(),NULL,16);
      output_value = L6470_getparam(SS1,param);
      Serial.print("Address 0x");
      Serial.print(param,HEX);
      Serial.print(" value is: 0x");
      Serial.println(output_value,HEX);
      Serial.println();
      
    }else if (cmdcode == "run"){ // run, with direction
      Serial.println("run");
      dir = strtol(inputString.substring(3,4).c_str(),NULL,16);
//      input_value = strtol(inputString.substring(4).c_str(),NULL,16);
      input_value = inputString.substring(4).toInt();
      L6470_run(SS1,dir,input_value);
      Serial.println();
      
    }else if (cmdcode == "stp"){ 
      Serial.println("stepclock");
      
    }else if (cmdcode == "mov"){ // move, with direction
      Serial.println("move");
      dir = strtol(inputString.substring(3,4).c_str(),NULL,16);
      input_value = inputString.substring(4).toInt();
      L6470_move(SS1,dir,input_value);
      Serial.println();
      
    }else if (cmdcode == "got"){
      Serial.println("goto");
    }else if (cmdcode == "gtd"){
      Serial.println("goto_dir");
      
    }else if (cmdcode == "gou"){ // go until, with direction
      Serial.println("gountil");
      dir = strtol(inputString.substring(3,4).c_str(),NULL,16);
      input_value = inputString.substring(4).toInt();
      L6470_gountil(SS1,0,dir,input_value); // gountil command use step/sec command
      Serial.println();
      
    }else if (cmdcode == "rsw"){ // release switch
      Serial.println("release switch");
      dir = strtol(inputString.substring(3,4).c_str(),NULL,16);
      L6470_releasesw(SS1,0,dir);
      Serial.println();
      
    }else if (cmdcode == "goh"){
      Serial.println("gohome");
    }else if (cmdcode == "gom"){
      Serial.println("gomark");
    }else if (cmdcode == "rsp"){
      Serial.println("resetpos");
      
    }else if (cmdcode == "rsd"){ // reset device
      Serial.println("resetdevice");
      L6470_resetdevice(SS1);
      Serial.println();
      
    }else if (cmdcode == "ssp"){ // soft stop
      Serial.println("softstop");
      L6470_softstop(SS1);
      Serial.println();
      
    }else if (cmdcode == "hsp"){ // hard stop
      Serial.println("hardstop");
      L6470_hardstop(SS1);
      Serial.println();
      
    }else if (cmdcode == "shz"){ // soft high Z
      Serial.println("softhiz");
      L6470_softhiz(SS1);
      Serial.println();

    }else if (cmdcode == "hhz"){ // hard high Z
      Serial.println("hardhiz");
      L6470_hardhiz(SS1);
      
    }else if (cmdcode == "get"){ // get status of L6470
      Serial.println("getstatus");
      output_value = L6470_getstatus(SS1);
      Serial.print("Current status is ");
      Serial.println(output_value,HEX);
      Serial.println();
      
    }else if (cmdcode == "sal"){ // show all parameters
      Serial.println("ShowAllParams");
      ShowAllParams(SS1);
      Serial.println();  
      
    }else if (cmdcode == "pos"){ // output absolute position
      Serial.print("abs_position: ");
      Serial.println(L6470_getparam(SS1, ABS_POS));
      Serial.println();
    }else if (cmdcode == "gts"){ // get status without clear alarm
      Serial.print("get status without clear it: ");
      Serial.println(L6470_getparam(SS1, STATUS), HEX);
      Serial.println();
    }else if (cmdcode == "con"){ // config
      Serial.print("config: ");
      Serial.println(L6470_getparam(SS1, CONFIG), HEX);
      Serial.println();    
    }else if (cmdcode == "rst"){ // reset all
      Serial.println("Reset all");
      L6470_resetdevice(SS1);
      L6470_init(SS1);
      STEPPING_PUMP_INIT(SS1);
      Serial.println();  
    }else if (cmdcode == "ini"){ // initialization function
      Serial.println("Stepping pump initialize");
      STEPPING_PUMP_SETUP(SS1);
      Serial.println();  
    }
    
    else{
      Serial.println("not a suitable input\n");
    }
    
    
    inputString = "";
 }

}


void serialEvent() { // receive the command from serial port
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true; // set the flag bit to be true
    } else{ // add it to the inputString:
      inputString += inChar;
    }
  }
}



void ShowAllParams(int port) { // additional function besides the functions in L6470 library
  Serial.print("01="); Serial.println(L6470_getparam(port, ABS_POS), HEX);
  Serial.print("02="); Serial.println(L6470_getparam(port, EL_POS), HEX);
  Serial.print("03="); Serial.println(L6470_getparam(port, MARK), HEX);
  Serial.print("04="); Serial.println(L6470_getparam(port, SPEED), HEX);
  Serial.print("05="); Serial.println(L6470_getparam(port, ACCELERATE), HEX);
  Serial.print("06="); Serial.println(L6470_getparam(port, DECELERATE), HEX);
  Serial.print("07="); Serial.println(L6470_getparam(port, MAX_SPEED), HEX);
  Serial.print("08="); Serial.println(L6470_getparam(port, MIN_SPEED), HEX);
  Serial.print("15="); Serial.println(L6470_getparam(port, FS_SPD), HEX);
  Serial.print("09="); Serial.println(L6470_getparam(port, KVAL_HOLD), HEX);
  Serial.print("0A="); Serial.println(L6470_getparam(port, KVAL_RUN), HEX);
  Serial.print("0B="); Serial.println(L6470_getparam(port, KVAL_ACC), HEX);
  Serial.print("0C="); Serial.println(L6470_getparam(port, KVAL_DEC), HEX);
  Serial.print("0D="); Serial.println(L6470_getparam(port, INT_SPD), HEX);
  Serial.print("0E="); Serial.println(L6470_getparam(port, ST_SLP), HEX);
  Serial.print("0F="); Serial.println(L6470_getparam(port, FN_SLP_ACC), HEX);
  Serial.print("10="); Serial.println(L6470_getparam(port, FN_SLP_DEC), HEX);
  Serial.print("11="); Serial.println(L6470_getparam(port, K_THERM), HEX);
  Serial.print("12="); Serial.println(L6470_getparam(port, ADC_OUT), HEX);
  Serial.print("13="); Serial.println(L6470_getparam(port, OCD_TH), HEX);
  Serial.print("14="); Serial.println(L6470_getparam(port, STALL_TH), HEX);
  Serial.print("16="); Serial.println(L6470_getparam(port, STEP_MODE), HEX);
  Serial.print("17="); Serial.println(L6470_getparam(port, ALARM_EN), HEX);
  Serial.print("18="); Serial.println(L6470_getparam(port, CONFIG), HEX);
  Serial.print("19="); Serial.println(L6470_getparam(port, STATUS), HEX);
}

void Currentspeed() { // output the parameters that we concern
    unsigned long speednow = L6470_getparam(SS1,SPEED) & 0x0FFFFF;
    Serial.print("speed: ");
    Serial.print(speednow);
  //  byte stepmode = L6470_getparam(SS1,STEP_MODE) & STEP_MODE_STEP_SEL_MSK;
  //  Serial.print("  step_mode: ");
  //  Serial.print(stepmode);
  Serial.print("  abs_position: ");
  Serial.println(L6470_getparam(SS1, ABS_POS));
  //  Serial.print("maxspeed: ");
  //  Serial.print(L6470_getparam(SS1,MAX_SPEED));
  //  Serial.print("  status: ");
  //  Serial.println(L6470_getparam(SS1,STATUS), HEX);
}
