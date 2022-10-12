#pragma once

#include <string>
/**
 * @brief Базовый класс парсера. здесь не буду каждый парсер расписывать, т.к. их много, а они просто соответствуют протоколу
 * общий прицип такой, что в строке передается тип пакеа, который пишется в начале, а сообщение пишется в квадратных скобках.
 * в базовом парсере просто возвращаемся тип сообщения
 */
class MessageParser
{
public:
    explicit MessageParser(const std::string& type)
        : type{type}
    {}
    virtual std::string parse(char *buffer, int len) {
        return type;
    }
protected:
    std::string type;
};

/**
 * @brief Специальный класс, у которого нет первого байта, он обрабатывает сообщения Startup, SSLRequest и CancelRequest
 */
class WithoutFirstByteParser : public MessageParser
{
public:
    WithoutFirstByteParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;

private:
    /**
     * в зависимсти от специального кода выбирается нужный парсер
     */
    std::string _parseStatup(char* buffer, int len);
    std::string _parseCancelRequest(char*buffer, int len);
};

class AuthParser : public MessageParser
{
public:
    AuthParser(const std::string& type) : MessageParser{type}{}
    std::string parse(char* buffer, int len) override;
};

class BackendKeyParser: public MessageParser {
public:
    BackendKeyParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class BindParser : public MessageParser
{
public:
    BindParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};


class CloseParser : public MessageParser
{
public:
    CloseParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class CommandCompleteParser : public MessageParser
{
public:
    CommandCompleteParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class CopyDataParser : public MessageParser
{
public:
    CopyDataParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};


class CopyFailParser : public MessageParser
{
public:
    CopyFailParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class CopyParser : public MessageParser
{
public:
    CopyParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class DataRowParser : public MessageParser
{
public:
    DataRowParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};


class DescribeParser : public MessageParser
{
public:
    DescribeParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};


class ErrorResponceParser : public MessageParser
{
public:
    ErrorResponceParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class ExecuteParser : public MessageParser
{
public:
    ExecuteParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};


class FunctionCallParser : public MessageParser
{
public:
    FunctionCallParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class FunctionCallResponceParser : public MessageParser
{
public:
    FunctionCallResponceParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class NegotiateProtocolVersionParser: public MessageParser
{
public:
    NegotiateProtocolVersionParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class NoticeResponseParser: public MessageParser
{
public:
    NoticeResponseParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class NotificationResponseParser: public MessageParser
{
public:
    NotificationResponseParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class ParameterDescriptionParser: public MessageParser
{
public:
    ParameterDescriptionParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class ParameterStatusParser: public MessageParser
{
public:
    ParameterStatusParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};


class ParseParser: public MessageParser
{
public:
    ParseParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class PasswordMessageParser: public MessageParser
{
public:
    PasswordMessageParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class QueryParser: public MessageParser
{
public:
    QueryParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class ReadyForQueryParser: public MessageParser
{
public:
    ReadyForQueryParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class RowDescriptionParser: public MessageParser
{
public:
    RowDescriptionParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;

private:
    std::string parseStartup(char* buffer, int len);
    std::string parseCancelRequest(char* buffer, int len);
};
