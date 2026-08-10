const http  = require('http');
const path  = require('path');
const fs    = require('fs');
const { spawnSync } = require('child_process');

const PORT    = 3000;
const CPP_BIN = path.join(__dirname, process.platform === 'win32' ? 'plague_sim.exe' : 'plague_sim');

const MIME = {
  '.html': 'text/html',
  '.css':  'text/css',
  '.js':   'text/javascript',
};

function callCpp(payload) {
  const result = spawnSync(CPP_BIN, [], {
    input:    JSON.stringify(payload),
    encoding: 'utf8',
    timeout:  5000,
  });
  if (result.error)  throw new Error('C++ spawn error: '   + result.error.message);
  if (result.status) throw new Error('C++ non-zero exit: ' + result.stderr);
  const line = result.stdout.trim();
  if (!line)         throw new Error('C++ returned empty output');
  return JSON.parse(line);
}

const server = http.createServer((req, res) => {
  res.setHeader('Access-Control-Allow-Origin',  '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') { res.writeHead(204); res.end(); return; }

  if (req.method === 'POST' && req.url === '/api') {
    let body = '';
    req.on('data', chunk => body += chunk);
    req.on('end', () => {
      try {
        const result = callCpp(JSON.parse(body));
        res.writeHead(200, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify(result));
      } catch (e) {
        console.error('[API Error]', e.message);
        res.writeHead(500, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify({ status: 'error', error: e.message }));
      }
    });
    return;
  }

  if (req.method === 'GET') {
    const urlPath  = req.url === '/' ? '/index.html' : req.url;
    const filePath = path.join(__dirname, urlPath);
    if (!filePath.startsWith(__dirname)) { res.writeHead(403); res.end('Forbidden'); return; }
    fs.readFile(filePath, (err, data) => {
      if (err) { res.writeHead(404); res.end('Not found'); return; }
      const mime = MIME[path.extname(filePath)] || 'text/plain';
      res.writeHead(200, { 'Content-Type': mime });
      res.end(data);
    });
    return;
  }

  res.writeHead(404); res.end('Not found');
});

server.listen(PORT, () => {
  console.log(`Plague Simulator running at http://localhost:${PORT}`);
});
