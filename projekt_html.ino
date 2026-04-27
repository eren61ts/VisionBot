//Projekt VisionBot
//Dieser Code beinhaltet alle Funktionen inklusive die HTML-Seite zum Ausführen des Projektes
//Zur initialisierung muss das Board Esp32 by espressif Systems heruntergeladen werden.
//Als Board muss AI Thinker ESP32-CAM ausgewählt werden. 

//Bibliotheken einbinden (Sollte im Board Esp32 by espressif Systems enthalten sein) 
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
// WLAN
const char* ssid = "Deine_SSID";  //Hier Wlan-SSID hinzufügen
const char* password = "Dein_Passwort"; //Hier Wlan-Passwort hinzufügen
// ===== MOTOR PINS (anpassen falls nötig) =====
#define IN1 2
#define IN2 14
#define IN3 15
#define IN4 13
// ===== CAMERA PINS AI THINKER =====
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

WebServer server(80);   //WebServer auf Port 80 starten

// Zunächst alle Funktionen deklarieren damit sie aufrufbar sind 
void handleRoot();
void handleStream();
void stopMotors();
void forward();
void back();
void left();
void right();
void forwardLeft();
void forwardRight();
void backLeft();
void backRight();

// Setup - Initialisierung
void setup() {
  Serial.begin(115200);   // baut eine Serielle Verbindung mit Bautrate 115200 auf (Im Serial Monitor auf gleichem Wert einstellen!!)
  // Pins als OUTPUT (Ausgang) initialisieren
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();   // Motoren zum Anfang ausschalten, falls sie noch an sein sollten.

  // Initialisierung der Kamera - aus den Beispielen unter Esp32 -> Camera -> CameraWebServer
  // sorgt hauptsächlich für die Pins-Definition, Datenverkehr und Kommunikation zwischen der Kamera und Esp32
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;   //Bilder über JPEG-Format senden

  // kontrolliert ob psram enabled ist, wenn nicht schwächt es die Bildqualität
  if (psramFound()) {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 20;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 25;
    config.fb_count = 1;
  }

  // Überprüfung ob die Kamera ohne Probleme initialisiert wurde
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Kamera Fehler!");
    return;
  }

  // Einstellung zum drehen des Kamera-Bildes - Wenn Kamera normalerweise richtig montiert ist, sollte das nicht nötig sein
  sensor_t* s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);


  WiFi.begin(ssid, password);   //versucht eine verbindung mit dem oben eingegeben SSID und Passwort herzustellen
  while (WiFi.status() != WL_CONNECTED) {   // prüft ob mit dem Wlan verbunden ist solange die Verbindung erfolgt.
    delay(500);
  }
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());   //gibt die IP des Esp32 aus

  // Das ist das Herzstück. Wenn folgende URL's aufgerufen werden, werden die dazu gehörigen Funktionen aufgeruft
  server.on("/", handleRoot);
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/forward", []() { forward(); server.send(200, "text/plain", "OK"); });
  server.on("/back", []() { back(); server.send(200, "text/plain", "OK"); });
  server.on("/left", []() { left(); server.send(200, "text/plain", "OK"); });
  server.on("/right", []() { right(); server.send(200, "text/plain", "OK"); });
  server.on("/forward-left", []() { forwardLeft(); server.send(200, "text/plain", "OK"); });
  server.on("/forward-right", []() { forwardRight(); server.send(200, "text/plain", "OK"); });
  server.on("/back-left", []() { backLeft(); server.send(200, "text/plain", "OK"); });
  server.on("/back-right", []() { backRight(); server.send(200, "text/plain", "OK"); });
  server.on("/stop", []() { stopMotors(); server.send(200, "text/plain", "OK"); });
  server.begin(); //startet den Webserver
}

// loop - Endlosschleife
void loop() {
  server.handleClient();    // erkennt Anfragen und verarbeitet diese, befindet sich in der WebServer Bibliothek
}

