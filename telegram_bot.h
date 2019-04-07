#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

#include <UniversalTelegramBot.h>
#include <Client.h>
#include <set>

class TelegramBot {
public:
  TelegramBot(Client& client);

  void processIncomingMessages();

  void publish(const String& message);

private:
  UniversalTelegramBot bot_;
  std::set<String> subscribers_;
};

#endif
