#pragma once
#include <pgmspace.h>

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>Zowi Control</title>
<style>
*{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent}
:root{
  --bg:#0d1117;--surf:#161b22;--surf2:#21262d;
  --green:#3fb950;--blue:#58a6ff;--red:#f85149;
  --yellow:#e3b341;--text:#e6edf3;--muted:#8b949e;
  --border:#30363d;--radius:10px;
}
body{background:var(--bg);color:var(--text);font-family:system-ui,sans-serif;
  min-height:100vh;display:flex;flex-direction:column;user-select:none}
h3{font-size:.75rem;text-transform:uppercase;letter-spacing:.08em;
  color:var(--muted);margin-bottom:8px}

/* STATUS BAR */
#status-bar{background:var(--surf);border-bottom:1px solid var(--border);
  padding:8px 12px;display:flex;flex-wrap:wrap;gap:10px;align-items:center}
.pill{padding:3px 10px;border-radius:20px;font-size:.78rem;font-weight:600;
  background:var(--surf2);border:1px solid var(--border)}
.pill span{font-weight:400;color:var(--muted)}
#bt-pill{border-color:var(--red)}
#bt-pill.on{border-color:var(--green)}
.dot{display:inline-block;width:8px;height:8px;border-radius:50%;
  background:var(--red);margin-right:4px;transition:background .3s}
.dot.on{background:var(--green)}
#bat-val{color:var(--green)}
#dist-val,#noise-val{color:var(--blue)}

/* MAIN GRID */
main{flex:1;padding:12px;display:flex;flex-direction:column;gap:12px;max-width:600px;margin:0 auto;width:100%}

/* CARD */
.card{background:var(--surf);border:1px solid var(--border);border-radius:var(--radius);padding:14px}

/* DPAD */
.dpad{display:grid;grid-template-columns:repeat(3,1fr);gap:6px;max-width:240px;margin:0 auto}
.dpad .empty{background:transparent;pointer-events:none}
.btn{background:var(--surf2);border:1px solid var(--border);border-radius:8px;
  color:var(--text);font-size:1.2rem;padding:0;min-height:56px;
  display:flex;align-items:center;justify-content:center;cursor:pointer;
  transition:background .1s,transform .1s;touch-action:manipulation;width:100%}
.btn:active,.btn.active{background:var(--blue);border-color:var(--blue);transform:scale(.94)}
.btn.stop{background:var(--surf2);border-color:var(--red)}
.btn.stop:active,.btn.stop.active{background:var(--red);border-color:var(--red)}
.btn.green{border-color:var(--green)}
.btn.green:active,.btn.green.active{background:var(--green);border-color:var(--green)}

/* SPEED SLIDER */
.slider-row{display:flex;align-items:center;gap:10px;margin-top:10px}
.slider-row label{font-size:.8rem;color:var(--muted);white-space:nowrap}
input[type=range]{flex:1;accent-color:var(--blue);height:6px;cursor:pointer}
#speed-val{font-size:.8rem;color:var(--blue);min-width:38px;text-align:right}

/* EXTRAS GRID */
.extras{display:grid;grid-template-columns:repeat(auto-fill,minmax(90px,1fr));gap:6px}
.btn-sm{background:var(--surf2);border:1px solid var(--border);border-radius:8px;
  color:var(--text);font-size:.75rem;padding:8px 4px;cursor:pointer;
  display:flex;flex-direction:column;align-items:center;gap:3px;
  transition:background .1s;touch-action:manipulation;width:100%}
.btn-sm .ico{font-size:1.2rem}
.btn-sm:active{background:var(--blue);border-color:var(--blue)}

/* GESTURES */
.gestures{display:grid;grid-template-columns:repeat(auto-fill,minmax(80px,1fr));gap:6px}
.btn-g{background:var(--surf2);border:1px solid var(--border);border-radius:8px;
  color:var(--text);font-size:.72rem;padding:8px 4px;cursor:pointer;
  display:flex;flex-direction:column;align-items:center;gap:3px;
  transition:background .1s;touch-action:manipulation;width:100%}
