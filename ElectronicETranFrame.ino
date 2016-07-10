#include <Keypad.h>
#include <Wire.h> 

#define DISPLAY_WIDTH 20
#define DISPLAY_HEIGHT 4
#define DISPLAY_LEN (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#include "LiquidCrystal_I2C.h"
#define BACKLIGHT_PIN (3)

LiquidCrystal_I2C lcd1(0x27, 2, 1, 0, 4, 5, 6, 7, BACKLIGHT_PIN, POSITIVE);
LiquidCrystal_I2C lcd2(0x26, 2, 1, 0, 4, 5, 6, 7, BACKLIGHT_PIN, POSITIVE);

#define ROWS 4 //four rows
#define COLS 3 //three columns

char keys[ROWS][COLS] = {
	{'1','2','3'},
	{'4','5','6'},
	{'7','8','9'},
	{'*','0','#'}
};

//IDs that will be returned on a key press
char keysID[ROWS][COLS] = {
	{1,2,3},
	{7,8,9},
	{4,5,6},
	{10,11,12}
};

byte rowPins[ROWS] = {5, 6, 7, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keysID), rowPins, colPins, ROWS, COLS );

//mapping of 2 key presses to a letter
char letterMapping[6][6] = {
	{'I','H','G','L','K','J'},
	{'2','1','0','5','4','3'},
	{'C','B','A','F','E','D'},
	{'U','T','S','X','W','V'},
	{'8','7','6','Z','Y','9'},
	{'O','N','M','R','Q','P'}
};

#define NUM_LEDS 6
uint8_t LED_PINS[NUM_LEDS] = {A2, 11, A3, A1, 10, A0};

//
void setLED(uint8_t key)
{
	for (int i = 0; i < NUM_LEDS; i++)
		digitalWrite(LED_PINS[i], LOW);
	
	if (key >= 1 && key <= 6)	
		digitalWrite(LED_PINS[key - 1], HIGH);
}

//loads a full buffer of text onto the display
void setText(LiquidCrystal_I2C *lcd, char *text, uint8_t cursorPos)
{
	char tmpBuf[DISPLAY_WIDTH + 1];
	tmpBuf[DISPLAY_WIDTH] = 0;

	lcd->setCursor(0, 0);
	memcpy(tmpBuf, text + DISPLAY_WIDTH * 0, DISPLAY_WIDTH);
	lcd->print(tmpBuf);

	lcd->setCursor(0, 1);
	memcpy(tmpBuf, text + DISPLAY_WIDTH * 1, DISPLAY_WIDTH);
	lcd->print(tmpBuf);

	lcd->setCursor(0, 2);
	memcpy(tmpBuf, text + DISPLAY_WIDTH * 2, DISPLAY_WIDTH);
	lcd->print(tmpBuf);

	lcd->setCursor(0, 3);
	memcpy(tmpBuf, text + DISPLAY_WIDTH * 3, DISPLAY_WIDTH);
	lcd->print(tmpBuf);

	uint8_t col = cursorPos % DISPLAY_WIDTH;
	uint8_t row = cursorPos / DISPLAY_WIDTH;
	lcd->setCursor(col, row);
}

//
void setup()
{
	Serial.begin(9600);

	for (int i = 0; i < NUM_LEDS; i++)
		pinMode(LED_PINS[i], OUTPUT);

	//show LED test
	for (int i = 0; i < NUM_LEDS; i++)
	{
		setLED(1 + i);
		delay(300);
	}
	setLED(0);

	// Switch on the backlight
	pinMode(BACKLIGHT_PIN, OUTPUT);
	digitalWrite(BACKLIGHT_PIN, HIGH);

	lcd1.begin(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	lcd2.begin(DISPLAY_WIDTH, DISPLAY_HEIGHT);

	//show display test
	char buf[DISPLAY_LEN + 1];
	for (int i = 0; i < DISPLAY_LEN; i++)
		buf[i] = 'A' + i % 26;

	setText(&lcd1, buf, 0);
	setText(&lcd2, buf, 0);
	delay(500);

	lcd1.clear();
	lcd2.clear();

	//show ready message
	lcd1.home();
	lcd1.cursor();
	lcd1.print("Display 1: READY");  

	lcd2.home();
	lcd2.cursor();
	lcd2.print("Display 2: READY");  

	delay(1000);

	lcd1.clear();
	lcd2.clear();

	uint8_t pos = 0;
	memset(buf, ' ', DISPLAY_LEN);
	buf[DISPLAY_LEN] = 0;

	char key1, key2;
	while (1)
	{
		char letter = 0;
		while ((key1 = keypad.getKey()) == NO_KEY); //wait for keypress

		//delete last letter
		if (key1 == 7)
		{
			if (pos > 0)
			{
				pos--;
				buf[pos] = ' ';
			}
		}
		//add a space
		else if (key1 == 9)
		{
			letter = ' ';
		}
		//reset display
		else if (key1 == 12)
		{
			memset(buf, ' ', DISPLAY_LEN);
			pos = 0;
		}
		//add chosen letter		
		else
		{
			setLED(key1);
			while ((key2 = keypad.getKey()) == NO_KEY); //wait for keypress
			setLED(key2);
			delay(300);
			setLED(0);
			
			if (key1 >= 1 && key1 <= 6 && key2 >= 1 && key2 <= 6)
				letter = letterMapping[key1 - 1][key2 - 1];
		}

		//if a letter was added
		if (letter)
		{
			buf[pos] = letter;
			if (pos < DISPLAY_LEN - 1)
				pos++;
		}

		setText(&lcd1, buf, pos);
		setText(&lcd2, buf, pos);
	}
}

//
void loop() {}

