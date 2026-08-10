const API = '/api';
let G = null, countries = [], cureActive = false, cureRate = 0, curePrev = 0, saboCD = 0;

const $ = id => document.getElementById(id);
const fmt = n => (n=+n) >= 1e9 ? (n/1e9).toFixed(2)+'B' : n >= 1e6 ? (n/1e6).toFixed(1)+'M' : n >= 1e3 ? (n/1e3).toFixed(0)+'K' : String(n);
const callApi = p => fetch(API, { method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify(p) }).then(r => r.json());
const svgEl = tag => document.createElementNS('http://www.w3.org/2000/svg', tag);

const STATUS_LABELS = ['CLEAN','INFECTED','ENDEMIC','CRITICAL','EXTINCT'];
const STATUS_CLS    = ['s0','s1','s2','s3','s4'];

const POS = [[430,52],[400,82],[100,74],[138,136],[340,40],[265,44],[285,52],[287,120],[312,87],[515,144],[502,64],[102,102]];
const EDGES = [
  [0,1],[0,4],[0,9],[0,10],[1,4],[1,5],[1,7],[1,8],[1,9],[1,10],
  [2,3],[2,4],[2,5],[2,6],[2,7],[2,8],[2,9],[2,10],[2,11],
  [3,5],[3,6],[3,7],[3,11],[4,5],[4,6],[4,8],[4,10],
  [5,6],[5,7],[5,8],[5,9],[5,11],[6,7],[6,8],[6,11],
  [7,8],[8,9],[9,10],[10,11],
];

// --- Cure ---
function syncCureFromState() {
  const cp = G.cure_progress || 0;
  if (!cureActive && G.cure_detected) {
    cureActive = true;
    $('cure-wrap').style.display = 'flex';
    setTicker('Pathogen detected — cure research has begun.');
  }
  if (cureActive && cp > curePrev) cureRate = cp - curePrev;
  curePrev = cp;
  renderCureUI();
}

function renderCureUI() {
  const cp = G?.cure_progress || 0;
  if (!cureActive) { $('cure-pct').textContent = 'Not detected'; return; }
  const curedCount = G?.cured?.filter(x => x).length || 0;
  const hi = curedCount >= 9;
  $('cure-pct').textContent   = `${curedCount} / 12 countries`;
  $('cure-bfill').style.width = Math.round(cp * 100) + '%';
  $('cure-hfill').style.width = Math.round(cp * 100) + '%';
  $('cure-hpct').textContent  = `${curedCount}/12`;
  [$('cure-bfill'), $('cure-hfill')].forEach(el => el.classList.toggle('hi', hi));
}

function updateCureButtons() {
  const busy = saboCD > 0;
  $('btn-sabo').disabled    = !cureActive || !G || G.points < 20 || busy;
  $('btn-disinfo').disabled = !cureActive || !G || G.points < 15;
  $('sabo-lbl').textContent = busy ? `Sabotage (cd ${saboCD})` : 'Sabotage labs';
}

function doSabotage() {
  if (!cureActive || !G || G.points < 20 || saboCD > 0) return;
  G.points -= 20; G.cure_progress = Math.max(0, (G.cure_progress || 0) - 0.12); saboCD = 5;
  setTicker('Labs sabotaged — cure set back 12%.');
  renderCureUI(); updateCureButtons(); renderUI();
}

function doDisinfo() {
  if (!cureActive || !G || G.points < 15) return;
  G.points -= 15; G.cure_progress = Math.max(0, (G.cure_progress || 0) - 0.05);
  setTicker('Disinfo campaign launched — cure slowed.');
  renderCureUI(); renderUI();
}

function resetCure() {
  cureActive = false; cureRate = 0; curePrev = 0; saboCD = 0;
  $('cure-wrap').style.display = 'none';
  $('cure-bfill').style.width  = '0%';
  [$('cure-bfill'), $('cure-hfill')].forEach(el => el.classList.remove('hi'));
  $('btn-sabo').disabled = $('btn-disinfo').disabled = true;
  renderCureUI();
}

// --- UI ---
const setTicker = msg => $('ticker').textContent = msg;

function lock(disabled) {
  $('btn-next').disabled = disabled;
  $('btn-inf').disabled  = disabled || !G || G.points < 10;
  $('btn-sev').disabled  = disabled || !G || G.points < 10;
}