.btn-g .ico{font-size:1.4rem}
.btn-g:active{background:var(--yellow);border-color:var(--yellow);color:#000}

/* SOUNDS */
.sounds{display:flex;flex-wrap:wrap;gap:6px}
.btn-k{background:var(--surf2);border:1px solid var(--border);border-radius:20px;
  color:var(--text);font-size:.75rem;padding:6px 12px;cursor:pointer;
  transition:background .1s;touch-action:manipulation}
.btn-k:active{background:var(--green);border-color:var(--green);color:#000}

/* DANCE TOGGLE */
.dance-row{display:flex;align-items:center;justify-content:space-between}
.toggle{position:relative;display:inline-block;width:48px;height:26px;cursor:pointer}
.toggle input{opacity:0;width:0;height:0}
.toggle-slider{position:absolute;inset:0;background:var(--surf2);
  border-radius:13px;border:1px solid var(--border);transition:.3s}
.toggle-slider:before{content:"";position:absolute;height:20px;width:20px;
  left:2px;bottom:2px;background:var(--muted);border-radius:50%;transition:.3s}
.toggle input:checked + .toggle-slider{background:var(--green);border-color:var(--green)}
.toggle input:checked + .toggle-slider:before{transform:translateX(22px);background:#fff}

/* TOAST */
#toast{position:fixed;bottom:20px;left:50%;transform:translateX(-50%);
  background:#333;color:#fff;padding:8px 18px;border-radius:20px;
  font-size:.8rem;opacity:0;transition:opacity .3s;pointer-events:none;z-index:99}
#toast.show{opacity:1}

/* RECONNECT OVERLAY */
#overlay{display:none;position:fixed;inset:0;background:rgba(0,0,0,.7);
  align-items:center;justify-content:center;z-index:50;flex-direction:column;gap:12px}
#overlay.show{display:flex}
.spinner{width:36px;height:36px;border:3px solid var(--border);
  border-top-color:var(--blue);border-radius:50%;animation:spin .8s linear infinite}
@keyframes spin{to{transform:rotate(360deg)}}
</style>
</head>
<body>

<!-- STATUS BAR -->
<div id="status-bar">
  <div class="pill" id="bt-pill">
    <span class="dot" id="bt-dot"></span>BT: <span id="bt-label">Desconectado</span>
  </div>
  <div class="pill">WiFi: <span id="ip-val">--</span></div>
  <div class="pill">🔋 <span id="bat-val">--%</span></div>
  <div class="pill">📡 <span id="dist-val">-- cm</span></div>
  <div class="pill">🔊 <span id="noise-val">--</span></div>
</div>

<main>

  <!-- MOVEMENT -->
  <div class="card">
    <h3>Movimiento</h3>
    <div class="dpad">
      <div class="empty"></div>
      <button class="btn" id="btn-fwd" title="Adelante">▲</button>
      <div class="empty"></div>
      <button class="btn" id="btn-left" title="Girar izquierda">◄</button>
      <button class="btn stop" id="btn-stop" title="Parar">■</button>
      <button class="btn" id="btn-right" title="Girar derecha">►</button>
      <div class="empty"></div>
      <button class="btn" id="btn-bwd" title="Atrás">▼</button>
      <div class="empty"></div>
    </div>

    <!-- Speed -->
    <div class="slider-row">
      <label>Velocidad</label>
      <input type="range" id="speed" min="500" max="2000" value="1000" step="100">
      <span id="speed-val">1000ms</span>
    </div>
  </div>

  <!-- EXTRA MOVES -->
  <div class="card">
    <h3>Movimientos extra</h3>
    <div class="extras">
      <button class="btn-sm" data-move="6"  data-size="30"><span class="ico">🌙</span>Moonwalk L</button>
      <button class="btn-sm" data-move="7"  data-size="30"><span class="ico">🌙</span>Moonwalk R</button>
      <button class="btn-sm" data-move="8"  data-size="30"><span class="ico">🎸</span>Swing</button>
      <button class="btn-sm" data-move="11" data-size="15"><span class="ico">⬆</span>Salto</button>
      <button class="btn-sm" data-move="12" data-size="30"><span class="ico">🦋</span>Flap ↑</button>
      <button class="btn-sm" data-move="13" data-size="30"><span class="ico">🦋</span>Flap ↓</button>
      <button class="btn-sm" data-move="9"  data-size="30"><span class="ico">💃</span>Crusaito L</button>
      <button class="btn-sm" data-move="10" data-size="30"><span class="ico">💃</span>Crusaito R</button>
      <button class="btn-sm" data-move="5"  data-size="30"><span class="ico">↕</span>Arriba-Abajo</button>
      <button class="btn-sm" data-move="14" data-size="20"><span class="ico">🩰</span>Puntas</button>
      <button class="btn-sm" data-move="19" data-size="20"><span class="ico">😬</span>Jitter</button>
      <button class="btn-sm" data-move="20" data-size="15"><span class="ico">🌀</span>Giro asc.</button>
    </div>
  </div>

  <!-- DANCE MODE -->
  <div class="card">
    <div class="dance-row">
      <div>
        <h3 style="margin-bottom:2px">Modo baile autónomo</h3>
        <p style="font-size:.75rem;color:var(--muted)">El ESP32 elige movimientos al azar</p>
      </div>
      <label class="toggle">
        <input type="checkbox" id="dance-toggle">
        <span class="toggle-slider"></span>
      </label>
    </div>
  </div>

  <!-- GESTURES -->
  <div class="card">
    <h3>Gestos</h3>
    <div class="gestures">
      <button class="btn-g" data-h="1"><span class="ico">😊</span>Feliz</button>
      <button class="btn-g" data-h="2"><span class="ico">🤩</span>Súper feliz</button>
      <button class="btn-g" data-h="3"><span class="ico">😢</span>Triste</button>
      <button class="btn-g" data-h="4"><span class="ico">😴</span>Dormir</button>
      <button class="btn-g" data-h="5"><span class="ico">💨</span>Pedo</button>
      <button class="btn-g" data-h="6"><span class="ico">😕</span>Confuso</button>
      <button class="btn-g" data-h="7"><span class="ico">❤️</span>Amor</button>
      <button class="btn-g" data-h="8"><span class="ico">😡</span>Enfadado</button>
      <button class="btn-g" data-h="9"><span class="ico">😤</span>Nervioso</button>
      <button class="btn-g" data-h="10"><span class="ico">✨</span>Magia</button>
      <button class="btn-g" data-h="11"><span class="ico">👋</span>Saludo</button>
      <button class="btn-g" data-h="12"><span class="ico">🏆</span>Victoria</button>
      <button class="btn-g" data-h="13"><span class="ico">❌</span>Fallo</button>
    </div>
  </div>

  <!-- SOUNDS -->
  <div class="card">
    <h3>Sonidos</h3>
    <div class="sounds">
      <button class="btn-k" data-k="1">Conexión</button>
      <button class="btn-k" data-k="2">Adiós</button>
      <button class="btn-k" data-k="3">Sorpresa</button>
      <button class="btn-k" data-k="4">Oh-Oh</button>
      <button class="btn-k" data-k="6">Mimoso</button>
      <button class="btn-k" data-k="7">Dormir</button>
      <button class="btn-k" data-k="8">Feliz</button>
      <button class="btn-k" data-k="9">Súper feliz</button>
      <button class="btn-k" data-k="11">Triste</button>
      <button class="btn-k" data-k="12">Confuso</button>
      <button class="btn-k" data-k="13">Pedo 1</button>
      <button class="btn-k" data-k="14">Pedo 2</button>
      <button class="btn-k" data-k="15">Pedo 3</button>
    </div>
  </div>

</main>

<!-- WiFi reconnect overlay -->
<div id="overlay">
  <div class="spinner"></div>
  <p style="color:var(--muted);font-size:.9rem">Reconectando…</p>
</div>

<!-- Toast notifications -->
<div id="toast"></div>

<script>
(function(){
'use strict';

// ---- WebSocket -------------------------------------------------------
let ws, reconnTimer;
const overlay = document.getElementById('overlay');

function connect() {
  ws = new WebSocket('ws://' + location.host + '/ws');

  ws.onopen = () => {
    overlay.classList.remove('show');
    clearTimeout(reconnTimer);
    pollSensors();
  };

  ws.onclose = () => {
    overlay.classList.add('show');
    reconnTimer = setTimeout(connect, 3000);
  };

  ws.onerror = () => ws.close();

  ws.onmessage = ({data}) => {
    try { handle(JSON.parse(data)); } catch(e) {}
  };
}

function send(cmd, args) {
  if (!ws || ws.readyState !== WebSocket.OPEN) return;
  ws.send(JSON.stringify(args !== undefined ? {cmd, args} : {cmd}));
}

// ---- Incoming messages ----------------------------------------------
function handle(d) {
  if (d.type === 'status') {
    const on = d.bt;
    document.getElementById('bt-dot').className   = 'dot' + (on ? ' on' : '');
    document.getElementById('bt-pill').className  = 'pill' + (on ? ' on' : '');
    document.getElementById('bt-label').textContent = on ? 'Conectado' : 'Desconectado';
    document.getElementById('ip-val').textContent = d.ip || '--';
    document.getElementById('dance-toggle').checked = !!d.dance;
  } else if (d.type === 'sensor') {
    if (d.key === 'bat') {
      const el = document.getElementById('bat-val');
      el.textContent = d.val.toFixed(0) + '%';
      el.style.color = d.val < 20 ? 'var(--red)' : d.val < 45 ? 'var(--yellow)' : 'var(--green)';
    } else if (d.key === 'dist') {
      document.getElementById('dist-val').textContent =
        d.val >= 0 ? d.val.toFixed(0) + ' cm' : '-- cm';
    } else if (d.key === 'noise') {
      document.getElementById('noise-val').textContent = d.val;
    }
  } else if (d.type === 'name') {
    document.title = 'Zowi – ' + d.val;
  }
}

// ---- Sensor polling -------------------------------------------------
let sensorTimer;
function pollSensors() {
  clearInterval(sensorTimer);
  sensorTimer = setInterval(() => {
    send('D'); send('N'); send('B');
  }, 2000);
}

// ---- Speed slider ---------------------------------------------------
let currentT = 1000;
const speedEl = document.getElementById('speed');
speedEl.addEventListener('input', () => {
  currentT = parseInt(speedEl.value);
  document.getElementById('speed-val').textContent = currentT + 'ms';
});

// ---- D-pad (hold = repeat) ------------------------------------------
function setupDpad(id, moveId, size) {
  const el = document.getElementById(id);
  let timer;

  function start(e) {
    e.preventDefault();
    el.classList.add('active');
    sendMove(moveId, size);
    timer = setInterval(() => sendMove(moveId, size), currentT + 100);
  }
  function stop(e) {
    e.preventDefault();
    el.classList.remove('active');
    clearInterval(timer);
    send('S');
  }

  el.addEventListener('touchstart', start, {passive:false});
  el.addEventListener('touchend',   stop,  {passive:false});
  el.addEventListener('mousedown',  start);
  el.addEventListener('mouseup',    stop);
  el.addEventListener('mouseleave', stop);
}

function sendMove(id, size) {
  send('M', [id, currentT, size || 15]);
}

setupDpad('btn-fwd',   1, 15);
setupDpad('btn-bwd',   2, 15);
setupDpad('btn-left',  3, 15);
setupDpad('btn-right', 4, 15);

document.getElementById('btn-stop').addEventListener('click', () => send('S'));

// ---- Extra moves (tap = 1 repetition) --------------------------------
document.querySelectorAll('.btn-sm[data-move]').forEach(btn => {
  btn.addEventListener('click', () => {
    const id   = parseInt(btn.dataset.move);
    const size = parseInt(btn.dataset.size) || 15;
    send('M', [id, currentT, size]);
  });
});

// ---- Dance mode toggle ----------------------------------------------
document.getElementById('dance-toggle').addEventListener('change', function() {
  send('DANCE', [this.checked]);
  showToast(this.checked ? '🕺 Modo baile activado' : '⏹ Baile detenido');
});

// ---- Gestures -------------------------------------------------------
document.querySelectorAll('.btn-g[data-h]').forEach(btn => {
  btn.addEventListener('click', () => {
    send('H', [parseInt(btn.dataset.h)]);
  });
});

// ---- Sounds ---------------------------------------------------------
document.querySelectorAll('.btn-k[data-k]').forEach(btn => {
  btn.addEventListener('click', () => {
    send('K', [parseInt(btn.dataset.k)]);
  });
});

// ---- Toast ----------------------------------------------------------
let toastTimer;
function showToast(msg) {
  const el = document.getElementById('toast');
  el.textContent = msg;
  el.classList.add('show');
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => el.classList.remove('show'), 2200);
}

// ---- Init -----------------------------------------------------------
connect();
})();
</script>
</body>
</html>
)rawliteral";
