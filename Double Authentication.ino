#include<MFRC522.h>
#include<SPI.h>

#define SDAPIN 53     //Defing SDA PIN
#define RESETPIN 5   // Defing RESET PIN

byte FoundTag; // Variable to use to check if TAG was found
byte ReadTag; // Variable to use to store anto-collision value to read tag information
byte TagData[MAX_LEN]; //Variable to store Full Tag Data
byte TagSerialNumber[5]; //Variable to use to store only Tag Serial number
byte GoodTagSerialNumber[4]={0XE5,0x6E,0XE2,0x2B}; // Stored UID
int match=0;
byte keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, }; //Default factory key
byte data[MAX_LEN];
byte sect_value[4]={0x50,0x61,0x64,0x6D}; //Predefined sector value
byte sect_data[MAX_LEN]; // Getting sector value
int pass=0;
//byte TagSerialNumber_2[4]={0x8E,0x0D,0x05,0x6B};
MFRC522 nfc(SDAPIN,RESETPIN);
void setup(){
  SPI.begin();
  Serial.begin(115200);
  Serial.println("Looking For the RFID Reader");
  nfc.begin();
  byte version=nfc.getFirmwareVersion();
  
  if(!version){
    Serial.println("Can't Find RDFID reader");
    while(1);//Halt Until RDFID module is found
  }
  
  Serial.println("Found Chip");
  Serial.println(version,HEX);
}

// Function for reading sector values

void reading(byte SerialNumber[4])
{
  
   byte status;
   status = nfc.authenticate(MF1_AUTHENT1A,2, keyA, SerialNumber); //Authenticating 2nd block using factory serial
    if (status == MI_OK) {
        Serial.print("Authenticated block ");
        Serial.print("2");
        Serial.println(" with key A.");
        status = nfc.readFromTag(2, data);
       if (status == MI_OK) {
            for (int j = 0; j < 15; j++) {
              Serial.print(data[j], HEX);
              Serial.print(", ");
          }
          Serial.println(data[15],HEX);
          memcpy(sect_data,data,4);      //Extracting 4 bytes of data
        } else {
          Serial.println("Read failed.");
        }
      }
}




void loop()
{
 
  //Request for TagData, if its is found it will return MI_OK
  FoundTag=nfc.requestTag(MF1_REQIDL,TagData);
  
  if(FoundTag == MI_OK)
  {
    ReadTag= nfc.antiCollision(TagData); // Reading Tag data
    memcpy(TagSerialNumber, TagData,5); // Copying the Tag Data into tag Serial Number So that we can read it
    
    Serial.println("Tag Detected");
    Serial.print("Tag Serial Number:   ");
    for (int i=0;i<4;i++)
    {
      Serial.print(TagSerialNumber[i],HEX);
      Serial.print(",");
    }
    Serial.print("");
    Serial.println();
  }
  
  //Checking for the known cards;
  for(int i=0;i<4;i++)
  {
    if(TagSerialNumber[i]!=GoodTagSerialNumber[i])
    {
      break;
    }
    if(i==3)
    {
      match=1;
    }
  }
  
  //Checking whether card is matched or not
  if(match==1)
  {
    Serial.println("Card is matched");
    match=0;
    nfc.selectTag(TagSerialNumber);
    reading(TagSerialNumber);
  }
  
  for(int i=0;i<4;i++)
  {
    if(sect_value[i]!=sect_data[i])
    {
      break;
    }
    if(i==3)
    {
      pass=1;
    }
  }
  
  
  
  //If the sector value mathes the following code will run
  
   if(pass==1)
  {
    Serial.println("Card is fully auntinticated");
   pass=0;
    Serial.println("Put your personal data card");
    for (int k=5;k>=1;k--)
    {
      Serial.println(k);
      delay(1000);
    }
    nfc.reset();    //Reseting the card values
    nfc.begin();   // Initializing the RFID
    
  FoundTag=nfc.requestTag(MF1_REQIDL,TagData);
  
  if(FoundTag == MI_OK)
  {
    ReadTag= nfc.antiCollision(TagData); // Reading Tag data
    memcpy(TagSerialNumber, TagData,5);
  }
    
  nfc.selectTag(TagSerialNumber);
  reading(TagSerialNumber);
  }
  
  
  
  
  
  
 // Reseting the tag serial number 
 for(int i=0;i<5;i++)
  {
    TagSerialNumber[i]=0;
   
  }
  
   
 // Reseting the sector data 
 for(int i=0;i<4;i++)
  {
    sect_data[i]=0;
   
  }
  nfc.haltTag();
}
