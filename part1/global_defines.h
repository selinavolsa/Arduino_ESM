#ifndef ES_Const_Defines_h
#define ES_Const_Defines_h

// Font dimensions

#define FONT_WIDTH 5
#define FONT_HEIGHT 8
#define LINE_HEIGHT 10

// Button mask definitions

#define BTN_LEFT   1 << 0
#define BTN_RIGHT  1 << 1
#define BTN_UP     1 << 2
#define BTN_DOWN   1 << 3
#define BTN_ACCEPT 1 << 4

// Color definitions

#define BLACK    0x0000
#define RED      0x001F
#define WHITE    0xFFFF

// Pin definitions

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC  8

#define PIN_LEFT 18
#define PIN_RIGHT 17
#define PIN_UP 16
#define PIN_DOWN 15
#define PIN_ACCEPT 14

// define time step in ms for delay()

#define FRAMETIME 100

// Maxima definitions

#define NAMELENGTH 21
#define PROMPTLENGTH 200
#define MAXITEMNUM 15
#define LOGSTRINGLENGTH 300
#define LABELLENGTH 16

// Filepath definitions

#define LOGFILE "aesmlog.csv"

class Adafruit_ST7735;

extern Adafruit_ST7735* tft;

#endif