function renderUI() {
  if (!G) return;
  ['day','inf','sev','pts'].forEach(k => $('hdr-'+k).textContent = G[k === 'pts' ? 'points' : k === 'inf' ? 'infectivity' : k === 'sev' ? 'severity' : 'day']);
  ['day','pts','inf','sev'].forEach(k => $('sb-'+k).textContent = G[k === 'pts' ? 'points' : k === 'inf' ? 'infectivity' : k === 'sev' ? 'severity' : 'day']);
  $('v-inf').textContent = G.infectivity; $('v-sev').textContent = G.severity;

  const ti = G.total_infected || 0, td = G.total_dead || 0, tp = G.total_population || 1;
  const alive = Math.max(0, tp - td);
  $('s-inf').textContent  = fmt(ti);
  $('s-dead').textContent = fmt(td);
  $('s-pct').textContent  = alive > 0 ? ((ti / alive) * 100).toFixed(1) + '%' : '0%';
  $('s-ctry').textContent = (G.infected || []).filter(x => x > 0).length + '/12';

  $('btn-inf').disabled = $('btn-sev').disabled = G.points < 10;
  updateCureButtons();
  if (G.message) setTicker(G.message);

  $('tbody').innerHTML = countries.map((c, i) => {
    const pop = G.population[i]||0, inf = G.infected[i]||0, dead = G.dead[i]||0;
    const al = Math.max(0, pop - dead), pi = al > 0 ? ((inf/al)*100).toFixed(1) : '0';
    const si = dead >= pop && pop > 0 ? 4 : inf > 0 && +pi >= 99 ? 3 : inf > 0 && +pi >= 40 ? 2 : inf > 0 ? 1 : 0;
    return `<tr><td>${c.name}${i===G.origin?' ★':''}</td><td>${fmt(pop)}</td><td>${fmt(inf)}</td><td>${pi}%</td><td style="color:#e5393577">${fmt(dead)}</td><td class="${STATUS_CLS[si]}">${STATUS_LABELS[si]}</td></tr>`;
  }).join('');
  updateDots();
}

function showGameOver() {
  $('screen-game').style.display = 'none';
  const ov = $('screen-over');
  ov.style.display = 'flex';
  ov.className = G.won ? 'win' : 'lose';
  $('over-h1').textContent  = G.won ? 'EXTINCTION' : 'CONTAINED';
  $('over-msg').textContent = G.message || (G.won ? 'Humanity is extinct.' : 'Humanity survived.');
  const kp = G.total_population > 0 ? ((G.total_dead/G.total_population)*100).toFixed(2) : '0';
  $('over-stats').textContent =
    `Days played   ${G.day}\nKilled        ${fmt(G.total_dead)}\nWorld pop.    ${fmt(G.total_population)}\nKill %        ${kp}%\nInfectivity   ${G.infectivity}\nSeverity      ${G.severity}\nCure at       ${Math.round((G.cure_progress||0)*100)}%`;
}

// --- Map ---
function drawLines() {
  const g = $('map-lines'); g.innerHTML = '';
  EDGES.forEach(([a,b]) => {
    const l = svgEl('line');
    l.setAttribute('x1',POS[a][0]); l.setAttribute('y1',POS[a][1]);
    l.setAttribute('x2',POS[b][0]); l.setAttribute('y2',POS[b][1]);
    l.setAttribute('stroke','#1a2a1a'); l.setAttribute('stroke-width','0.7');
    g.appendChild(l);
  });
}

function drawDots() {
  const g = $('map-dots'); g.innerHTML = '';
  countries.forEach((c, i) => {
    const circle = svgEl('circle');
    circle.setAttribute('cx',POS[i][0]); circle.setAttribute('cy',POS[i][1]);
    circle.setAttribute('r','5'); circle.setAttribute('fill','#0a150a');
    circle.setAttribute('stroke','#4caf50'); circle.setAttribute('stroke-width','1');
    circle.id = 'dot'+i;
    g.appendChild(circle);
    const t = svgEl('text');
    t.setAttribute('x',POS[i][0]+7); t.setAttribute('y',POS[i][1]+4);
    t.setAttribute('font-size','7'); t.setAttribute('fill','#4caf50');
    t.setAttribute('font-family','Courier New, monospace'); t.setAttribute('opacity','.7');
    t.textContent = c.name.slice(0,3).toUpperCase() + (i===G?.origin?'*':'');
    g.appendChild(t);
  });
}

