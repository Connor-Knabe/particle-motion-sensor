#define LED D7
#define SENSORONE D0
#define SENSORTWO D5
void setup() {
    Serial.begin(115200);
    Time.zone(-6);
    Time.beginDST();
    pinMode(LED, OUTPUT);
    pinMode(SENSORONE, INPUT);
    pinMode(SENSORTWO, INPUT);
    RGB.control(true); 
    RGB.brightness(0);
}

bool sensorOneMotionHasBeenDetected = false;
bool sensorOneNoMotionDetectedAlert = true;

bool sensorTwoMotionHasBeenDetected = false;
bool sensorTwoNoMotionDetectedAlert = true;

//2 days
int noMotionAlertTime = 2 * 24 * 60;

unsigned long lastTimeOneOn = 0;
unsigned long lastTimeOneOff = 0;
unsigned long lastTimeAlertOneOff = 0;

unsigned long lastTimeTwoOn = 0;
unsigned long lastTimeTwoOff = 0;
unsigned long lastTimeAlertTwoOff = 0;

int motionOffTime = 5;
uint32_t msDelay;


void loop() {
    if (millis() - msDelay < 3000) return;
    msDelay = millis();
  
    int sensorOneValue = digitalRead(SENSORONE);
    int sensorTwoValue = digitalRead(SENSORTWO);

    detectMotion(sensorOneValue,"sensorOne ", lastTimeOneOn, lastTimeOneOff, lastTimeAlertOneOff, sensorOneMotionHasBeenDetected, sensorOneNoMotionDetectedAlert);
    detectMotion(sensorTwoValue,"sensorTwo ", lastTimeTwoOn, lastTimeTwoOff, lastTimeAlertTwoOff, sensorTwoMotionHasBeenDetected, sensorTwoNoMotionDetectedAlert);
}

void detectMotion(int sensorVal, char *sensorName, unsigned long &lastTimeOn, unsigned long &lastTimeOff, unsigned long &lastTimeAlertOff, bool &motionHasBeenDetected, bool &noMotionDetectedAlert){
    char alertInfo[40];
    char sensorStr[10];
    char lastOffTimeStr[20];
    
    strcpy(alertInfo, sensorName);
    sprintf(sensorStr, "Motion: %ld", sensorVal);
    strcat(alertInfo, sensorStr);
    

    if(sensorVal>0){
        Particle.publish("Motion Detected", alertInfo, 60, PRIVATE);
        //motion
        digitalWrite(LED, HIGH);
       
        //motion was previously not found but now has been seen for x time
        if(!motionHasBeenDetected){
            Particle.publish("motion_seen", sensorName, 60,  PRIVATE);
            motionHasBeenDetected = true;
            noMotionDetectedAlert = false;
        } 
    } else {
        digitalWrite(LED, LOW);
        Particle.publish("No Motion Detected", alertInfo, 60, PRIVATE);
        if ((millis() - lastTimeOff) >=  motionOffTime * 30 * 1000) {
        	(lastTimeOff) = millis();
        	//motion was previously detected but now hasn't for x time
            if(motionHasBeenDetected){
                Particle.publish("no_motion", sensorName, 60,  PRIVATE);
                motionHasBeenDetected = false;
            } 
        }

        if ((millis() - lastTimeAlertOff) >=  noMotionAlertTime * 60  * 1000) {
        	(lastTimeAlertOff) = millis();
        	//motion has previously been seen but now hasn't for x time
            if(noMotionDetectedAlert){
                Particle.publish("no_motion_detected_alert", sensorName, 60,  PRIVATE);
            } else { 
                noMotionDetectedAlert = true;
            } 
        }
    }
}