// Funktion zum aufrufen der kompletten Seite. Ohne dieses würde nichts auf der Webseite sichtbar sein. Beinhaltet HTML, CSS und Javascript 
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>VisionBot</title>
  <style>
    :root {
      --bg: #f4f1e8;
      --panel: #fffaf0;
      --accent: #146356;
      --accent-strong: #0e4b41;
      --danger: #b8402a;
      --text: #1e1e1e;
      --shadow: 0 10px 30px rgba(20, 99, 86, 0.18);
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      font-family: Verdana, sans-serif;
      background: linear-gradient(180deg, #efe7d6 0%, #f7f4ee 100%);
      color: var(--text);
      display: flex;
      justify-content: center;
      padding: 16px;
    }
    .wrap {
      width: 100%;
      max-width: 760px;
      background: var(--panel);
      border-radius: 20px;
      box-shadow: var(--shadow);
      padding: 18px;
    }
    h2 {
      margin: 0 0 8px;
      text-align: center;
    }
    p {
      text-align: center;
      margin: 0 0 14px;
    }
    img {
      width: 100%;
      max-width: 480px;
      aspect-ratio: 4 / 3;
      display: block;
      margin: 0 auto 16px;
      border-radius: 14px;
      background: #d8d8d8;
    }
    .grid {
      display: grid;
      grid-template-columns: repeat(3, minmax(80px, 1fr));
      gap: 10px;
      max-width: 420px;
      margin: 0 auto;
    }
    button {
      min-height: 70px;
      border: none;
      border-radius: 16px;
      background: var(--accent);
      color: white;
      font-size: 20px;
      font-weight: 700;
      box-shadow: 0 6px 16px rgba(20, 99, 86, 0.25);
      touch-action: manipulation;
    }
    button:active,
    button.active {
      transform: scale(0.97);
      background: var(--accent-strong);
    }
    .stop {
      background: var(--danger);
    }
    .hint {
      margin-top: 14px;
      font-size: 14px;
      opacity: 0.8;
    }
  </style>
</head>
<body>
  <div class="wrap">
    <h2>VisionBot</h2>
    <p>Gedrueckt halten zum Fahren. Tastatur: W A S D, diagonal mit WA, WD, SA, SD.</p>
    <img id="stream" src="/stream?t=0" alt="Kamera Stream">
    <div class="grid">
      <button data-command="forward-left">↖</button>
      <button data-command="forward">↑</button>
      <button data-command="forward-right">↗</button>
      <button data-command="left">←</button>
      <button class="stop" data-command="stop">STOP</button>
      <button data-command="right">→</button>
      <button data-command="back-left">↙</button>
      <button data-command="back">↓</button>
      <button data-command="back-right">↘</button>
    </div>
    <p class="hint">Loslassen stoppt sofort.</p>
  </div>
  <script>
    const stream = document.getElementById('stream');
    const buttons = Array.from(document.querySelectorAll('button[data-command]'));
    const activeKeys = new Set();
    let activeCommand = 'stop';
    function resolveCommand() {
      const w = activeKeys.has('w');
      const a = activeKeys.has('a');
      const s = activeKeys.has('s');
      const d = activeKeys.has('d');
      if (w && a) return 'forward-left';
      if (w && d) return 'forward-right';
      if (s && a) return 'back-left';
      if (s && d) return 'back-right';
      if (w) return 'forward';
      if (s) return 'back';
      if (a) return 'left';
      if (d) return 'right';
      return 'stop';
    }
    function sendCommand(command) {
      if (command === activeCommand) return;
      activeCommand = command;
      fetch('/' + command).catch(() => {});
      buttons.forEach((button) => {
        button.classList.toggle('active', button.dataset.command === command && command !== 'stop');
      });
    }
    function pressCommand(command) {
      sendCommand(command);
    }
    function releaseCommand(command) {
      if (activeCommand === command) {
        sendCommand('stop');
      }
    }
    buttons.forEach((button) => {
      const command = button.dataset.command;
      if (command === 'stop') {
        button.addEventListener('click', () => {
          activeKeys.clear();
          sendCommand('stop');
        });
        return;
      }
      const start = (event) => {
        event.preventDefault();
        pressCommand(command);
      };
      const end = (event) => {
        event.preventDefault();
        releaseCommand(command);
      };
      button.addEventListener('mousedown', start);
      button.addEventListener('mouseup', end);
      button.addEventListener('mouseleave', end);
      button.addEventListener('touchstart', start, { passive: false });
      button.addEventListener('touchend', end);
      button.addEventListener('touchcancel', end);
    });
    document.addEventListener('keydown', (event) => {
      const key = event.key.toLowerCase();
      if (!['w', 'a', 's', 'd'].includes(key)) return;
      event.preventDefault();
      activeKeys.add(key);
      sendCommand(resolveCommand());
    });
    document.addEventListener('keyup', (event) => {
      const key = event.key.toLowerCase();
      if (!['w', 'a', 's', 'd'].includes(key)) return;
      event.preventDefault();
      activeKeys.delete(key);
      sendCommand(resolveCommand());
    });
    window.addEventListener('blur', () => {
      activeKeys.clear();
      sendCommand('stop');
    });
    setInterval(() => {
      stream.src = '/stream?t=' + Date.now();
    }, 150);
  </script>
</body>
</html>
  )rawliteral";
  server.send(200, "text/html", html);
}

// Funktion zum Anzeigen des Live-Bilds
void handleStream() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Camera failed");
    return;
  }
  server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

// Zuletzt die Bewegungsrichtungen 
// Falls invertiert: HIGH/LOW je Richtung tauschen
void forward() {    // vorwärts
  Serial.println("Vorwärts");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void back() {       // rückwärts
  Serial.println("Rückwärts");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void left() {     // links auf der Stelle
  Serial.println("links");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void right() {      // rechts auf der Stelle
  Serial.println("rechts");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void forwardLeft() {      // links + bewegung nach vorne
  Serial.println("Vorne Links");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void forwardRight() {     // rechts + bewegung nach vorne
  Serial.println("Vorne Rechts");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void backLeft() {     // links + bewegung nach hinten
  Serial.println("Hinten Links");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void backRight() {      // rechts + bewegung nach hinten
  Serial.println("Hinten Rechts");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void stopMotors() {     // Stopp
  Serial.println("Stopp");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}