#include "SparkFun_Photon_Weather_Shield_Library.h"
#include "SparkFunPhant.h"

float humidity = 0;
float tempf = 0;
float pascals = 0;
float baroTemp = 0;

int count = 0;//This triggers a post and print on the first time through the loop

//Create Instance of HTU21D or SI7021 temp and humidity sensor and MPL3115A2 barrometric sensor
Weather sensor;

////////////PHANT STUFF//////////////////////////////////////////////////////////////////
const char server[] = "data.sparkfun.com";
const char publicKey[] = "roMoRqEOgjIxMnQnWDW7";
const char privateKey[] = "jk9kP7l6YjfMmR8R5b5j";
Phant phant(server, publicKey, privateKey);
/////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
    Serial.begin(9600);   // open serial over USB
    //Begin posting data immediately instead of waiting for a key press.

    //Initialize the I2C sensors and ping them
    sensor.begin();

    /*You can only receive acurate barrometric readings or acurate altitiude
    readings at a given time, not both at the same time. The following two lines
    tell the sensor what mode to use. You could easily write a function that
    takes a reading in one made and then switches to the other mode to grab that
    reading, resulting in data that contains both acurate altitude and barrometric
    readings. For this example, we will only be using the barometer mode. Be sure
    to only uncomment one line at a time. */
    sensor.setModeBarometer();//Set to Barometer Mode
    //baro.setModeAltimeter();//Set to altimeter Mode

    //These are additional MPL3115A2 functions the MUST be called for the sensor to work.
    sensor.setOversampleRate(7); // Set Oversample rate
    //Call with a rate from 0 to 7. See page 33 for table of ratios.
    //Sets the over sample rate. Datasheet calls for 128 but you can set it
    //from 1 to 128 samples. The higher the oversample rate the greater
    //the time between data samples.

    sensor.enableEventFlags(); //Necessary register calls to enble temp, baro ansd alt

    getWeather();

    getWeather();
    postToPhant();
}
//---------------------------------------------------------------
void loop()
{
    //Rather than use a delay, keeping track of a counter allows the photon to
    // still take readings and do work in between printing out data.
    count++;
    //alter this number to change the amount of time between each reading
    //this number is low for troubleshooting purposes. Be sure to change it
    //once you deploy so as not to send too much data.
    if(count == 10000)
    {
      //Get readings from all sensors
       getWeather();
       postToPhant();
       count = 0;
    }
}

void getWeather()
{
    // Measure Relative Humidity from the HTU21D or Si7021
    humidity = sensor.getRH();

    // Measure Temperature from the HTU21D or Si7021
    tempf = sensor.getTempF();
    // Temperature is measured every time RH is requested.
    // It is faster, therefore, to read it from previous RH
    // measurement with getTemp() instead with readTemp()

    //Measure the Barometer temperature in F from the MPL3115A2
    baroTemp = sensor.readBaroTempF();

    //Measure Pressure from the MPL3115A2
    pascals = sensor.readPressure();

}
//---------------------------------------------------------------
int postToPhant()
{
    //phant.add("altf", altf);//add this line if using altitude instead
    phant.add("barotemp", baroTemp);
    phant.add("humidity", humidity);
    phant.add("hectopascals", pascals/100);
    phant.add("tempf", tempf);

    TCPClient client;
    char response[512];
    int i = 0;
    int retVal = 0;

    if (client.connect(server, 80))
    {
        Serial.println("Posting!");
        client.print(phant.post());
        delay(1000);
        while (client.available())
        {
            char c = client.read();
            //Serial.print(c);
            if (i < 512)
                response[i++] = c;
        }
        if (strstr(response, "200 OK"))
        {
            Serial.println(tempf);
            Serial.println("Post success!");
            retVal = 1;
        }
        else if (strstr(response, "400 Bad Request"))
        {
            Serial.println("Bad request");
            retVal = -1;
        }
        else
        {
            retVal = -2;
        }
    }
    else
    {
        Serial.println("connection failed");
        retVal = -3;
    }
    client.stop();
    return retVal;

}
