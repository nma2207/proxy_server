#pragma once

#include "message_parser.h"
#include <map>
#include <memory>

/**
 * @brief Класс, который принимает сообщений для парсинга
 */
class PostgreSqlManager
{
public:
    PostgreSqlManager();
    ~PostgreSqlManager();
    std::string parseB(char* buffer, int len); // если надо распарсить сообщение от сервера
    std::string parseF(char* buffer, int len); // если надо распарсить сообщение от клиента

private:
    std::map<char, MessageParser*> _parsersB; // парсеры сервера
    std::map<char, MessageParser*> _parsersF; // парсеры клиента

};
