#ifndef CONFIG_HTML_H
#define CONFIG_HTML_H

const char* configHTML = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">

<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Configuração do Dispositivo</title>
  <style>
      body {
          font-family: Arial, sans-serif;
          background: #f3f3f3;
          padding: 20px;
      }

      .container {
          position: relative;
          background: #fff;
          padding: 20px;
          border-radius: 8px;
          max-width: 500px;
          margin: auto;
          box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
      }

      h2 {
          text-align: center;
      }

      input,
      button {
          border: 1px solid #ccc;
          width: 100%;
          padding: 10px;
          margin: 10px 0px;
          font-size: 16px;
          border-radius: 6px;
      }

      input {
          width: 95%;
      }

      button {
          background-color: #007BFF;
          color: white;
          border: none;
          font-weight: bold;
          cursor: pointer;
      }

      button:hover {
          background-color: #0056b3;
      }

      #status {
          text-align: center;
          margin-top: 15px;
          font-weight: bold;
      }

      #configForm {
          display: flex;
          flex-direction: column;
          justify-content: center;
          margin: 0px auto;
          align-items: center;
      }

      #back-button {
          position: absolute;
          left: 22px;
          top: 30px;
          border: solid 1px;
          width: fit-content;
          height: fit-content;
          border-color: rgb(128, 128, 128);
          background-color: rgba(253, 253, 253, 0.89);
          color: black;
          border-radius: 30%;
          padding: 3px 10px 3px 6px;
          text-align: left;
          font-size: 1em;
          cursor: pointer;
          box-shadow: 0 5px 10px rgba(0, 0, 0, 0.1);
      }

      #reset {
          background-color: red;
      }

      #reset:hover {
          background-color: rgba(161, 4, 4, 0.774);
      }

      #confirmModal {
          position: fixed;
          top: 0;
          left: 0;
          width: 100vw;
          height: 100vh;
          background: rgba(0, 0, 0, 0.4);
          display: flex;
          align-items: center;
          justify-content: center;
          z-index: 999;
      }

      .modal-content {
          background: white;
          padding: 20px;
          border-radius: 10px;
          text-align: center;
          max-width: 300px;
          width: 80%;
          box-shadow: 0 0 10px rgba(0, 0, 0, 0.3);
      }

      .modal-buttons {
          margin-top: 20px;
          display: flex;
          justify-content: space-around;
          gap: 10px;
      }

      .modal-buttons button {
          padding: 10px 20px;
          border: none;
          border-radius: 6px;
          font-weight: bold;
          cursor: pointer;
      }

      .modal-buttons button:first-child {
          background-color: #007BFF;
          color: white;
      }

      .modal-buttons button:last-child {
          background-color: gray;
          color: white;
      }
  </style>
</head>

<body>
  <div class="container">
      <h2>Configuração do Dispositivo</h2>
      <button id="back-button" onclick="window.location.href='/'">&#x25C0;</button>
      <form id="configForm">
          <input type="text" id="wifiSSID" placeholder="Wi-Fi SSID" />
          <input type="text" id="wifiPassword" placeholder="Wi-Fi Password" />
          <input type="text" id="apSSID" placeholder="AP SSID" />
          <input type="text" id="apPassword" placeholder="AP Password" />
          <input type="text" id="botToken" placeholder="Telegram Bot Token" />
          <input type="text" id="chatId" placeholder="Telegram Chat ID" />
          <input type="text" id="codeBox" placeholder="Código da caixa" />
          <button type="submit">Salvar Configuração</button>
          <button id="reset" type="button" onclick="resetConfig()">Resetar Configuração</button>
      </form>
      <p id="status"></p>

      <div id="confirmModal" style="display: none;">
          <div class="modal-content">
              <p>Tem certeza que deseja resetar a configuração?</p>
              <div class="modal-buttons">
                  <button onclick="confirmReset()">Sim</button>
                  <button onclick="closeModal()">Cancelar</button>
              </div>
          </div>
      </div>

  </div>

  <script>
      const form = document.getElementById("configForm");
      const statusEl = document.getElementById("status");
      const fields = ["wifiSSID", "wifiPassword", "apSSID", "apPassword", "botToken", "chatId", "codeBox"];
      let originalConfig = {};

      async function loadCurrentConfig() {
          try {
              const response = await fetch("/get-config");
              const config = await response.json();
              originalConfig = config;

              fields.forEach(id => {
                  const input = document.getElementById(id);
                  if (config[id]) {
                      input.value = config[id];
                  }
              });
          } catch (err) {
              console.error("Erro ao carregar configuração atual:", err);
              statusEl.textContent = "Não foi possível carregar a configuração atual";
              statusEl.style.color = "red";
          }
      }

      function isValidApPassword() {
          const apPassword = document.getElementById("apPassword").value.trim();
          if (apPassword.length < 8 && apPassword != 0) {
              statusEl.textContent = "O AP Password deve ter no mínimo 8 caracteres.";
              statusEl.style.color = "red";
              return false;
          }
          return true;
      }

      form.addEventListener("submit", async (e) => {
          e.preventDefault();

          if (!isValidApPassword()) return;

          const data = {};
          let hasChanges = false;

          fields.forEach(id => {
              const currentValue = document.getElementById(id).value.trim();
              const originalValue = originalConfig[id] || "";

              if (currentValue !== originalValue) {
                  data[id] = currentValue;
                  hasChanges = true;
              }
          });

          if (!hasChanges) {
              statusEl.textContent = "Nenhuma configuração foi alterada.";
              statusEl.style.color = "orange";
              return;
          }

          try {
              const response = await fetch("/update-config", {
                  method: "POST",
                  headers: {
                      "Content-Type": "application/json"
                  },
                  body: JSON.stringify(data)
              });

              const text = await response.text();
              statusEl.textContent = text;
              statusEl.style.color = response.ok ? "green" : "red";

              if (response.ok) {
                  Object.assign(originalConfig, data);
              }
          } catch (err) {
              statusEl.textContent = "Erro na requisição";
              statusEl.style.color = "red";
          }
      });

      function resetConfig() {
          document.getElementById("confirmModal").style.display = "flex";
      }

      async function confirmReset() {
          try {
              const response = await fetch("/reset-config", { method: "POST" });
              const text = await response.text();
              statusEl.textContent = text || "Configuração resetada";
              statusEl.style.color = "blue";
          } catch (err) {
              statusEl.textContent = "Erro ao resetar configuração";
              statusEl.style.color = "red";
          } finally {
              closeModal();
          }
      }

      function closeModal() {
          document.getElementById("confirmModal").style.display = "none";
      }

      loadCurrentConfig();
  </script>
</body>
</html>
)rawliteral";

#endif