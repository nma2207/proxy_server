#pragma once

#include "message_parser.h"
#include <map>
#include <memory>

class PostgreSqlManager
{
public:
    PostgreSqlManager();
    std::string parse(char* buffer, int len) const;

private:
    std::map<char, std::unique_ptr<MessageParser>> parsers;

};
