// Signale à l'utilisateur une erreur
void failedSignal() {
  while (true) bip(5, 500);
}

// Fait un bip
void bip(int loop, int time) {
  for (int i=0; i<loop; i++) {
    tone(BUZZER, 900, time);
    delay(time+100);
  }  
  delay(1000);
}

// Logging pour l'information
void logInfo(const String text) { logInfo(text, false); }
void logInfo(const String text, bool withSeparator) {
  if (Serial) {
    Serial.println("[I] " + text);  
    if (withSeparator) Serial.println("[I] ========================================");
  }
}

// Logging pour l'erreur
void logError(const String text) { logError(text, false); }
void logError(const String text, bool withSeparator) {
  if (Serial) {
    Serial.println("[E] " + text);   
    if (withSeparator) Serial.println("[E] ========================================");
  }
}

// Ecrit l'en-tête du tableau dans le fichier spécifié
bool writeFileHeader(String filePath) {
  return writeFile(filePath, "Le temps est depuis l'allumage de la carte;\n"
                             "Temps (s);Altitude approximative (m);Température gyroscope (°C);Température intérieur (°C);Température extérieur (°C);"
                                "Pression intérieur (hPa);Pression extérieur (hPa);Accélération (m/s^2);...;...;Rotation (rad/s);...;...;\n"
                             "...;...;...;...;...;...;...;X;Y;Z;X;Y;Z;\n");
}

// Récupère toutes les valeurs des capteurs et les écrits dans l'ordre de l'en-tête
void getAndWriteSensorsEvents(String filePath, int maxOperationTime) {
  uint32_t lastTime = millis();

  mpu.getEvent(&accelSensor, &gyroSensor, &tempSensor);
  String content = String(bmp1.readAltitude(seaPressure)) + ";" + String(tempSensor.temperature, 3) + ";" + String(bmp1.readTemperature(), 3) + ";" + String(bmp2.readTemperature(), 3) + ";" + String(bmp1.readPressure()/100, 4) + ";";
  content += String(bmp2.readPressure()/100, 4) + ";" + String(accelSensor.acceleration.x, 6) + ";" + String(accelSensor.acceleration.y, 6) + ";" + String(accelSensor.acceleration.z, 6) + ";";
  content += String(gyroSensor.gyro.x, 6) + ";" + String(gyroSensor.gyro.y, 6) + ";" + String(gyroSensor.gyro.z, 6) + ";";
  content = getFormattedMillis(millis()) + ";" + content;

  logInfo("");
  logInfo("Content that will be written: " + content);
  writeFile(filePath, content + "\n", true);
  logInfo("");
  
  int16_t loopTime = millis()-lastTime;
  logInfo("Operation took " + String(loopTime) + " ms to execute.");
  loopTime = maxOperationTime-loopTime;
  loopTime = loopTime < 0 ? 0 : loopTime;
  logInfo("The end of the loop will be in " + String(loopTime) + " ms");
  delay(loopTime);
}

// function taken here: https://github.com/arduino-libraries/Arduino_DebugUtils/issues/5
uint16_t allSeconds, secondsRemaining;  
char formattedMillis[15]; 

// return a millisecond value as a string formatted as "HH:MM:SS.mmm"
String getFormattedMillis(uint32_t msCount) {
  allSeconds = msCount / 1000;           // total number of seconds to calculate remaining values
  secondsRemaining = allSeconds % 3600;    // seconds left over
  
  //                        HH:  MM:  SS:  mmm:   seconds to hours:   seconds to minutes:    seconds left over:    milliseconds:
  sprintf(formattedMillis, "%02d:%02d:%02d.%03d", allSeconds / 3600, secondsRemaining / 60, secondsRemaining % 60, msCount % 1000);
  return String(formattedMillis);
}
