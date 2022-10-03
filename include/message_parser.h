#pragma once

#include <string>
/**
 * @brief Abstract class for parsers
 */
class MessageParser
{
public:
    MessageParser();
    virtual std::string parse(const std::string& type, char *buffer, int len) {
        return type;
    };
};

class StartupParser : public MessageParser
{
public:
    StartupParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class AuthParser : public MessageParser
{
public:
    AuthParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class BackendKeyParser: public MessageParser {
public:
    BackendKeyParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class BindParser : public MessageParser
{
public:
    BindParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};


class CancelRequestParser : public MessageParser
{
public:
    CancelRequestParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class CloseParser : public MessageParser
{
public:
    CloseParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class CommandCompleteParser : public MessageParser
{
public:
    CommandCompleteParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class CopyDataParser : public MessageParser
{
public:
    CopyDataParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};


class CopyFailParser : public MessageParser
{
public:
    CopyFailParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class CopyOutParser : public MessageParser
{
public:
    CopyOutParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};


class DataRowParser : public MessageParser
{
public:
    DataRowParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};


class DescribeParser : public MessageParser
{
public:
    DescribeParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};


class ErrorResponceParser : public MessageParser
{
public:
    ErrorResponceParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class ExecuteParser : public MessageParser
{
public:
    ExecuteParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};


class FunctionCallParser : public MessageParser
{
public:
    FunctionCallParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class FunctionCallResponceParser : public MessageParser
{
public:
    FunctionCallResponceParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class NegotiateProtocolVersionParser: public MessageParser
{
public:
    NegotiateProtocolVersionParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class NoticeResponseParser: public MessageParser
{
public:
    NoticeResponseParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class NotificationResponseParser: public MessageParser
{
public:
    NotificationResponseParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class ParameterDesctiotionParser: public MessageParser
{
public:
    ParameterDesctiotionParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};


class ParseParser: public MessageParser
{
public:
    ParseParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class PasswordMessageParser: public MessageParser
{
public:
    PasswordMessageParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class PortalSuspendedParser: public MessageParser
{
public:
    PortalSuspendedParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class ReadyForQueryParser: public MessageParser
{
public:
    ReadyForQueryParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class RowDescriptionParser: public MessageParser
{
public:
    RowDescriptionParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};

class SslRequestParser: public MessageParser
{
public:
    SslRequestParser();
    std::string parse(const std::string& type, char* buffer, int len) override;
};
