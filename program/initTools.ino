// Initialisation de la carte SD
bool initSDCard(int csPin) {
  logInfo("", true);
  logInfo("SD Card Module initialization ...");

  // Try to init card
  if (!SD.begin(csPin, SPI_HALF_SPEED)) {
    logError("Initialization Failed. No SD Card found or no FAT16 or FAT32 partition found!", true);
    return false;
  } else {
    logInfo("SD card found!");
    logInfo("Getting informations ...");
    logInfo("");
  }

  // Print SD Card informations
  printCardInfos();

  logInfo("");
  logInfo("Initialization success.", true);
  delay(100);
  return true; // Initialization success
}

// Initialise l'environnement de fichier et renvoie le chemin à utiliser pour l'écriture
String initFilesEnvironement(String rootDirectory) {
  logInfo("", true);
  logInfo("Files environement path initialization ...");

  // Test if partition is read-only
  logInfo("Write test on the device ...");
  if (!createFile("/", "write.test")) {
    logError("Can't write on device! The partition is in read-only.");
    return "";
  }
  deleteFile("/write.test");

  // Try to find the root directory
  logInfo("");
  if (!exists(rootDirectory, false)) {
    logError("Root directory don't exist. Creating directory ...");
    if (!createDir(rootDirectory)) {
      logError("Failed to create root directory!", true);
      return "";
    }
  } else logInfo("(root directory)");

  // Try to find an uncreated folder
  byte tests = 0;
  String path = rootDirectory + "/" + String(tests);
  while (exists(path, false) && tests++ < 255) {
    logInfo("Find another directory ...");
    path = rootDirectory + "/" + String(tests);
  }
  if (tests == 255) {
    logInfo("");
    logError("Too many tests were done to find an uncreated folder! Please delete or rename existing folders", true);
    return ""; // Too many tests
  }

  // Succes to find an uncreated folder
  logInfo("");
  logInfo("Succes to find an uncreated folder! '" + path + "' will now be used as the current directory for writing files. Creating folder ...");
  createDir(path);
  logInfo("Initialization success.", true);
  delay(100);
  return path + "/";
}

bool initMPU(byte address, Adafruit_MPU6050 &mpu) {
  logInfo("", true);
  logInfo("MPU6050 initialization ...");

  // Try to initialize
  if (!mpu.begin(address)) {
    logError("Failed to find MPU6050 chip!", true);
    return false;
  } else {
    logInfo("MPU6050 Found!");
    logInfo("Getting informations ...");
    logInfo("");
    logInfo("I2C Address: 0x" + String(address, HEX));
  }

  // Print accelerometer range
  sensor_t sensor;
  mpu.getAccelerometerSensor()->getSensor(&sensor);
  //mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu6050_accel_range_t accelRange = mpu.getAccelerometerRange();
  logInfo("Accelerometer range: ±" + String(accelRange == MPU6050_RANGE_2_G ? "2" : accelRange == MPU6050_RANGE_4_G ? "4" : accelRange == MPU6050_RANGE_8_G ? "8" : accelRange == MPU6050_RANGE_16_G ? "16" : "<unknown>") + " G");
  logInfo("Accelerometer resolution: ~" + String(sensor.resolution, 4) + " G");

  // Print gyroscope range
  mpu.getGyroSensor()->getSensor(&sensor);
  //mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu6050_gyro_range_t gyroRange = mpu.getGyroRange();
  logInfo("Gyro range: ±" + String(gyroRange == MPU6050_RANGE_250_DEG ? "250" : gyroRange == MPU6050_RANGE_500_DEG ? "500" : gyroRange == MPU6050_RANGE_1000_DEG ? "1000" : gyroRange == MPU6050_RANGE_2000_DEG ? "2000" : "<unknown>") + " deg/s");
  logInfo("Gyro resolution: ~" + String(sensor.resolution, 6) + " deg/s");

  // Print temperature sensor infos
  mpu.getTemperatureSensor()->getSensor(&sensor);
  logInfo("Temperature range: " + String(sensor.min_value) + " °C ~ +" + String(sensor.max_value) + "° C");
  logInfo("Temperature resolution: ~" + String(sensor.resolution, 4) + " °C");

  // Print the band width
  //mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  mpu6050_bandwidth_t bandwidth = mpu.getFilterBandwidth();
  logInfo("Filter bandwidth: " + String(bandwidth == MPU6050_BAND_260_HZ ? "260" : bandwidth == MPU6050_BAND_184_HZ ? "184" : bandwidth == MPU6050_BAND_94_HZ ? "94" : bandwidth == MPU6050_BAND_44_HZ ? "44" : bandwidth == MPU6050_BAND_21_HZ ? "21" : bandwidth == MPU6050_BAND_10_HZ ? "10" : bandwidth == MPU6050_BAND_5_HZ ? "5" : "<unknown>") + " Hz");

  logInfo("");
  logInfo("Initialization success.", true);
  delay(100);
  return true; // Initialization success
}

