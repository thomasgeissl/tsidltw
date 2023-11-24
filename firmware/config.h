#define ID 1
#define HAS_WIFI true
#define HAS_MIDI false
#define HAS_RTP_MIDI false
#define HAS_MPU6050 true
#define USE_MPU6050_TEMPERATURE false
#define HAS_VL53L false

#define LOCAL_OSC_PORT 8000
#define REMOTE_OSC_PORT 8088


#define HAS_ANALOG_INPUTS false
#define NUMBER_OF_ANALOG_INPUTS 2
int analogInputPins[NUMBER_OF_ANALOG_INPUTS] = {34, 39};


#define NUMBER_OF_TOUCHES 9
int touchPins[NUMBER_OF_TOUCHES] = {13, 12, 14, 27, 33, 32, 15, 2, 4};
//int touchPins[NUMBER_OF_TOUCHES] = {13};

#define SEND_INTERVAL_MS 10

#define MAX_NUMBER_OF_TRIES 24

#define AP_SSID "nantlab-wifi"
#define AP_PASSWORD "nantlab-wlan-pw"

#define NETWORK_SSID "nantlab-wlan"
#define PASSWORD "nantlab-wlan-pw"