function updateDots() {
  if (!G) return;
  const cured = G.cured || [];
  countries.forEach((_, i) => {
    const dot = $('dot'+i); if (!dot) return;
    const pop = G.population[i]||0, inf = G.infected[i]||0, dead = G.dead[i]||0;
    const pct = pop > 0 ? inf/pop : 0;
    if (dead >= pop && pop > 0) {
      dot.setAttribute('fill','#111'); dot.setAttribute('stroke','#333'); dot.setAttribute('r','4');
    } else if (cured[i]) {
      dot.setAttribute('fill','#0a1a2a'); dot.setAttribute('stroke','#29b6f6'); dot.setAttribute('r','6');
    } else if (inf > 0) {
      dot.setAttribute('fill',`rgb(${Math.round(150+pct*105)},${Math.round((1-pct)*40)},0)`);
      dot.setAttribute('stroke','#e53935'); dot.setAttribute('r',String(5+Math.round(pct*8)));
    } else {
      dot.setAttribute('fill','#0a150a'); dot.setAttribute('stroke','#4caf50'); dot.setAttribute('r','5');
    }
  });
  const lines = $('map-lines').children;
  EDGES.forEach(([a,b], idx) => {
    const l = lines[idx]; if (!l) return;
    const ai = (G.infected[a]||0) > 0, bi = (G.infected[b]||0) > 0;
    l.setAttribute('stroke', ai&&bi ? '#e5393540' : ai||bi ? '#ff220015' : '#1a2a1a');
    l.setAttribute('stroke-width', ai&&bi ? '1' : ai||bi ? '0.9' : '0.7');
  });
}

// --- Game flow ---
async function init() {
  try {
    const data = await callApi({ action:'init' });
    countries = data.countries;
    const picker = $('country-picker'); picker.innerHTML = '';
    countries.forEach((c,i) => {
      const btn = document.createElement('button');
      btn.className = 'cbtn';
      btn.innerHTML = `${c.name}<small>${c.region} · ${fmt(c.population)}</small>`;
      btn.onclick = () => startGame(i);
      picker.appendChild(btn);
    });
    drawLines();
  } catch {
    $('country-picker').innerHTML = '<span style="color:#e53935">Cannot connect — run: node server.js</span>';
  }
}

async function startGame(ci) {
  G = await callApi({ action:'start', country_index:ci });
  resetCure();
  $('screen-welcome').style.display = 'none';
  $('screen-game').style.display    = 'flex';
  drawDots(); renderUI();
}

async function nextDay() {
  if (!G || G.game_over) return;
  lock(true);
  G = await callApi({ action:'next_day', ...G });
  saboCD && saboCD--; syncCureFromState(); renderUI();
  G.game_over ? showGameOver() : lock(false);
}

async function upgrade(type) {
  if (!G || G.game_over || G.points < 10) return;
  lock(true);
  G = await callApi({
    action:'upgrade', type,
    day:G.day, points:G.points, infectivity:G.infectivity, severity:G.severity,
    origin:G.origin, all_infected_day:G.all_infected_day??-1,
    eradication_deadline:G.eradication_deadline??-1,
    game_over:G.game_over, won:G.won,
    total_infected:G.total_infected, total_dead:G.total_dead, total_population:G.total_population,
    message:G.message||'', infected:G.infected, dead:G.dead,
    population:G.population, spread_delay:G.spread_delay,
  });
  renderUI(); lock(false);
}

function restart() {
  G = null; countries = [];
  $('ticker').textContent = '';
  $('screen-over').style.display    = 'none';
  $('screen-welcome').style.display = 'block';
  resetCure(); init();
}

document.addEventListener('keydown', e => {
  if (!G || G.game_over) return;
  if (e.key === 'n' || e.key === 'N' || e.key === ' ') { e.preventDefault(); nextDay(); }
  if (e.key === 'i' || e.key === 'I') upgrade('infectivity');
  if (e.key === 's' || e.key === 'S') upgrade('severity');
});

init();
