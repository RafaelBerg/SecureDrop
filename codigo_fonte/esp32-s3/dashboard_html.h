#ifndef DASHBOARD_HTML_H
#define DASHBOARD_HTML_H

const char* dashboardHTML = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">

<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Rastreamento</title>
  <style>
      body {
          font-family: Arial, sans-serif;
          background: #f3f3f3;
          padding: 20px;
          height: 100vh;
          overflow: hidden;
      }

      .input-area,
      .dashboard-area {
          background: white;
          padding: 15px;
          border-radius: 10px;
          box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
          margin-bottom: 20px;
      }

      .input-area input,
      .input-area button,
      .dashboard-area select {
          width: 100%;
          padding: 10px;
          margin-top: 10px;
          border-radius: 6px;
          border: 1px solid #ccc;
          box-sizing: border-box;
          font-size: 1em;
      }

      .input-area button {
          background-color: #007BFF;
          color: white;
          border: none;
          font-weight: bold;
          cursor: pointer;
      }

      .input-area button:hover {
          background-color: #0056b3;
      }

      .dashboard-area h2 {
          margin-top: 0;
          margin-bottom: 15px;
          color: #333;
      }

      .card {
          background: white;
          border-radius: 10px;
          padding: 15px;
          box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
          margin-bottom: 15px;
      }

      .codigo {
          font-weight: bold;
          font-size: 1.1em;
          color: #333;
          margin-bottom: 8px;
      }

      .status {
          font-weight: normal;
      }

      .entregue {
          color: green;
          font-weight: bold;
      }

      .pendente {
          color: red;
          font-weight: bold;
      }

      .data-entrega {
          font-size: 0.9em;
          color: #555;
          margin-top: 5px;
      }

      .card button {
          background: #dc3545;
          color: white;
          border: none;
          padding: 8px 12px;
          border-radius: 5px;
          font-size: 0.9em;
          cursor: pointer;
          margin-top: 25px;
      }

      .card button:hover {
          background: #a71d2a;
      }

      .empty-message {
          text-align: center;
          color: #666;
          margin-top: 30px;
      }

      #statusFilter {
          margin-bottom: 15px;
      }

      #config {
          position: fixed;
          right: 22px;
          bottom: 22px;
          border: solid 1px;
          border-color: rgba(128, 128, 128, 0.158);
          background-color: rgb(255, 255, 255);
          border-radius: 30%;
          padding: 4px 5px;
          font-size: 1.8em;
          cursor: pointer;
          box-shadow: 0 5px 10px rgba(0, 0, 0, 0.1);
      }

      #codeList {
          max-height: var(--vh50);
          overflow-y: auto;
      }
  </style>
</head>

<body>
  <div class="input-area">
      <h3>Adicionar Código</h3>
      <input type="text" id="codeInput" placeholder="Digite o código de rastreio" />
      <button onclick="addCode()">Adicionar</button>
  </div>

  <div class="dashboard-area">
      <h2>Dashboard de Rastreamento</h2>
      <select id="statusFilter" onchange="fetchAndRenderCodes()">
          <option value="">Todos os status</option>
          <option value="pendente">Pendente</option>
          <option value="entregue">Entregue</option>
      </select>
      <div id="codeList"></div>
  </div>

  <button id="config" onclick="window.location.href='/config'">⚙️</button>
  
<script>
  function setVh50() {
      const alturaTotal = window.innerHeight;
      const inputArea = document.querySelector('.input-area');
      const dashboardArea = document.querySelector('.dashboard-area');
      const titulo = dashboardArea?.querySelector('h2');
      const filtro = dashboardArea?.querySelector('#statusFilter');

      const alturaUsada =
          (inputArea?.offsetHeight || 0) +
          (titulo?.offsetHeight || 0) +
          (filtro?.offsetHeight || 0) +
          140; // compensação de espaçamento

      const alturaRestante = alturaTotal - alturaUsada;
      document.documentElement.style.setProperty('--vh50', `${alturaRestante}px`);
  }

  window.addEventListener('load', setVh50);
  window.addEventListener('resize', setVh50);
  async function fetchCodes() {
    try {
      const res = await fetch('/codes');
      if (!res.ok) throw new Error('Erro ao buscar códigos');
      return await res.json();
    } catch (e) {
      alert('Erro ao carregar códigos: ' + e.message);
      return [];
    }
  }

  async function fetchAndRenderCodes() {
    const codes = await fetchCodes();
    const filter = document.getElementById('statusFilter').value.toLowerCase();
    const list = document.getElementById('codeList');
    list.innerHTML = '';

    const filtered = filter ? codes.filter(c => c.status.toLowerCase() === filter) : codes;

    if (filtered.length === 0) {
      list.innerHTML = '<p class="empty-message">Nenhum código cadastrado.</p>';
      return;
    }

    filtered.forEach(({ code, status, date }) => {
      const card = document.createElement('div');
      card.className = 'card';

      const statusLower = status.toLowerCase();
      const statusClass = statusLower === 'entregue' ? 'entregue' : 'pendente';

      let deliveryInfo = '';
      if (statusLower === 'entregue' && date) {
        const d = new Date(date);
        if (!isNaN(d.getTime())) {
          deliveryInfo = `<div class="data-entrega">Data de entrega: ${d.toLocaleDateString('pt-BR')} ${d.toLocaleTimeString('pt-BR', { hour: '2-digit', minute: '2-digit' })}</div>`;
        }
      }

      card.innerHTML =
        '<div class="codigo">Código: ' + code + '</div>' +
        '<div class="status">Status: <span class="' + statusClass + '">' + status.charAt(0).toUpperCase() + status.slice(1) + '</span></div>' +
        deliveryInfo +
        '<button onclick="deleteCode(\'' + code + '\')">Apagar</button>';

      list.appendChild(card);
    });
  }

  async function addCode() {
    const input = document.getElementById('codeInput');
    const code = input.value.trim();
    if (!code) {
      alert('Digite um código válido!');
      return;
    }

    try {
      const res = await fetch('/codes', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ code })
      });

      if (!res.ok) {
        const text = await res.text();
        alert('Erro: ' + text);
        return;
      }

      input.value = '';
      fetchAndRenderCodes();
    } catch (e) {
      alert('Erro ao adicionar código: ' + e.message);
    }
  }

  async function deleteCode(code) {
    if (!confirm('Deseja realmente apagar o código "' + code + '"?')) return;

    try {
      const res = await fetch('/codes?code=' + encodeURIComponent(code), {
        method: 'DELETE'
      });

      if (!res.ok) {
        const text = await res.text();
        alert('Erro ao apagar código: ' + text);
        return;
      }

      fetchAndRenderCodes();
    } catch (e) {
      alert('Erro ao apagar código: ' + e.message);
    }
  }

  window.onload = fetchAndRenderCodes;
</script>
</body>
</html>
)rawliteral";

#endif