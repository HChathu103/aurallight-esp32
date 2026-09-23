function formatTime(seconds) {
    if (seconds < 60) {
        return seconds + "s";
    }
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return secs === 0 ? `${mins}m` : `${mins}m ${secs}s`;
}

function updateDelayLabel(val) {
    document.getElementById('delayValDisplay').innerText = formatTime(parseInt(val));
}

function fetchStatus() {
    fetch('/status')
        .then(res => res.json())
        .then(data => {
            const statusText = document.getElementById('relayStatusText');
            const statusRing = document.getElementById('statusRing');
            const toggleBtn = document.getElementById('toggleBtn');
            
            if (data.state) {
                statusText.innerText = "ON";
                statusText.className = "on";
                statusRing.classList.add('active');
                toggleBtn.innerText = "Turn OFF System";
                toggleBtn.classList.add('active-state');
            } else {
                statusText.innerText = "OFF";
                statusText.className = "off";
                statusRing.classList.remove('active');
                toggleBtn.innerText = "Turn ON System";
                toggleBtn.classList.remove('active-state');
            }
        })
        .catch(err => console.error('Sync error:', err));
}

function toggleRelay() {
    fetch('/toggle')
        .then(() => fetchStatus())
        .catch(err => console.error('Toggle error:', err));
}

function setTimer() {
    const delay = document.getElementById('delaySlider').value;
    fetch(`/setDelay?val=${delay}`)
        .then(res => res.text())
        .then(() => {
            // Subtle visual feedback toast/alert replacement
            const btn = document.querySelector('.timer-box .btn-secondary');
            const originalText = btn.innerText;
            btn.innerText = "✓ Timer Updated Successfully!";
            btn.style.color = "#22c55e";
            setTimeout(() => {
                btn.innerText = originalText;
                btn.style.color = "";
            }, 2000);
        })
        .catch(err => console.error('Timer error:', err));
}

// Poll state every 2 seconds
setInterval(fetchStatus, 2000);
window.onload = fetchStatus;