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

### Einrichtung

1. Code auf den ESP32-CAM hochladen
2. WLAN-Daten im Code eintragen
3. Serielle Konsole öffnen und IP-Adresse ablesen
4. IP-Adresse im Browser öffnen

### Hinweise

* Die Kamerakonfiguration basiert auf Standard-Beispielcode der ESP32-CAM
* Verbesserungen sind möglich (bessere Benutzeroberfläche, flüssigeres Streaming, optimierte Steuerung)


