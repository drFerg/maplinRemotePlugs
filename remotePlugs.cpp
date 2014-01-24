/* Library for controlling Maplin remote control plugs using a cheap 433Mhz transmitter */
/* Author: Fergus Leahy - fergus.leahy@gmail.com */
/* Awesome reverse engineering and basic timing code created by fanjita */
/* at http://www.fanjita.org/serendipity/archives/53-Interfacing-with-radio-controlled-mains-sockets-part-2.html */

/* MACROS for pins */
#define VCC_PIN   A3
#define GND_PIN   A4
#define DATA_PIN  A5
#define LED_PIN   13

/* Number of retries due to lossy radio */
#define RETRIES 6

/* DEVICE ON/OFF COMMAND CODE */
#define DEV_ON 13107
#define DEV_OFF 21299

/* MACROS for Group and Plug addrs */
#define GROUP1 0
#define GROUP2 1
#define GROUP3 2
#define GROUP4 3

#define PLUG1 0
#define PLUG2 1
#define PLUG3 2
#define PLUG4 3

/* PULSE times in microseconds */
#define PREAMBLE 13000
#define SYNC_PULSE_WIDTH 500
#define PULSE_ZERO_WIDTH 500
#define PULSE_ONE_WIDTH 1500

/* MACROS for ON/OFF commands */
#define turnOn(GROUP, DEVICE) sendCommand(GROUP, DEVICE, DEV_ON)
#define turnOff(GROUP, DEVICE) sendCommand(GROUP, DEVICE, DEV_OFF)

/* PLUG address codes */
long plugs[4][4] = {
  {859124533L, 861090613L, 892547893L, 1395864373L},
  {859124563L, 861090643L, 892547923L, 1395864403L},
  {859125043L, 861091123L, 892548403L, 1395864883L},
  {859132723L, 861098803L, 892556083L, 1395872563L}  
};

void initRadio() {
  digitalWrite(GND_PIN, LOW);
  digitalWrite(DATA_PIN, HIGH);
}

void powerOn() {
  digitalWrite(VCC_PIN, HIGH);
  digitalWrite(13, HIGH);
}

void powerOff() {
  digitalWrite(VCC_PIN, LOW);
}

void sendData(long device, int command) {
  digitalWrite(13, LOW);
  /* Transmit preamble and sync pulse */
  digitalWrite(DATA_PIN, LOW);
  delayMicroseconds(PREAMBLE);
  digitalWrite(DATA_PIN, HIGH);
  delayMicroseconds(SYNC_PULSE_WIDTH);
  digitalWrite(DATA_PIN, LOW);
  
  /* Tansmit command bits */  
  /* 1 == state change for 1.5ms, 0 == state change for 0.5ms */
  char state = HIGH;
  for (unsigned long longMask = 1; longMask > 0; longMask <<= 1) {   
    state = !state;
    digitalWrite(DATA_PIN, state);
    if (device & longMask) delayMicroseconds(PULSE_ONE_WIDTH);
    else delayMicroseconds(PULSE_ZERO_WIDTH);
  }
  for (unsigned int shortMask = 1; shortMask > 0; shortMask <<= 1) {   
    state = !state;
    digitalWrite(DATA_PIN, state);
    if (command & shortMask) delayMicroseconds(PULSE_ONE_WIDTH);
    else delayMicroseconds(PULSE_ZERO_WIDTH);
  }
}

void sendCommand(int group, int device, int command) {
  long groupDevice = plugs[group][device];
  /* Transmit the data several times -- lossy radio */
  for (int i = 0; i < RETRIES; i++) {
    sendData(groupDevice, command);
  }
}
