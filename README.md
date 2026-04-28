## VisionBot – ESP32-CAM Roboter

Dieses Projekt ist ein WLAN-gesteuerter Roboter auf Basis des ESP32-CAM.
Der Roboter kann über eine Weboberfläche gesteuert werden und liefert gleichzeitig ein Live-Kamerabild im Browser.

### Funktionen

* Steuerung über den Webbrowser
* Live-Videoübertragung (ESP32-CAM)
* Bewegung in mehrere Richtungen (vorwärts, rückwärts, links, rechts, diagonal)
* Einfache HTML-basierte Benutzeroberfläche

### Hardware

* ESP32-CAM
* L298N Motortreiber
* 4 DC-Motoren (4WD-Chassis)
* 18650 Akkus

### Funktionsweise

Der ESP32 verbindet sich mit einem WLAN und startet einen Webserver.
Über die Webseite können Steuerbefehle gesendet werden.
Diese werden vom ESP32 verarbeitet und in Motorbewegungen umgesetzt.
Gleichzeitig wird das Kamerabild live an die Webseite übertragen.

## Voraussetzungen
## andere versionen könnten auch klappen, aber dies sind die empfohlenen.
- Arduino IDE 2.3.8
- esp32 by Espressiv Systems Boardpaket Version 3.3.8 (Espressif Systems)

### Einrichtung
1. Arduino IDE von der offiziellen Webseite herunterladen (achte auf die Version)
 - https://www.arduino.cc/en/software/#ide
2. Den Board esp32 by Esspressiv Systems herunterladen (ebenfalls auf Version achten)
3. Code auf den ESP32-CAM hochladen  (upload)
4. WLAN-Daten im Code eintragen 
5. Serielle Konsole öffnen und IP-Adresse ablesen
6. IP-Adresse im Browser öffnen  (Sicherstellen, dass du im selbem Netz bist!)

### Hinweise

* Die Kamerakonfiguration basiert auf Standard-Beispielcode der ESP32-CAM
* Verbesserungen sind möglich (bessere Benutzeroberfläche, flüssigeres Streaming, optimierte Steuerung)