bool initBMP(byte address, Adafruit_BMP280 &bmp, String label) {
  logInfo("", true);
  logInfo("BMP280 (" + label + ") initialization ...");

  // Try to initialize
  if (!bmp.begin(address)) {
    logError("Could not find a valid BMP280 sensor!", true);
    return false;
  } else {
    logInfo("BMP280 sensor Found!");
    logInfo("Getting informations ...");
    logInfo("");
    logInfo("I2C Address: 0x" + String(address, HEX));
  }
  
  // Try to find chip ID
  logInfo("Chip ID: 0x" + String(bmp.sensorID(), HEX));
  if (bmp.sensorID() == 0xff) {
    logError("Chip ID is not recognized!", true);
    return false;
  }

  // Print sensor status
  logInfo("Chip register status: " + String(bmp.getStatus()));

  // Print temperature sensor infos
  sensor_t sensor;
  bmp.getTemperatureSensor()->getSensor(&sensor);
  logInfo("");
  logInfo("Temperature sensor informations:");
  logInfo(" - Sensor type: " + String(sensor.type));
  logInfo(" - Measure range: " + String(sensor.min_value) + " °C ~ +" + String(sensor.max_value) + "° C");
  logInfo(" - Measure resolution: ~" + String(sensor.resolution, 4) + " °C");

  // Print pressure sensor infos
  bmp.getPressureSensor()->getSensor(&sensor);
  logInfo("");
  logInfo("Pressure sensor informations:");
  logInfo(" - Sensor type: " + String(sensor.type));
  logInfo(" - Measure range: " + String(sensor.min_value) + " hPa ~ +" + String(sensor.max_value) + " hPa");
  logInfo(" - Measure resolution: ~" + String(sensor.resolution, 4) + " hPa");

  logInfo("");
  logInfo("Initialization success.", true);
  delay(100);
  return true; // Initialization success
}

float calculateSeaPressure(float altitude, float defaultPressure) {
  logInfo("", true);
  logInfo("Calculate pressure at sea level ... [default value: " + String(defaultPressure) + " hPa]");
  logInfo("");
  
  if (altitude == 0) {
    logInfo("No value was given. Using the reference file ...");
    
    if (exists(SEA_PRESSION_FILE, true)) {
      logInfo("The value in the file will be used as the current sea pressure.");
      float read = readFile(SEA_PRESSION_FILE).toFloat();
      
      if (read == 0) logError("No or invalid value in the file! Use the default value and creating new file ...");
      else {
        logInfo("Valid value found! '" + String(read) +"' hPa will be used as the current sea pressure.");
        return read;
      }
    
    } else logError("Reference file not found! Use the default value and creating the file ...");
    
    writeFile(SEA_PRESSION_FILE, String(defaultPressure));
    logInfo("", true);
    return defaultPressure;
    
  } else {
    logInfo("Given value found! Using the value to calculate pressure at sea level ...");
    
    float actualPressure = bmp1.readPressure(),
          seaPressure = bmp1.seaLevelForAltitude(altitude, actualPressure);
    logInfo("The pressure at sea level is " + String(seaPressure) + " hPa for a current pressure of " + String(actualPressure) + " hPa and an altitude of " + String(altitude) + " m");
    logInfo("This value wil be used and writen in the reference file ...");
    
    writeFile(SEA_PRESSION_FILE, String(seaPressure, 4));
    logInfo("", true);
    return seaPressure;
  }
}
