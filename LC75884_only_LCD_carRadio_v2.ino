/**********************************************************************/
/*This code is not clean and OFFCOURSE will be possible do it better! */ 
/*this is only a group of functions to be used as independent code,  */ 
/*and by this way will be possible to any person can do the changes   */ 
/*of code and see what happen.                                         */
/*The code don't depend of any external library or extenal functions  */
/*complicated.                                                         */
/*I'm let the values to sent as binnary, this allow swap bit by bit */
/* to is possible test segment by segment without convert to HEX    */
/**********************************************************************/
#define VFD_in 7  // This is the pin number 7 on Arduino UNO
#define VFD_clk 8 // This is the pin number 8 on Arduino UNO
#define VFD_ce 9 // This is the pin number 9 on Arduino UNO

//ATT: On the Uno and other ATMEGA based boards, unsigned ints (unsigned integers) are the same as ints in that they store a 2 byte value.
//Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647.

//*************************************************//
void setup() {
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_in, OUTPUT);
  pinMode(VFD_ce, OUTPUT);
 
  pinMode(13, OUTPUT);
  //Serial.begin(115200); // only to debug
}
/*********************************************************************/
// I h've created 2 functions to send bit's, one with strobe, other without strobe.
void send_char_without(unsigned char a){
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
  }
}
/*********************************************************************/
void send_char_8bit_stb(unsigned char a){
 unsigned char transmit = 15; //define our transmit pin
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
 int i = -1;
  data=a;
  // This lines is because the clk have one advance in data, see datasheet of sn74HC595
  // case don't have this signal instead of "." will se "g"
  for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
   i++;
   digitalWrite(VFD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(VFD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(VFD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(VFD_clk,HIGH);// 
    delayMicroseconds(1);
    if (i==7){
    //Serial.println(i);
    digitalWrite(VFD_ce, HIGH);
    delayMicroseconds(1);
    }  
  }
}
/*******************************************************************/
void allON_LC75884(){
  unsigned char group=0;
//send total of 72 bits, the last 2 bits refer the message group;
       for(int i=0; i<4;i++){   // Do until 220 bits
            group=i;
          digitalWrite(VFD_ce, LOW); //
          delayMicroseconds(1);
          send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
          delayMicroseconds(1);
          // On the 75884 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
         
          send_char_without(0B11111111);  send_char_without(0B11111111); //   0:8     
          send_char_without(0B11111111);  send_char_without(0B11111111); //  16:24    
          send_char_without(0B11111111);  send_char_without(0B11111111); //  32:40     
          send_char_without(0B11111111);  send_char_without(0B00000000); //  48:56      
              switch (group){ //-120:127//Last 3 bits is "DD" data direction, and is used
                case 0: send_char_without(0B00000000); break;
                case 1: send_char_without(0B10000000); break;
                case 2: send_char_without(0B01000000); break;
                case 3: send_char_without(0B11000000); break;
              }
          
      // to mark the 4 groups of 220 bits, 00, 01, 10, 11.
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); // 
      delayMicroseconds(1);
      }
}
/**************************************************************************/
void allOFF_LC75884(){
unsigned char group=0;
//send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
//The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
       for(int i=0; i<4;i++){ // Dx until 600 bits
            group=i;
        digitalWrite(VFD_ce, LOW); //
        delayMicroseconds(1);
        send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
        delayMicroseconds(1);
      // On the 75884 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
         
          send_char_without(0B00000000);  send_char_without(0B00000000); //   0:8      
          send_char_without(0B00000000);  send_char_without(0B00000000); //  16:24    
          send_char_without(0B00000000);  send_char_without(0B00000000); //  32:40     
          send_char_without(0B00000000);  send_char_without(0B00000000); //  48:56      
              switch (group){ //63:71// Last 3 bits is "DD" data direction, and is used
                case 0: send_char_without(0B00000000); break;
                case 1: send_char_without(0B10000000); break;
                case 2: send_char_without(0B01000000); break;
                case 3: send_char_without(0B11000000); break;
              }
      // to mark the 4 groups of 120 bits, 000, 001, 010, 011.
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); // 
      delayMicroseconds(1);
      }
}
/**************************************************************************/
void tstSegment_LC75884(){
unsigned char group=0;
//send total of 64 bits, the 4 last bits belongs to set of DR, SC, BU, etc;
//The p0, p1, p2 & p3 at 0, means all pins from s1 to s12 will belongs to segments, other's settings tell will px is a port general purpose!
       for(int i=0; i<4;i++){ // Dx until 600 bits
            group=i;
        digitalWrite(VFD_ce, LOW); //
        delayMicroseconds(1);
        send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
        delayMicroseconds(1);
      // On the 75884 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
         
          send_char_without(0B00000000);  send_char_without(0B00000000); //   0:8      
          send_char_without(0B00000000);  send_char_without(0B00000000); //  16:24    
          send_char_without(0B00000000);  send_char_without(0B00000000); //  32:40     
          send_char_without(0B00000000);  send_char_without(0B00000000); //  48:56      
              switch (group){ //63:71// Last 3 bits is "DD" data direction, and is used
                case 0: send_char_without(0B00000000); break;
                case 1: send_char_without(0B10000000); break;
                case 2: send_char_without(0B01000000); break;
                case 3: send_char_without(0B11000000); break;
              }
      // to mark the 4 groups of 120 bits, 000, 001, 010, 011.
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); // 
      delayMicroseconds(1);
      }
}
void segmentOneByOne(void) {
  byte AA = 0x00;
  byte AB = 0x00;
  byte AC = 0x00;
  byte AD = 0x00;

  unsigned long n = 0b10000000000000000000000000000000;
    for(int i=0; i<4; i++){ // Do until send a total of 4 blocks with 56 bits
        for(int m = 0; m < 32; m++) {
        digitalWrite(VFD_ce, LOW); //
        delayMicroseconds(1);
        send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
        delayMicroseconds(1);
      // On the 75884 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)

     AA= (n & 0x000000ffUL) >>  0;
     AB= (n & 0x0000ff00UL) >>  8;
     AC= (n & 0x00ff0000UL) >> 16;
     AD= (n & 0xff000000UL) >> 24;
           
      //Serial.print(AD, HEX);Serial.print(", "); Serial.print(AC, HEX); Serial.print(", "); Serial.print(AB, HEX); Serial.print(", "); Serial.print(AA, HEX); Serial.print(", nº: "); Serial.println(m, HEX);
      if (n >= 2147483648; n >>= 1) { //iterate through bit mask  //(2147483648) 
      }
      else{
        n = 0b10000000000000000000000000000000;// Length of 4 bytes (32 bit)
        delay(200);
      }
          if (m <2){
          send_char_without((AA));  send_char_without((AB)); //   0:8      
          send_char_without((AC));  send_char_without((AD)); //  16:24   
          send_char_without(0B00000000);  send_char_without(0B00000000); //  32:40     
          send_char_without(0B00000000);  send_char_without(0B00000000); //  48:56      
          }
          else{
          send_char_without(0B00000000);  send_char_without(0B00000000); //0:8     
          send_char_without(0B00000000);  send_char_without(0B00000000); //16:24   
          send_char_without((AA));  send_char_without((AB)); //  32:40         
          send_char_without((AC));  send_char_without((AD)); //  48:56    
          }
              switch (i){ //63:71// Last 3 bits is "DD" data direction, and is used
                case 0: send_char_without(0B00000000); break;
                case 1: send_char_without(0B10000000); break;
                case 2: send_char_without(0B01000000); break;
                case 3: send_char_without(0B11000000); break;
              }
      // to mark the 4 groups of 120 bits, 000, 001, 010, 011.
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); // 
      delayMicroseconds(1);
      delay(150); // To allow see the segment on state during test!
      }
      delay(150); // To allow see the segment on state during test!
       allOFF_LC75884();
    }
}
/**************************************************************************/
void HiFolks0(){
//send total of 72 bits
          digitalWrite(VFD_ce, LOW); //
          delayMicroseconds(1);
          send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
          delayMicroseconds(1);
          // On the 75884 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
         
          send_char_without(0B00000000);  send_char_without(0B00000000); //   0:8     
          send_char_without(0B00000000);  send_char_without(0B00000000); //  16:24    
          send_char_without(0B00000000);  send_char_without(0B00000000); //  32:40     
          send_char_without(0B00000000);  send_char_without(0B00000000); //  48:56      
              
          send_char_without(0B00000000); //msg:00
               
      // to mark the 5 groups of 220 bits, 00, 01, 10, 11.
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); // 
      delayMicroseconds(1);
}
void HiFolks1(){
//send total of 72 bits
          digitalWrite(VFD_ce, LOW); //
          delayMicroseconds(1);
          send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
          delayMicroseconds(1);
          // On the 75884 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
         
          send_char_without(0B00000000);  send_char_without(0B11100000); //   0:8     
          send_char_without(0B00000000);  send_char_without(0B00001110); //  16:24    
          send_char_without(0B01000000);  send_char_without(0B00000000); //  32:40     
          send_char_without(0B00000000);  send_char_without(0B00000000); //  48:56      
          send_char_without(0B10000000); 
          
      // to mark the 4 groups of 220 bits, 00, 01, 010, 011.
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); // 
      delayMicroseconds(1);
}
void HiFolks2(){
//send total of 72 bits
          digitalWrite(VFD_ce, LOW); //
          delayMicroseconds(1);
          send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
          delayMicroseconds(1);
          // On the 75884 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
         
          send_char_without(0B11100000);  send_char_without(0B00000001); //   0:8     
          send_char_without(0B10100100);  send_char_without(0B00001001); //  16:24    
          send_char_without(0B10101010);  send_char_without(0B00001000); //  32:40     
          send_char_without(0B00000000);  send_char_without(0B00000000); //  48:56      
              
           send_char_without(0B01000000); 
          
      // to mark the 4 groups of 220 bits, 00, 01, 10, 11.
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); // 
      delayMicroseconds(1);
}
void HiFolks3(){
//send total of 72 bits
          digitalWrite(VFD_ce, LOW); //
          delayMicroseconds(1);
          send_char_8bit_stb(0B01000010); //(0x42) firts 8 bits is address, every fixed as (0B01001011), see if clk finish LOW or HIGH Very important!
          delayMicroseconds(1);
          // On the 75884 the message have first 16*8 bits more 8 times to performe 128 bits(last byte is control: 0BXXX00000)
         
          send_char_without(0B11100000);  send_char_without(0B10100000); //   0:8     
          send_char_without(0B01100000);  send_char_without(0B00001001); //  16:24    
          send_char_without(0B00001100);  send_char_without(0B00000000); //  32:40     
          send_char_without(0B00000000);  send_char_without(0B00000000); //  48:56      
              
          send_char_without(0B11000000);
           
      // to mark the 4 groups of 220 bits, 00, 01, 10, 11.
      delayMicroseconds(1);
      digitalWrite(VFD_ce, LOW); // 
      delayMicroseconds(1);
}

void loop() {
  for(int i=0; i< 5; i++){
    allON_LC75884(); // All on
    delay(300);
    allOFF_LC75884(); // All off
    delay(300);
  }

segmentOneByOne();

HiFolks0();  //Block 00
HiFolks1();  //Block 01
HiFolks2();  //Block 02
HiFolks3();  //Block 03

delay(2000);
}
