#include "telegram_bot.h"
#include <Preferences.h>
#include <HardwareSerial.h>
#include <StreamString.h>
#include "bot_token.h"

TelegramBot::TelegramBot(Client& client)
  : bot_(BOT_TOKEN, client)
{
  Preferences pref;
  pref.begin("bot", true);
  String subscribers = pref.getString("subscribers", "");
  StreamString* stream = (StreamString*)&subscribers;
  while (stream->available()) {
    subscribers_.insert(
      stream->readStringUntil(','));
  }
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
      else {
        continue;
      }

      StreamString stream;
      for (const String& chatId : subscribers_) {
        stream.printf("%s,", chatId);
      }
      Preferences pref;
      pref.begin("bot", true);
      pref.putString("subscribers", stream);
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
