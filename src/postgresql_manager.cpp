#include "postgresql_manager.h"

PostgreSqlManager::PostgreSqlManager()
{
    _parsersB = {
        {'R', new AuthParser("Authentication")},
        {'K', new BackendKeyParser("BackendKeyData")},
        {'2', new MessageParser("BindComplete")},
        {'3', new MessageParser("CloseComplete")},
        {'C', new CommandCompleteParser("CommandComplete")},
        {'d', new CopyDataParser("CopyData")},
        {'G', new CopyInParser("CopyInResponce")},
        {'H', new CopyOutParser("CopyOutParser")},
        {'W', new CopyBothParser("CopyBothResponce")},
        {'D', new DataRowParser("DataRow")},
        {'I', new MessageParser("EmptyQueryResponce")},
        {'E', new ErrorResponceParser("ErrorResonce")},
        {'V', new FunctionCallResponceParser("FunctionCallResponce")},
        {'v', new NegotiateProtocolVersionParser("NegotiateProtocol")},
        {'n', new MessageParser("NoData")},
        {'N', new NoticeResponseParser("NoticeResponce")},
        {'A', new NotificationResponseParser("NotificationResponce")},
        {'t', new ParameterDescriptionParser("ParameterDescription")},
        {'S', new ParameterStatusParser("ParameterStatus")},
        {'1', new MessageParser("ParseComplete")},
        {'s', new MessageParser("PortalSuspended")},
        {'Z', new ReadyForQueryParser("ReadyForQuery")},
        {'T', new RowDescriptionParser("RowDescription")},
    };

    _parsersF = {
        {'B', new BindParser("Bind")},
        {'C', new CloseParser("Close")},
        {'d', new CopyDataParser("CopyData")},
        {'c', new MessageParser("CopyDone")},
        {'f', new CopyFailParser("CopyFail")},
        {'D', new DescribeParser("Describe")},
        {'E', new ExecuteParser("Execute")},
        {'H', new MessageParser("Flush")},
        {'F', new FunctionCallParser("FunctionCall")},
        {'P', new ParseParser("Parse")},
        {'p', new PasswordMessageParser("PasswordMessage")},
        {'Q', new QueryParser("Query")},
        {'S', new MessageParser("Sync")},
        {'X', new MessageParser("Terminate")},
        {'\0', new WithoutFirstByteParser("")}
    };
}

PostgreSqlManager::~PostgreSqlManager()
{
    for(auto it = _parsersB.begin(); it!=_parsersB.end(); it++)
        delete it->second;

    for(auto it = _parsersF.begin(); it!=_parsersF.end(); it++)
        delete it->second;
}

std::string PostgreSqlManager::parseB(char *buffer, int len)
{
    auto it = _parsersB.find(buffer[0]);
    if (it == _parsersB.end())
        return std::string("CANNOT FIND THAT TYPE ") + buffer[0];
    else
        return it->second->parse(buffer, len);
}

std::string PostgreSqlManager::parseF(char *buffer, int len)
{
    auto it = _parsersF.find(buffer[0]);
    if (it == _parsersF.end())
        return std::string("CANNOT FIND THAT TYPE ") + buffer[0];
    else
        return it->second->parse(buffer, len);
}
