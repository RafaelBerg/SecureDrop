## ✅ Requisitos Funcionais

<b>1. Autenticação de Entregas:</b>

- O sistema deve permitir o cadastro de códigos de rastreio autorizados.

- O ESP32-CAM deve ler QR Codes ou códigos de barras para validar entregas.

<b>2. Controle de Acesso:</b>

- Um servo motor deve travar/destravar a tampa da caixa conforme a validação do código.

<b>3. Notificações Remotas:</b>

- O sistema deve ser capaz de enviar notificações via Telegram quando conectado à internet.

<b>4. Interface Web de Configuração:</b>

- O ESP32-S3 deve criar uma rede local e disponibilizar uma interface de configuração via navegador (http://192.168.4.1/config).

- Deve ser possível configurar a rede Wi-Fi, token do bot do Telegram, e os códigos de rastreio autorizados por essa interface.

<b>5. Monitoramento do Estado da Caixa:</b>

- Um sensor magnético deve detectar a abertura e fechamento da tampa.

- O display OLED deve exibir mensagens de status da caixa (ex: travada, desbloqueada, aguardando código).

<b>6. Funcionamento Offline:</b>

- A caixa deve funcionar corretamente mesmo sem conexão com a internet, mantendo a lógica local e controle de acesso.

<b>7. Armazenamento Local:</b>

- Os dados de configuração devem ser salvos localmente (em memória flash) para persistência entre reinicializações.
