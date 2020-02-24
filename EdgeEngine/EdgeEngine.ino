using std::vector;
using std::string;
#include <time.h>
#include <EdgeEngine_library.h>

int pirPin = 35;
int lightPin = 34;// pin 27 not working with this sketch
int potPin = 32;

clock_t pirCounter;
clock_t cycleCounter; // count execution cycle time
clock_t sleepTime;

sample* motion=NULL;
sample* potentiometer=NULL;
sample* light=NULL;
const char* ssidWifi = "ssid";
const char* passWifi = "pass";

edgine* Edge;
connection* Connection; //Wrapper for the wifi connection
vector<sample*> samples;


/**
 * setup
 */
int i;
void setup() {
  
  Serial.begin(115200);

  //setup connection
  Connection = connection::getInstance();
  Connection->setupConnection(ssidWifi, passWifi);

  options opts;
  //login
  opts.username = "riccardo-office-temperature-sensor-username";
  opts.password =  "riccardo-office-temperature-sensor-password";
  //route
  opts.url = "http://students.atmosphere.tools";
  opts.ver = "v1";
  opts.login = "login";
  opts.devs = "devices";
  opts.scps = "scripts";
  opts.measurements = "measurements";
  opts.info= "info";
  opts.issues="issues";
  //Edgine identifiers
  opts.thing = "riccardo-office";
  opts.device = "environment-riccardo-office";
  opts.id = "environment-riccardo-office";
  
  //initialize Edge engine
  Edge=edgine::getInstance();
  Edge->init(opts);
  
  //Interrupt sensor setup
  pinMode(pirPin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(pirPin), detectedMotion, FALLING);
  i=0;
  
}



void loop() {
  cycleCounter=clock();
  
  //create a light measurement sample
  light = new sample("light");
  int lig=analogRead(lightPin);
  light->startDate=Edge->Api->getActualDate();
  light->endDate=light->startDate;
  // light->value= lig;
  light->value=(double)ESP.getFreeHeap();
  samples.push_back(light);
  
  //create a potentiometer measurement sample

  potentiometer = new sample("temperature");
  int pot=analogRead(potPin);
  potentiometer->startDate=Edge->Api->getActualDate();
  potentiometer->endDate=potentiometer->startDate;
  //potentiometer->value=pot;
  potentiometer->value=i++;
  samples.push_back(potentiometer);
  Edge->evaluate(samples);
  samples.clear(); // after evaluated all samples delete them

  delete potentiometer;
  potentiometer=NULL;
  delete light;
  light =NULL;
  //delete motion;

  // if( ((double)clock()-pirCounter)>=2000){// pir sensor needs 2 seconds to be ready to give another measurement
  //   //attachInterrupt(digitalPinToInterrupt(pirPin), detectedMotion, FALLING);
  //   //Serial.println("attachInterrupt");
  // }
  if (!Connection->isConnected()) {
    Serial.println("Device disconnected");
    Serial.print("WIFI STATUS: ");
    Serial.println(WiFi.status());
    Connection->reconnect();
  }

  cycleCounter=clock()-cycleCounter;// duration of the exexution of th cycle
  
  // subtract te execution time to the Sleep period if result is not negative
  ((double)cycleCounter/CLOCKS_PER_SEC) < Edge->getPeriod() ? sleepTime=(Edge->getPeriod()-(double)cycleCounter/CLOCKS_PER_SEC)*1000 : sleepTime=0;//delay in milliseconds
  
  
  delay(sleepTime);
}




// void detectedMotion(){
//   detachInterrupt(digitalPinToInterrupt(pirPin)); //PIR sensor needs 2 seconds to take an image to compare to
//   pirCounter=(double)clock();
//   Serial.println("Motion detected");
//   motion = new sample("motion");
//   motion->startDate=Edge->Api->getActualDate();
//   motion->endDate=motion->startDate;
//   motion->value=1;
//   samples.push_back(motion);
// }
