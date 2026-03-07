#ifndef WEBPAGES_H
#define WEBPAGES_H

#include <Arduino.h>

// HTML page with real-time vibration monitoring and WiFi configuration
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ro">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Senzor Vibrație KY-002</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.min.js"></script>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #262d50 0%, #180e22 100%);
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 1000px;
            margin: 0 auto;
            background: white;
            border-radius: 15px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            padding: 30px;
        }
        h1 {
            color: #132057;
            text-align: center;
            margin-bottom: 10px;
            font-size: 2.2em;
        }
        .subtitle {
            text-align: center;
            color: #666;
            margin-bottom: 30px;
            font-size: 0.9em;
        }
        .status-card {
            background: linear-gradient(135deg, #54c4a2 0%, #4b95a2 100%);
            color: white;
            padding: 25px;
            border-radius: 12px;
            margin-bottom: 25px;
            display: flex;
            justify-content: space-around;
            align-items: center;
            flex-wrap: wrap;
        }
        .status-item {
            text-align: center;
            padding: 10px;
        }
        .status-label {
            font-size: 0.85em;
            opacity: 0.9;
            margin-bottom: 5px;
        }
        .status-value {
            font-size: 2em;
            font-weight: bold;
        }
        .vibration-indicator {
            width: 80px;
            height: 80px;
            border-radius: 50%;
            margin: 0 auto;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 2.5em;
            transition: all 0.3s;
        }
        .vibration-active {
            background: #ef4444;
            box-shadow: 0 0 30px rgba(239, 68, 68, 0.6);
            animation: shake 0.5s infinite;
        }
        .vibration-inactive {
            background: #10b981;
            box-shadow: 0 0 20px rgba(16, 185, 129, 0.3);
        }
        @keyframes shake {
            0%, 100% { transform: translateX(0); }
            25% { transform: translateX(-5px) rotate(-5deg); }
            75% { transform: translateX(5px) rotate(5deg); }
        }
        .chart-container {
            position: relative;
            height: 350px;
            margin: 25px 0;
            background: #f8fafc;
            border-radius: 12px;
            padding: 20px;
        }
        .tabs {
            display: flex;
            gap: 10px;
            margin-bottom: 20px;
            border-bottom: 2px solid #e5e7eb;
        }
        .tab-button {
            padding: 12px 24px;
            background: none;
            border: none;
            border-bottom: 3px solid transparent;
            cursor: pointer;
            font-weight: 600;
            color: #64748b;
            transition: all 0.3s;
        }
        .tab-button.active {
            color: #667eea;
            border-bottom-color: #667eea;
        }
        .tab-button:hover {
            color: #667eea;
        }
        .tab-content {
            display: none;
        }
        .tab-content.active {
            display: block;
        }
        .form-group {
            margin-bottom: 15px;
        }
        .form-label {
            display: block;
            margin-bottom: 5px;
            font-weight: 600;
            color: #1e293b;
        }
        .form-input {
            width: 100%;
            padding: 10px;
            border: 2px solid #e5e7eb;
            border-radius: 8px;
            font-size: 1em;
        }
        .form-input:focus {
            outline: none;
            border-color: #667eea;
        }
        .btn {
            padding: 12px 24px;
            border: none;
            border-radius: 8px;
            font-weight: bold;
            cursor: pointer;
            font-size: 1em;
        }
        .btn-primary {
            background: #667eea;
            color: white;
        }
        .btn-danger {
            background: #ef4444;
            color: white;
        }
        .wifi-status {
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 20px;
            font-weight: 600;
        }
        .wifi-connected {
            background: #d1fae5;
            color: #065f46;
        }
        .wifi-disconnected {
            background: #fee2e2;
            color: #991b1b;
        }
        .info-box {
            background: #f0f9ff;
            border-left: 4px solid #667eea;
            padding: 15px;
            border-radius: 8px;
            margin-top: 20px;
        }
        .footer {
            text-align: center;
            margin-top: 20px;
            color: #64748b;
            font-size: 0.85em;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>📊 Monitor Vibrații KY-002</h1>
        <p class="subtitle">ESP32 - Monitorizare în timp real</p>
        
        <!-- Tabs -->
        <div class="tabs">
            <button class="tab-button active" onclick="switchTab('monitor')">📊 Monitorizare</button>
            <button class="tab-button" onclick="switchTab('wifi')">📡 WiFi</button>
        </div>
        
        <!-- Monitor Tab -->
        <div id="monitor-tab" class="tab-content active">
            <!-- Status Card -->
            <div class="status-card">
                <div class="status-item">
                    <div class="status-label">Status Senzor</div>
                    <div class="vibration-indicator vibration-inactive" id="indicator">
                        ✓
                    </div>
                    <div style="margin-top: 10px; font-size: 1.2em;" id="statusText">Fără vibrații</div>
                </div>
                <div class="status-item">
                    <div class="status-label">Total evenimente</div>
                    <div class="status-value" id="count">0</div>
                </div>
                <div class="status-item">
                    <div class="status-label">Ultima detecție</div>
                    <div class="status-value" style="font-size: 1.2em;" id="lastTime">-</div>
                </div>
            </div>
            
            <!-- Chart -->
            <div class="chart-container">
                <canvas id="vibrationChart"></canvas>
            </div>
            
            <div class="info-box">
                <strong>ℹ️ Informații:</strong>
                <ul style="margin-left: 20px; margin-top: 10px; line-height: 1.8;">
                    <li>Senzor: KY-002 Vibration Switch Module</li>
                    <li>Pin GPIO: 14</li>
                    <li>Sampling: 100 Hz (la fiecare 10ms)</li>
                    <li>Grafic: ultimele 100 de citiri (1 secundă)</li>
                    <li>Actualizare web: la fiecare 300ms</li>
                </ul>
            </div>
        </div>
        
        <!-- WiFi Configuration Tab -->
        <div id="wifi-tab" class="tab-content">
            <div id="wifiStatus" class="wifi-status wifi-disconnected">
                Mod: Access Point | IP: <span id="currentIP">-</span>
            </div>
            
            <h3 style="margin-bottom: 15px; color: #1e293b;">Conectare la WiFi</h3>
            <p style="margin-bottom: 20px; color: #64748b;">Configurează ESP32 să se conecteze la rețeaua ta WiFi.</p>
            
            <div class="form-group">
                <label class="form-label">SSID (Nume rețea WiFi)</label>
                <input type="text" id="wifiSSID" class="form-input" placeholder="Nume rețea WiFi">
            </div>
            
            <div class="form-group">
                <label class="form-label">Parolă WiFi</label>
                <input type="password" id="wifiPassword" class="form-input" placeholder="Parolă WiFi">
            </div>
            
            <div style="display: flex; gap: 10px; margin-top: 20px;">
                <button onclick="saveWiFiConfig()" class="btn btn-primary" style="flex: 1;">
                    💾 Salvează și conectează
                </button>
                <button onclick="clearWiFiConfig()" class="btn btn-danger">
                    🗑️ Șterge
                </button>
            </div>
            
            <div id="wifiMessage" style="margin-top: 15px; padding: 10px; border-radius: 8px; text-align: center; display: none;"></div>
            
            <div class="info-box">
                <h4 style="margin-bottom: 10px; color: #1e293b;">ℹ️ Informații</h4>
                <ul style="margin-left: 20px; line-height: 1.8; color: #64748b;">
                    <li>Configurația este salvată în EEPROM</li>
                    <li>ESP32 va încerca conectarea la pornire</li>
                    <li>Dacă nu reușește, va activa Access Point</li>
                    <li>Access Point: <strong>ESP32_VibrationSensor</strong> / <strong>12345678</strong></li>
                </ul>
            </div>
        </div>

        <div class="footer">
            ESP32 + KY-002 Vibration Sensor
        </div>
    </div>

    <script>
        // Chart.js configuration
        const ctx = document.getElementById('vibrationChart').getContext('2d');
        const chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Semnal Vibrație',
                    data: [],
                    borderColor: '#667eea',
                    backgroundColor: 'rgba(102, 126, 234, 0.1)',
                    borderWidth: 2,
                    fill: true,
                    tension: 0.1,
                    pointRadius: 2,
                    pointHoverRadius: 4
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                animation: {
                    duration: 0 // Disable animation for real-time update
                },
                scales: {
                    y: {
                        beginAtZero: true,
                        max: 1.2,
                        ticks: {
                            stepSize: 1,
                            callback: function(value) {
                                return value === 1 ? 'VIBRAȚIE' : (value === 0 ? 'OK' : '');
                            }
                        },
                        grid: {
                            color: 'rgba(0, 0, 0, 0.05)'
                        }
                    },
                    x: {
                        display: false
                    }
                },
                plugins: {
                    legend: {
                        display: true,
                        position: 'top'
                    },
                    tooltip: {
                        callbacks: {
                            label: function(context) {
                                return context.parsed.y === 1 ? 'Vibrație detectată' : 'Fără vibrații';
                            }
                        }
                    }
                }
            }
        });
        
        function updateData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    // Update status card
                    document.getElementById('count').textContent = data.count;
                    document.getElementById('lastTime').textContent = data.lastTime;
                    
                    // Update indicator
                    const indicator = document.getElementById('indicator');
                    const statusText = document.getElementById('statusText');
                    
                    if (data.state === 1) {
                        indicator.className = 'vibration-indicator vibration-active';
                        indicator.textContent = '⚠';
                        statusText.textContent = 'VIBRAȚIE!';
                    } else {
                        indicator.className = 'vibration-indicator vibration-inactive';
                        indicator.textContent = '✓';
                        statusText.textContent = 'Fără vibrații';
                    }
                    
                    // Update chart with history data from server
                    if (data.history && data.history.length > 0) {
                        chart.data.labels = Array.from({length: data.history.length}, (_, i) => i);
                        chart.data.datasets[0].data = data.history;
                        chart.update('none'); // Update without animation
                    }
                })
                .catch(error => {
                    console.error('Eroare actualizare:', error);
                });
        }
        
        // Update every 300ms
        setInterval(updateData, 300);
        updateData();
        
        // Tab switching
        function switchTab(tabName) {
            document.querySelectorAll('.tab-content').forEach(tab => {
                tab.classList.remove('active');
            });
            document.querySelectorAll('.tab-button').forEach(btn => {
                btn.classList.remove('active');
            });
            
            document.getElementById(tabName + '-tab').classList.add('active');
            event.target.classList.add('active');
            
            if (tabName === 'wifi') {
                updateWiFiStatus();
            }
        }
        
        // WiFi functions
        function updateWiFiStatus() {
            fetch('/wifi_status')
                .then(response => response.json())
                .then(data => {
                    const statusDiv = document.getElementById('wifiStatus');
                    document.getElementById('currentIP').textContent = data.ip;
                    
                    if (data.connected) {
                        statusDiv.className = 'wifi-status wifi-connected';
                        statusDiv.innerHTML = '✅ Conectat la: <strong>' + data.ssid + '</strong> | IP: ' + data.ip;
                    } else {
                        statusDiv.className = 'wifi-status wifi-disconnected';
                        statusDiv.innerHTML = '📡 Mod: Access Point | IP: ' + data.ip;
                    }
                    
                    if (data.saved_ssid) {
                        document.getElementById('wifiSSID').value = data.saved_ssid;
                    }
                })
                .catch(error => console.error('Eroare:', error));
        }
        
        function saveWiFiConfig() {
            const ssid = document.getElementById('wifiSSID').value;
            const password = document.getElementById('wifiPassword').value;
            
            if (!ssid) {
                showWiFiMessage('Introduceți SSID!', 'error');
                return;
            }
            
            showWiFiMessage('Se salvează configurația...', 'info');
            
            fetch('/wifi_config', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: 'ssid=' + encodeURIComponent(ssid) + '&password=' + encodeURIComponent(password)
            })
            .then(response => response.json())
            .then(data => {
                showWiFiMessage(data.message, data.success ? 'success' : 'error');
                if (data.success) {
                    setTimeout(() => {
                        showWiFiMessage('ESP32 se va restarta în 3 secunde...', 'info');
                    }, 2000);
                }
            })
            .catch(error => {
                showWiFiMessage('Eroare la salvare!', 'error');
            });
        }
        
        function clearWiFiConfig() {
            if (confirm('Sigur doriți să ștergeți configurația WiFi?')) {
                fetch('/wifi_clear')
                    .then(response => response.json())
                    .then(data => {
                        showWiFiMessage(data.message, 'success');
                        document.getElementById('wifiSSID').value = '';
                        document.getElementById('wifiPassword').value = '';
                        setTimeout(() => updateWiFiStatus(), 1000);
                    })
                    .catch(error => {
                        showWiFiMessage('Eroare la ștergere!', 'error');
                    });
            }
        }
        
        function showWiFiMessage(text, type) {
            const msg = document.getElementById('wifiMessage');
            msg.textContent = text;
            msg.style.display = 'block';
            
            if (type === 'success') {
                msg.style.background = '#d1fae5';
                msg.style.color = '#065f46';
            } else if (type === 'error') {
                msg.style.background = '#fee2e2';
                msg.style.color = '#991b1b';
            } else {
                msg.style.background = '#dbeafe';
                msg.style.color = '#1e40af';
            }
            
            if (type !== 'info') {
                setTimeout(() => { msg.style.display = 'none'; }, 5000);
            }
        }
    </script>
</body>
</html>
)rawliteral";

#endif // WEBPAGES_H
