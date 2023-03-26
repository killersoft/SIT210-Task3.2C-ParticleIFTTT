long OnTime = 0;
long LastTimeOn = 0;
int AnalogValue; //Value caused from No-light to full light as a variable voltage divider circuit for the phototransistor
int PhotoTransisorThresholdOnOffValue = 1000; // Found 1000 was a good compromise between Light sensitivity for 'ON' vs 'OFf State' Ranges ~220 -> ~4000.
long hour = 3600000; //60000 millis = 1 Minute, 1 Hour = 3,600,000millis.. 

SYSTEM_THREAD(ENABLED); // It allows your code to run before the cloud is connected.
SerialLogHandler logHandler; // This uses the USB serial port for debugging logs.
const pin_t LED_PIN = D6; // This is where the LED is plugged in. The other side goes to a resistor and to GND
const pin_t SENSOR_PIN = A0; // This is where the phototransistor is plugged into, resistance(10k) across the Collector & Emitter lowers when light hits the clear shell.

void setup() {
    pinMode(LED_PIN, OUTPUT); // Set the PIN for the LED as an output capable pin.
    // Subscribe to the integration response event
    Particle.subscribe("hook-response/2Hours_Light", myHandler, MY_DEVICES);
}

void loop() { 

    AnalogValue = analogRead(SENSOR_PIN); //Read the phtotransistor value
    
    if(AnalogValue>PhotoTransisorThresholdOnOffValue){ // Light ON analog value
            OnTime = OnTime + (millis()-LastTimeOn); //count upwards seconds of Light-On-Time
			LastTimeOn=millis(); //increment Last time on at end of loop.
    }
    //Next lets test if we have 2 hours worth of SUNLIGHT 
    if (OnTime > (hour*2)){  //Can set TO 60 SECONDS RATHER THAN 2 HOURS FOR DEMONSTARTION
        Particle.publish("2Hours_Light", PRIVATE); //Send out webhook to particle.io -> IFTTT->email notification.
         
        //Now reset counters
        OnTime=0; // Reset the upwards counter
        //Should put in a 24 hour delay here, but really need a RTC.
        delay(hour*12); //Ideally wait 12 hours until we look again for another 2 hours worth of daylight. If we had a Real-Time-Clock attached or went out to the internet to get time, this is a good solution
    }
        

}

// If we get publish a request, see the responce back from particle.io
void myHandler(const char *event, const char *data) {
    // Handle the integration response
    Log.info("Got message back from particle.io"); // Serial console output for monitoring
    digitalWrite(LED_PIN, HIGH); // Tell user we got confirmation sending the webhook/email
    delay(10000); // Add a 10 second LED ON delay to show we have sucessfully actioned the webhook and it was received.
    digitalWrite(LED_PIN, LOW); //Turn LED off
}
