#include "telegram_bot.h"
#include <HardwareSerial.h>
#include "bot_token.inl"

TelegramBot::TelegramBot(Client& client)
  : bot_(BOT_TOKEN, client)
{
}

void TelegramBot::processIncomingMessages()
{
  int numNewMessages = bot_.getUpdates(bot_.last_message_received + 1);
  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      const telegramMessage& msg = bot_.messages[i];
      Serial.printf("Got message from %s: %s\n", msg.from_name.c_str(), msg.text.c_str());
      if (msg.text == "/start") {
        subscribers_.insert(msg.chat_id);
      }
      else if (msg.text == "/stop") {
        subscribers_.erase(msg.chat_id);
      }
    }
    numNewMessages = bot_.getUpdates(bot_.last_message_received + 1);
  }
}

void TelegramBot::publish(const String& message)
{
  for (const String& chatId : subscribers_) {
    bot_.sendMessage(chatId, message, "");
  }
}
