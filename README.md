# 📦 Caixa Inteligente para Entregas (ESP32-S3 + ESP32-CAM)

Este projeto é um **MVP (Produto Mínimo Viável)** de uma **Caixa Inteligente para Entregas**, projetada para funcionar **offline** com a opção de se conectar ao Wi-Fi para enviar **notificações via Telegram**. O sistema utiliza dois ESP32s (um para a interface de controle e outro para leitura dos códigos de rastreio), além de componentes simples como servo motor, sensor magnético e display OLED.

---

## 🚀 Objetivo

Criar uma solução acessível e embarcada para automatizar o recebimento de entregas, mesmo na ausência do destinatário, com possibilidade de notificação remota.

---

## 🔧 Componentes Utilizados

| Componente       | Função                                                             |
|------------------|--------------------------------------------------------------------|
| **ESP32-S3**     | Cria a rede local para configuração e gerencia a lógica do sistema |
| **ESP32-CAM**    | Lê códigos de rastreio (QR code ou código de barras)               |
| **Servo motor**  | Trava e destrava a tampa da caixa                                  |
| **Display OLED** | Exibe mensagens de status                                          |
| **Sensor magnético com fio** | Detecta abertura/fechamento                            |

---

## 📚 Documentação

Este projeto também inclui documentos importantes para consulta:

- **Requisitos Funcionais e Não Funcionais** na pasta [`requisitos/`](./requisitos)
- **Diagramas** (blocos, elétricos) na pasta [`diagramas/`](./diagramas)

---

## 🖥️ Como Funciona

### 🔧 Configuração Inicial

1. Ao ligar pela primeira vez, o **ESP32-S3** cria uma rede Wi-Fi chamada `ESP32S3`
2. Conecte-se a essa rede (sem internet)
3. Acesse `http://192.168.4.1/config` pelo navegador
4. Configure:
   - Códigos de rastreio autorizados (http://192.168.4.1/)
   - (Opcional) Dados do Wi-Fi da sua casa
   - (Opcional) Dados do AP
   - (Opcional) Token e chat ID do seu bot do Telegram
   - (Opcional) Código do proprietário 

### 📦 Processo de Entrega

1. O entregador apresenta o código à câmera (**ESP32-CAM**)
2. O sistema verifica se o código é válido
3. Se for:
   - A caixa é destravada (servo motor)
   - O sensor magnético verifica a abertura e o depósito
   - A caixa é travada novamente
   - O **display** mostra mensagens durante todo o processo
   - Se configurado, o sistema **envia uma mensagem via Telegram** com a confirmação da entrega e o código

---

## 🌐 Conectividade

- 🔌 **Offline por padrão**: Todo o sistema funciona mesmo sem internet
- 📲 **Modo online opcional**: Se o usuário configurar o Wi-Fi, o sistema se conecta e envia **notificações via Telegram**

---

## 🛠️ Tecnologias e Linguagens

- ESP32-S3 & ESP32-CAM com C++ (Arduino Framework)
- HTML/CSS/JS para a interface de configuração local
- Comunicação entre ESP32-S3 e ESP32-CAM via UDP e chamadas HTTP
- API do Telegram para envio de mensagens (quando online)

---

## 📍 Status do Projeto

- [x] Rede local e página de configuração
- [x] Cadastro e validação de códigos de rastreio
- [x] Leitura de códigos com ESP32-CAM
- [x] Controle da trava com servo motor
- [x] Sensor magnético para confirmar entrega
- [x] Display OLED com status da entrega
- [x] Envio de notificação via Telegram (modo online)
- [ ] Melhorias na interface web
- [ ] Versão final da estrutura física da caixa

---

## 📸 Imagens e Demonstração

[🎥 Assista à demonstração no YouTube](https://www.youtube.com/watch?v=gez-uzXpYOM)

---


**Autores:** Edriel Jose, Rafael Berg, Wesley Wilson.