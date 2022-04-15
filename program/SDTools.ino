// Affiche les informations de la carte SD
void printCardInfos() {
  if (Serial) {
    FSInfo card;
    SDFS.info(card);
    uint8_t SDType = SD.type();
  
    logInfo("SD Card type: " + String(SDType == 0 ? "SD1" : SDType == 1 ? "SD2" : SDType == 2 ? "SDHC" : "<unknown>"));
    logInfo("FAT" + String(SD.fatType()) + " partition found.");
    logInfo("");
    logInfo("Cluster size: " + String(SD.clusterSize())); 
    logInfo("Blocks per cluster: " + String(SD.blocksPerCluster()));
    logInfo("Blocks size: " + String(SD.blockSize()));
    logInfo("Total blocks: " + String(SD.totalBlocks()));
    logInfo("Total Cluster: " + String(SD.totalClusters()));
    logInfo("");
    logInfo("Card size: " + String(SD.size()) + " bytes");
    logInfo("Total space: " + String(card.totalBytes) + " bytes");
    logInfo("Used space: " + String(card.usedBytes) + " bytes");
    logInfo("Note: the blocks of an SD card are always 512 bytes (so it takes 2 blocks to make 1KB)");  
  }
}

// Teste si le fichier/dossier existe
bool exists(const String path, bool isFile) {
  File file = SD.open(path);
  bool worked = file && (isFile ? !file.isDirectory() : file.isDirectory());
  
  if (worked) logInfo(String(isFile ? "File" : "Directory") + " '" + path + "' already exist.");
  else logError(String(isFile ? "File" : "Directory") + " '" + path + "' don't exist.");

  file.close();
  return worked;
}

// Créer un dossier
bool createDir(const String path){
  bool worked = SD.mkdir(path);
  
  if (worked) logInfo("Successful created directory: " + path);
  else logError("Failed to create directory: " + path);
  return worked;
}

// Supprime un dossier
bool removeDir(const String path){
  bool worked = SD.rmdir(path);
  
  if (worked) logInfo("Successful remove directory: " + path);
  else logError("Failed to remove directory: " + path);
  return worked;
}

// Lit les octets d'un fichier et renvoie sont contenu en texte
String readFile(const String path){
  File file = SD.open(path, "r");
  String output = "";
  
  if (file) {
    int read;
    while (file.available()) {
      read = file.read();
      if (read == -1) {
        logError("Failed to read all bytes in file at " + path);
        file.close();
        return output;
      } else output += (char) read;
    }
    logInfo("Successful read file at " + path);

  } else logError("Can't read file at " + path);
  
  file.close();
  return output;
}

// Créer un fichier vide
bool createFile(const String path, const String name) {
  File file = SD.open(path + (path.endsWith("/") ? "" : "/") + name, "w+");
  bool worked = file;
  
  if (worked) logInfo("File '" + name + "' created successfully at " + path);
  else logError("Failed to create file '" + name + "' at " + path);

  file.close();
  return worked;
}

// Ajoute/Remplace le contenu dans un fichier
bool writeFile(const String path, const String content){ return writeFile(path, content, false); }
bool writeFile(const String path, const String content, bool append){
  File file = SD.open(path, append ? "a" : "w+");
  bool worked = false;
  
  if (file) {
    if (file.write(content.c_str()) != 0) {
      logInfo("Successful write in file at " + path);
      worked = true;
     
    } else logError("The content was not fully written in file: " + path + "  (probably not enough space)");
  } else logError("Can't write in file at " + path);

  file.close();
  return worked;
}

// Renomme un fichier
bool renameFile(String path, const String name){
  bool worked = SD.rename(path, path.substring(0, path.lastIndexOf("/")+1) + name);
  
  if (worked) logInfo("File at '" + path + "' renamed to '" + name + "'");
  else logError("Failed to rename file at '" + path + "' to '" + name + "'");
  return worked;
}

// Supprime un fichier
bool deleteFile(const String path){
  bool worked = SD.remove(path);
  
  if (worked) logInfo("Successful delete file at " + path);
  else logError("Failed to delete file at " + path);
  return worked;
}
