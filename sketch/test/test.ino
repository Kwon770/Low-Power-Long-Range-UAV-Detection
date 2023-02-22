

unsigned int * model;

void setup() {
  Serial.begin(115200);
  while (!psramInit()) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("FreeHeap: ");
  Serial.println(ESP.getHeapSize());
  Serial.print("getFreePsram: ");
  Serial.println(ESP.getFreePsram());
  
  model = (unsigned int *) malloc(2000000);
  model[1] = 99;

  Serial.println("REGISTER");

  Serial.print("FreeHeap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("getFreePsram: ");
  Serial.println(ESP.getFreePsram());
}

void loop() {
  Serial.println(model[1]);
  delay(10000);
}
