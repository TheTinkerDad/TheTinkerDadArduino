#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>    // Core graphics library

#define FONT_HEIGHT 16
#define FONT_WIDTH 12

#define MAIN_BUF_SIZE 256
#define READ_BUF_SIZE 32

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

MCUFRIEND_kbv tft;
int16_t font_size, lines; 
char mainbuffer[MAIN_BUF_SIZE];
char buffer[READ_BUF_SIZE];

void setup() {

  Serial3.begin(9600);
  
  tft.reset();
  uint16_t id = tft.readID();
  tft.begin(id);
  tft.setRotation(1);

  font_size = FONT_HEIGHT / 8;
  lines = tft.width() / FONT_HEIGHT;
  
  tft.fillScreen(BLACK);
  tft.setTextColor(GREEN, BLACK);
  tft.setTextSize(font_size);

  tft.setCursor(0, 0);
  tft.setTextColor(GREEN); 
  tft.println("Awaiting data...");

  memset(mainbuffer, 0, MAIN_BUF_SIZE);
}
 
void loop(void) {

  int readBufSize = tft.width() / FONT_WIDTH;
  int incomingByte = 0;

  if ((incomingByte = Serial3.available()) > 0) {
    memset(buffer, 0, READ_BUF_SIZE);
    Serial3.readBytesUntil('\n', buffer, incomingByte);

    // Process the data we've received, even if it's partial
    for (int i = 0; buffer[i] != NULL; i++){
      // New data line
      if (buffer[i] == '>') {
        memset(mainbuffer, 0, MAIN_BUF_SIZE);

      // Finished data line
      } else if (buffer[i] == '<') {
        //tft.fillRect(0, 0, tft.width(), FONT_HEIGHT, BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(GREEN, BLACK);
        tft.println(mainbuffer);

      // Within data line
      } else {
        mainbuffer[strlen(mainbuffer)] = buffer[i];
      }
    }
  }
  delay(100);
}
