/* IMPORTATIONS */
#include <SD.h> // Librairie pour la carte SD
#include <Adafruit_MPU6050.h> // Librairie pour le Gyroscope/Accéléromètre
#include <Adafruit_BMP280.h> // Librairie pour le Baromètre


/* CONSTANTES */
#define BAUD_RATE 74880 // Vitesse de transmission
#define ROOT_DIRECTORY "/black-box-logs" // Répértoire d'écriture principale
#define FILE_EXTENSION ".csv" // Extension de fichier à utiliser
#define PRELAUNCH_FILE "pre-launch-logs" FILE_EXTENSION // Nom du fichier des tests avant lancement
#define LOGS_FILE "launch-logs" FILE_EXTENSION // Nom du fichier des tests pendant le lancement
#define SEA_PRESSION_FILE ROOT_DIRECTORY "/sea-pression.txt" // Fichier de référence pour la pression au niveau de la mer
#define MAX_OPERATION_TIME 100 // Temps maximun a respecter entre chaque boucle
#define PRELAUNCH_TESTS 50 // Nombres de tests avant le lancement
#define BUZZER 16 // Broche du buzzer


/* VARIABLES */
String filesPath; // Chemin vers les fichiers
float seaPressure = 1013.25, // Pression au niveau de la mer (valeur par défaut)
      altitude = 0; // L'altitude qui servira au calcul de la pression au niveau de la mer 
sensors_event_t accelSensor, // Objet 'event' pour l'accéléromètre
                gyroSensor, // Objet 'event' pour le gyroscope
                tempSensor; // Objet 'event' pour le thermomètre

/* OBJETS */
Adafruit_MPU6050 mpu; // Création de l'objet pour le gyroscope/accéléromètre
Adafruit_BMP280 bmp1; // Création de l'objet pour le baromètre 1
Adafruit_BMP280 bmp2; // Création de l'objet pour le baromètre 2 


/* INITIALISATION */
void setup() {
  Serial.begin(BAUD_RATE); // Initialisation du port série (pour le débugage)
  Serial.setTimeout(5000);
  
  logInfo("########## Begin of main::setup() ##########");
  logInfo("Wait 5s before starting ... You can type a new altitude to calculate pressure at sea level"); // Attend 10s avant de lancer le programme (le temps d'entrer la nouvelle altitude)
  float reads = Serial.readString().toFloat();
  if (reads == 0) logError("No or invalid value was given! Skipping this step ...");
  else {
    altitude = reads;
    logInfo("A valid value found! '" + String(altitude) + "' m will be used to calculate pression at sea level");
  }
  
  pinMode(BUZZER, OUTPUT); // Initialisation du buzzer en prioritaire
  bip(5, 200); // Signale à l'utilisateur le début de l'initialisation

  logInfo("Devices Initialization ...");

  if (!initSDCard(SS)) failedSignal(); // Initialisation de la carte CD sur le port SS (15)
  if (!initMPU(MPU6050_DEVICE_ID, mpu)) failedSignal(); // Initialisation du gyroscope/acceléromètre
  // Initialisation des baromètres/altimètrse
  if (!initBMP(BMP280_ADDRESS_ALT, bmp1)) failedSignal();
  if (!initBMP(BMP280_ADDRESS, bmp2)) failedSignal();

  // Initialise l'environement de fichier et renvoie le path a utiliser
  filesPath = initFilesEnvironement(ROOT_DIRECTORY);
  if (filesPath == "") failedSignal();

  // Calcul de la pression au niveau de la mer si une altitude a été entrée ou fichier présent
  seaPressure = calculateSeaPressure(altitude, seaPressure);

  // Créé le fichier pour les tests avant lancement
  logInfo("");
  logInfo("Creating file for pre-launch tests ...");
  if (!createFile(filesPath, PRELAUNCH_FILE)) failedSignal();
  if (!writeFileHeader(filesPath+PRELAUNCH_FILE)) failedSignal();

  // Tests de pré-lancement
  logInfo("--> Pre-launch tests started ...");
  for (int i=0; i<PRELAUNCH_TESTS; i++) getAndWriteSensorsEvents(filesPath+PRELAUNCH_FILE, MAX_OPERATION_TIME);

  // Créé le fichier qui stockera les données lors du vol
  logInfo("");
  logInfo("--> Pre-launch tests finished.");  
  logInfo("Creating file for launch logs ...");
  if (!createFile(filesPath, LOGS_FILE)) failedSignal();
  if (!writeFileHeader(filesPath+LOGS_FILE)) failedSignal();

  logInfo("########## End of main::setup() ##########");
  bip(3, 700); // Signale à l'utilisateur la fin de l'initialisation
}


/* BOUCLE PRINCIPALE */
void loop() {
  logInfo("########## Begin of main::loop() ##########");

  getAndWriteSensorsEvents(filesPath+LOGS_FILE, MAX_OPERATION_TIME);
  logInfo("");
  
  logInfo("########## End of main::loop() ##########");
}


/* FONCTIONS ADDITIONNELLES */
/*  Les fonctions utiles au programme sont séparées dans plusieurs fichier, selon leurs types d'utilisation, pour une meilleur lisibilité du code.
 *  
 *  Les fonctions liées à l'initialisation des différents capteurs, et du programme, se situent dans le fichier "initTools.ino"
 *  Les fonctions liées à la gestion de la carte SD se situent dans le fichier "SDTools.ino"
 *  Les autres fonctions sont quand à elles situées dans le fichier "unil.ino"
 */
