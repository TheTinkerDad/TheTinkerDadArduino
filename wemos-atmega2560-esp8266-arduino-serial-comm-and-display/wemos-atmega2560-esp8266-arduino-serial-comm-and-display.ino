#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>

#define FONT_HEIGHT 16
#define FONT_WIDTH 12
#define MAIN_BUF_SIZE 256
#define READ_BUF_SIZE 64
#define BLACK   0x0000
#define GREEN   0x07E0

MCUFRIEND_kbv tft;
int16_t font_size, lines; 
char mainbuffer[MAIN_BUF_SIZE];
char buffer[READ_BUF_SIZE+1];

void setup() {

  Serial3.begin(9600);
  
  tft.reset();
  tft.begin(tft.readID());
  tft.setRotation(1);

  font_size = FONT_HEIGHT / 8;
  lines = tft.width() / FONT_HEIGHT;
  
  tft.fillScreen(BLACK);
  tft.setTextColor(GREEN, BLACK);
  tft.setTextSize(font_size);
  tft.setCursor(0, 0);
  tft.println("Awaiting data...");

  memset(mainbuffer, 0, MAIN_BUF_SIZE);
}

void loop(void) {

  int readBufSize = tft.width() / FONT_WIDTH;
  int incomingByte = 0;

  incomingByte = Serial3.available();
  if (incomingByte > 0) {
    memset(buffer, 0, READ_BUF_SIZE+1);
    int batchSize = READ_BUF_SIZE > incomingByte ? incomingByte : READ_BUF_SIZE;
    Serial3.readBytes(buffer, batchSize);

    // Process the data we've received, even if it's partial
    for (int i = 0; i < batchSize; i++){
      if (buffer[i] == '>') {                 // New data block
        memset(mainbuffer, 0, MAIN_BUF_SIZE);
      } else if (buffer[i] == '<') {          // Finished data block
        tft.setCursor(0,0);
        tft.println(mainbuffer);
      } else {                                // Within data block
        mainbuffer[strlen(mainbuffer)] = buffer[i];
      }
    }
  }
  delay(100);
}
