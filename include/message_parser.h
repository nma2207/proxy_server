#pragma once

#include <string>
/**
 * @brief Abstract class for parsers
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

class StartupParser : public MessageParser
{
public:
    StartupParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
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


class CancelRequestParser : public MessageParser
{
public:
    CancelRequestParser(const std::string& type) : MessageParser{type}{};
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

class CopyInParser : public MessageParser
{
public:
    CopyInParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class CopyOutParser : public MessageParser
{
public:
    CopyOutParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};

class CopyBothParser : public MessageParser
{
public:
    CopyBothParser(const std::string& type) : MessageParser{type}{};
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

class PortalSuspendedParser: public MessageParser
{
public:
    PortalSuspendedParser(const std::string& type) : MessageParser{type}{};
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
};

class SslRequestParser: public MessageParser
{
public:
    SslRequestParser(const std::string& type) : MessageParser{type}{};
    std::string parse(char* buffer, int len) override;
};
