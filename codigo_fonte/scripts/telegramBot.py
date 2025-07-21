from telegram import Bot, BotCommand, Update
from telegram.ext import ApplicationBuilder, CommandHandler, ContextTypes

TOKEN = "SECRET!"

async def start(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text("Olá! Eu sou a sua Caixa de Correio Inteligente 🤖📦")

async def options(update: Update, context: ContextTypes.DEFAULT_TYPE):
    text = (
        "Aqui estão as opções disponíveis:\n"
        "/start - Iniciar o bot\n"
        "/options - Mostrar opções disponíveis\n"
    )
    await update.message.reply_text(text)

if __name__ == '__main__':
    bot = Bot(token=TOKEN)
    commands = [
        BotCommand("start", "Iniciar o bot"),
        BotCommand("options", "Mostrar opções disponíveis"),
    ]
    bot.set_my_commands(commands)

    app = ApplicationBuilder().token(TOKEN).build()
    app.add_handler(CommandHandler("start", start))
    app.add_handler(CommandHandler("options", options))

    print("Bot rodando...")
    app.run_polling()