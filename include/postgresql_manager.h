#pragma once

#include "message_parser.h"
#include <map>
#include <memory>

class PostgreSqlManager
{
public:
    PostgreSqlManager();
    ~PostgreSqlManager();
    std::string parseB(char* buffer, int len);
    std::string parseF(char* buffer, int len);

private:
    std::map<char, MessageParser*> _parsersB;
    std::map<char, MessageParser*> _parsersF;

};
