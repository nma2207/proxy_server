#include "message_parser.h"
#include <cstring>
#include <byteswap.h>
#include <vector>
#include <numeric>
namespace {

int toInt32(char* buffer)
{
    int tmp;
    memcpy(&tmp, buffer, 4);
    return __bswap_constant_32(tmp);
}

short toInt16(char*buffer)
{
    short tmp;
    memcpy(&tmp, buffer, 2);
    return __bswap_constant_16(tmp);
}

template <class T, int SIZE>
std::vector<T> toIntN(char*buffer, int n, T(*f)(char*buffer))
{
    std::vector<T> result(SIZE);
    for(int i=0; i<n; i++) {
        result[i] = f(buffer + i*SIZE);
    }

    return result;
}
std::string toString(char*buffer, int n)
{
    char tmp[n];
    strncpy(tmp, buffer, n);
    return std::string{tmp};
}

template<class T>
std::string toString(std::vector<T> numbers)
{
    std::string result = "[ "+ std::accumulate(numbers.begin(), numbers.end(), std::string{}, [](std::string a, T b){
        return a + ", " + std::to_string(b);
    }) + "]";

    return result;
}

} // namespace


std::string AuthParser::parse(char *buffer, int len)
{
    int messLen = toInt32(buffer+1);
    int code = toInt32(buffer+5);

    if (messLen==0) {
        return type;
    }

    std::string message="";
    switch (code) {
    case 0:
        message = "[AuthenticationOk]";
        break;
    case 2:
        message = "[AuthenticationKerberosV5]";
        break;
    case 3:
        message = "[AuthenticationCleartextPassword]";
        break;
    case 5:
        message = "[AuthenticationMD5Password " + std::string(buffer+9, buffer+13) + "]";
        break;
     case 6:
        message = "[AuthenticationSCMCredential]";
        break;
     case 7:
        message = "[AuthenticationGSS]";
        break;
    case 8:
        message = "[AuthenticationGSSContinue" + std::string(buffer+9, buffer+len) + "]";
        break;
    case 9:
        message = "[AuthenticationSSPI]";
        break;
    }

    return type+":" +message;
}

std::string BackendKeyParser::parse(char *buffer, int len)
{
    int n = toInt32(buffer+1);
    int pid = toInt32(buffer+5);
    int key = toInt32(buffer+9);
    return type + ":[pid="+std::to_string(pid) + " key=" +std::to_string(key) + "]";
}

std::string BindParser::parse(char *buffer, int len)
{
    int offset = 5;

    std::string destinationPortalName = toString(buffer+offset, len-offset);
    offset += destinationPortalName.length();

    std::string sourceStatement = toString(buffer+offset, len-offset);
    offset += sourceStatement.length();

    short paramNumber = toInt16(buffer+offset);
    offset += 2;

    std::vector<short> paramFormatCode = toIntN<short, 2>(buffer+offset, paramNumber, toInt16);
    offset += 2*paramNumber;

    short paramValuesNumber=toInt16(buffer+offset);
    offset+=2;

    int paramValueLength=toInt32(buffer+offset);
    offset+=4;

    std::string paramValue{buffer+offset, buffer+offset+paramValueLength};
    offset+=paramValueLength;

    short resultColumNumber=toInt16(buffer+offset);
    offset+=2;

    std::vector<short> resultColumn = toIntN<short, 2>(buffer+offset, paramNumber, toInt16);


    return type+":["+destinationPortalName+","+sourceStatement+","+toString(paramFormatCode) + toString(resultColumn) +"]";
}

std::string CloseParser::parse(char *buffer, int len)
{
    char state = buffer[5];
    std::string name = toString(buffer+6, len-6);
    return type+":["+state + ","+name+"]";

}

std::string CommandCompleteParser::parse(char *buffer, int len)
{
    std::string tag = toString(buffer+5, len-5);
    return type+":["+tag+"]";
}

std::string CopyDataParser::parse(char *buffer, int len)
{
    int n = toInt32(buffer+1);
    std::string data{buffer+5, buffer+n};
    return type + ":[" + data + "]";
}
