#include <Arduino.h>
#include <arduino.h>
#include <avr/io.h>
#include "prioq.h"

#define PIN_LED_YELLOW 6 //Peasant
#define PIN_LED_RED 7 //VIP
#define PIN_BTN1 2
#define PIN_BTN2 3
#define DELAY 50
#define FLASH_NUMBER 5
#define PERIOD_RED 250
#define PERIOD_YELLOW 500

uint8_t flag_red, flag_yellow = 0;
uint32_t timeRed, timeYellow = 0;

TPrioQueue *queue;

// Declares a new type called "funcptr"

typedef void (*funcptr)(void);
funcptr newPtr;

// Flashes LED at pin 7: 5 times a 4 Hz
void int0task() {
	digitalWrite(PIN_LED_RED, HIGH);
	delay(PERIOD_RED);
	digitalWrite(PIN_LED_RED, LOW);
}

// Flashes LED at pin 6: 5 times at 2HZ
void int1task() {
	digitalWrite(PIN_LED_YELLOW, HIGH);
	delay(PERIOD_YELLOW);
	digitalWrite(PIN_LED_YELLOW, LOW);
}

void int0ISR() {
	flag_red = 1;
}

void int1ISR() {
	flag_yellow = 1;
}

void setup() {

	pinMode(PIN_LED_YELLOW, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);
	attachInterrupt(0, int0ISR, FALLING); //controls BTN1
	attachInterrupt(1, int1ISR, FALLING); //controls BTN2

	// Set up the queue.
	queue = makeQueue();

	// Initialize the serial port
	Serial.begin(115200);
}

// Dequeues and calls functions if the queue is not empty
void loop() {


	static uint8_t enqueueRed, enqueueYellow = 0;

	if (flag_yellow) {
			if (millis() - timeYellow > DELAY) {
				if (!digitalRead(PIN_BTN2))
					enqueueYellow = 1;
				else
					flag_yellow = 0;
			}
		}

	if(enqueueYellow){
		for (int i = 0; i < FLASH_NUMBER; i++)
			enq(queue, (void *) int1task, 1);
		enqueueYellow = 0;
	}

/*-----------------------------------------------------------------------*/

	if (flag_red) {
			if (millis() - timeRed > DELAY) {
				if (!digitalRead(PIN_BTN1))
					enqueueRed = 1;
				else
					flag_red = 0;
			}
		}

	if(enqueueRed){
		for (int i = 0; i < FLASH_NUMBER; i++)
			enq(queue, (void *) int0task, 1);
		enqueueRed = 0;
	}

/*-----------------------------------------------------------------------*/

	if (qlen(queue) > 0)
		newPtr = (funcptr) deq(queue);

	newPtr();
}
