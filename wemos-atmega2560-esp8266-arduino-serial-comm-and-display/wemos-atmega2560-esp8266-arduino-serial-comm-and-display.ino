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
int currentLine = 0;

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

bool startsWith(const char *str, const char *pre) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

struct UpdatePacket {
  int marker1;
  long uptime;
  float cpu;
  float ram1;
  float ram2;
  float ram3;
  float ram4;
  int marker2;
};
 
void loop(void) {

  int readBufSize = tft.width() / FONT_WIDTH;
  int incomingByte = 0;

  if ((incomingByte = Serial3.available()) > 0) {
    
    struct UpdatePacket up;
    
    Serial3.readBytes((uint8_t *)&up, sizeof(up));

    Serial.print(up.marker1);
    Serial.print(" ");
    Serial.print(up.marker2);

    if (up.marker1 == 123 && up.marker2 == 321) {
      tft.setCursor(0, FONT_HEIGHT * (currentLine++) + 1);
      tft.setTextColor(GREEN, BLACK);

      // CPU
      char float_tmp[7];
      dtostrf(up.cpu, 3, 2, float_tmp);
      char * buf = (char*)malloc(sizeof(char) * 32);
      strcpy(buf, ">CPU: ");
      strcat(buf, float_tmp);
      strcat(buf, "%<");
      
      tft.println(buf);
    }
  }
  delay(2000);
}
