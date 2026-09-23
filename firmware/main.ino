#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

const int relayPin = 23;
bool relayState = false;
int maxDelaySeconds = 30; // Default auto-off timer value

const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AuraLight — Smart ESP32 Control</title>
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;700&display=swap" rel="stylesheet">
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Outfit', sans-serif; }
        body { background: #0d1117; color: #f0f6fc; display: flex; justify-content: center; align-items: center; min-height: 100vh; overflow: hidden; position: relative; }
        .ambient-glow { position: absolute; width: 400px; height: 400px; background: linear-gradient(135deg, #6366f1 0%, #a855f7 100%); filter: blur(120px); opacity: 0.25; z-index: -1; }
        .dashboard-card { background: rgba(22, 27, 34, 0.75); backdrop-filter: blur(16px); -webkit-backdrop-filter: blur(16px); border: 1px solid rgba(255, 255, 255, 0.1); padding: 32px; border-radius: 24px; box-shadow: 0 20px 40px rgba(0, 0, 0, 0.4); width: 360px; text-align: center; }
        .card-header .logo-badge { font-size: 1.8rem; background: rgba(99, 102, 241, 0.15); width: 50px; height: 50px; display: flex; align-items: center; justify-content: center; border-radius: 14px; margin: 0 auto 12px; border: 1px solid rgba(99, 102, 241, 0.3); }
        .card-header h1 { font-size: 1.5rem; font-weight: 700; }
        .subtitle { font-size: 0.85rem; color: #8b949e; margin-bottom: 24px; }
        .status-section { margin-bottom: 28px; }
        .status-indicator-ring { width: 90px; height: 90px; margin: 0 auto 10px; border-radius: 50%; display: flex; align-items: center; justify-content: center; background: conic-gradient(from 0deg, rgba(239, 68, 68, 0.2), rgba(239, 68, 68, 0.8)); transition: all 0.4s ease; }
        .status-indicator-ring.active { background: conic-gradient(from 0deg, rgba(34, 197, 94, 0.2), rgba(34, 197, 94, 0.9)); box-shadow: 0 0 25px rgba(34, 197, 94, 0.3); }
        .status-core { width: 76px; height: 76px; background: #161b22; border-radius: 50%; display: flex; align-items: center; justify-content: center; }
        #relayStatusText { font-weight: 700; font-size: 1.1rem; }
        #relayStatusText.off { color: #ef4444; }
        #relayStatusText.on { color: #22c55e; }
        .status-label { font-size: 0.8rem; color: #8b949e; text-transform: uppercase; letter-spacing: 1px; }
        .control-group { margin-bottom: 20px; text-align: left; }
        .label-flex { display: flex; justify-content: space-between; align-items: center; font-size: 0.9rem; color: #c9d1d9; margin-bottom: 8px; }
        .badge-time { background: rgba(99, 102, 241, 0.2); color: #a5b4fc; padding: 2px 8px; border-radius: 6px; font-size: 0.8rem; font-weight: 600; }
        input[type="range"] { width: 100%; accent-color: #6366f1; cursor: pointer; background: #30363d; height: 6px; border-radius: 3px; outline: none; margin-bottom: 12px; }
        button { width: 100%; padding: 12px; border: none; border-radius: 12px; font-size: 0.95rem; font-weight: 600; cursor: pointer; transition: all 0.25s ease; }
        .btn-primary { background: #6366f1; color: white; box-shadow: 0 4px 12px rgba(99, 102, 241, 0.3); }
        .btn-primary.active-state { background: #ef4444; box-shadow: 0 4px 12px rgba(239, 68, 68, 0.3); }
        .btn-secondary { background: rgba(255, 255, 255, 0.05); color: #c9d1d9; border: 1px solid rgba(255, 255, 255, 0.1); }
        .btn-secondary:hover { background: rgba(255, 255, 255, 0.1); color: white; }
    </style>
</head>
<body>
    <div class="ambient-glow"></div>
    <main class="dashboard-card">
        <header class="card-header">
            <div class="logo-badge">⚡</div>
            <h1>AuraLight</h1>
            <p class="subtitle">ESP32 Intelligent Automation</p>
        </header>
        <section class="status-section">
            <div class="status-indicator-ring" id="statusRing">
                <div class="status-core">
                    <span id="relayStatusText" class="off">OFF</span>
                </div>
            </div>
            <p class="status-label">Current Light State</p>
        </section>
        <section class="control-group">
            <button id="toggleBtn" class="btn-primary" onclick="toggleRelay()">Turn ON System</button>
        </section>
        <section class="control-group timer-box">
            <div class="label-flex">
                <label for="delaySlider">Timer / Auto-Off Delay</label>
                <span class="badge-time" id="delayValDisplay">30s</span>
            </div>
            <input type="range" id="delaySlider" min="5" max="300" step="5" value="30" oninput="updateDelayLabel(this.value)">
            <button class="btn-secondary" onclick="setTimer()">Apply Timer Configuration</button>
        </section>
    </main>
    <script>
        function formatTime(seconds) {
            if (seconds < 60) return seconds + "s";
            const mins = Math.floor(seconds / 60);
            const secs = seconds % 60;
            return secs === 0 ? `${mins}m` : `${mins}m ${secs}s`;
        }
        function updateDelayLabel(val) { document.getElementById('delayValDisplay').innerText = formatTime(parseInt(val)); }
        function fetchStatus() {
            fetch('/status').then(res => res.json()).then(data => {
                const statusText = document.getElementById('relayStatusText');
                const statusRing = document.getElementById('statusRing');
                const toggleBtn = document.getElementById('toggleBtn');
                if (data.state) {
                    statusText.innerText = "ON"; statusText.className = "on";
                    statusRing.classList.add('active');
                    toggleBtn.innerText = "Turn OFF System"; toggleBtn.classList.add('active-state');
                } else {
                    statusText.innerText = "OFF"; statusText.className = "off";
                    statusRing.classList.remove('active');
                    toggleBtn.innerText = "Turn ON System"; toggleBtn.classList.remove('active-state');
                }
            });
        }
        function toggleRelay() { fetch('/toggle').then(() => fetchStatus()); }
        function setTimer() {
            const delay = document.getElementById('delaySlider').value;
            fetch(`/setDelay?val=${delay}`).then(() => {
                const btn = document.querySelector('.timer-box .btn-secondary');
                const originalText = btn.innerText;
                btn.innerText = "✓ Timer Updated Successfully!";
                btn.style.color = "#22c55e";
                setTimeout(() => { btn.innerText = originalText; btn.style.color = ""; }, 2000);
            });
        }
        setInterval(fetchStatus, 2000);
        window.onload = fetchStatus;
    </script>
</body>
</html>
)rawliteral";

void handleRoot() { server.send(200, "text/html", MAIN_page); }
void handleToggle() {
  relayState = !relayState;
  digitalWrite(relayPin, relayState ? HIGH : LOW);
  server.send(200, "text/plain", "OK");
}
void handleStatus() {
  String json = "{\"state\":" + String(relayState ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}
void handleSetDelay() {
  if (server.hasArg("val")) {
    maxDelaySeconds = server.arg("val").toInt();
    server.send(200, "text/plain", "Updated");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/status", handleStatus);
  server.on("/setDelay", handleSetDelay);
  server.begin();
}

void loop() {
  server.handleClient();
}
