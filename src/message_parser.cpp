#include "message_parser.h"
#include <cstring>
#include <byteswap.h>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>

namespace {
/*
 * описаны вспомагательные функции для парсинга
 */

/**
 * @brief toInt32 когда кучу байт надо преобразовать в число 4 байтовое
 * @param buffer Указатель на начало, откуда надо парсить
 * @return 4 байтоове число
 */
int toInt32(char* buffer)
{
    int tmp;
    memcpy(&tmp, buffer, 4);
    return __bswap_constant_32(tmp);
}

/**
 * @brief toInt16 когда кучу байт надо преобразовать в число 2 байтовое
 * @param buffer Указатель на начало, откуда надо парсить
 * @return 2 байтовое число
 */
short toInt16(char*buffer)
{
    short tmp;
    memcpy(&tmp, buffer, 2);
    return __bswap_constant_16(tmp);
}

/**
 * @brief toIntN  Когда надо преобразовать кучу байт в массив чисел
 * @param buffer указатель на начало
 * @param n - количество чисел в массиве
 * T- тип числа, SIZE - его размер в байтах
 * @return вектор чисел
 */
template <class T, int SIZE>
std::vector<T> toIntN(char*buffer, int n, T(*f)(char*buffer))
{
    std::vector<T> result(n);
    for(int i=0; i<n; i++) {
        result[i] = f(buffer + i*SIZE);
    }

    return result;
}

/**
 * @brief toString Преобразование байтов в строку
 * @param buffer Указатель на начало
 * @param n максимальное количсевтво символов в строке
 * @return строку
 */
std::string toString(char*buffer, int n)
{
    char tmp[n];
    strncpy(tmp, buffer, n); // strncpy  копирует от начала, и пока не встретит символ '\0' или конец пакета
    return std::string{tmp};
}

/**
 * @brief Чтобы преобразовать массив чисел в строку для сохранения в лог-файл
 */
template<class T>
std::string toString(const std::vector<T>& numbers)
{
    std::stringstream ss;
    ss << "[";
    for(T n: numbers)
        ss<<n <<", ";
    ss <<"]";
    return ss.str();
}

template std::string toString<short>(const std::vector<short>&);

} // namespace

/*
 * ниже описаны все алгоритмы парсинга пакетов, не вижу смысла коментировать каждый
 */
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

    std::vector<short> resultColumn = toIntN<short, 2>(buffer+offset, resultColumNumber, toInt16);


    std::string res = type+":["+destinationPortalName+","+sourceStatement+","+toString(paramFormatCode) + toString(resultColumn) +"]";

    return res;
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

std::string CopyFailParser::parse(char *buffer, int len)
{
    std::string message = toString(buffer+5, len-5);
    return type + ":[" + message + "]";
}


std::string CopyParser::parse(char *buffer, int len)
{
    std::string formatType = buffer[5]==0 ? "text" : "binary";
    short columNumber = toInt16(buffer+6);
    std::vector<short> formats = toIntN<short, 2>(buffer+8, columNumber, toInt16);
    return type + ":["+formatType + ", " + toString(formats) +"]";
}

std::string DataRowParser::parse(char *buffer, int len)
{
    short columnNumber = toInt16(buffer+5);
    std::string res = "";
    int offset = 7;
    res += ":(";
    for(int i=0; i < columnNumber; i++) {

        int n = toInt32(buffer+offset);
        offset += 4;
        res += std::string{buffer+offset+4, buffer+offset+4+n} + ", ";
        offset += n;
    }
    res += ")";
    return type + res;
}

std::string DescribeParser::parse(char *buffer, int len)
{
    char statement = buffer[5];
    std::string name = toString(buffer+6, len-6);
    return type + "[" + statement + ", " + name + "]";
}


std::string ErrorResponceParser::parse(char *buffer, int len)
{
    char identificator = buffer[5];
    return identificator ? type + "[" + toString(buffer+6, len-6) + "]" : type;
}

std::string ExecuteParser::parse(char *buffer, int len)
{
    std::string nameOfPortal =toString(buffer+5, len-5);
    int rowMaxNumber = toInt32(buffer+5+nameOfPortal.length()+1);
    return type + "["+nameOfPortal + ", "+std::to_string(rowMaxNumber)+"]";
}



std::string FunctionCallResponceParser::parse(char *buffer, int len)
{
    int offset = 5;
    int id = toInt32(buffer+offset);
    offset += 4;
    short formatN = toInt16(buffer+offset);
    offset+=2;
    std::vector<short> codes = toIntN<short, 2>(buffer+offset, formatN, toInt16);
    offset += 2*codes.size();

    short argN = toInt16(buffer+offset);
    offset+=2;
    std::string res = "(";
    for(short i=0; i<argN; i++) {
        int n = toInt32(buffer+offset);
        offset += 4;
        if (n==-1) {
            res+= "NULL,";
        }
        else {
            res += std::string(buffer+offset, buffer+offset+n);
        }
        offset += n;
    }
    res+=")";
    short format = toInt16(buffer+offset);
    return type + "["+toString(codes)+", "+res+","+std::to_string(format)+"]";
}

std::string FunctionCallParser::parse(char *buffer, int len)
{
    int n = toInt32(buffer+5);
    return type + "[" + std::string(buffer+5, buffer+5+n) + "]";
}


std::string NegotiateProtocolVersionParser::parse(char *buffer, int len)
{
    int newestVersion = toInt32(buffer+5);
    int notRecognized = toInt32(buffer+9);
    std::string option = toString(buffer+13, len-13);
    return type+"[" + std::to_string(newestVersion) + ", "+ std::to_string(notRecognized) + ", " + option +"]";
}



std::string NoticeResponseParser::parse(char *buffer, int len)
{
    return type + (buffer[5] ? toString(buffer+6, len-6) : "");
}


std::string NotificationResponseParser::parse(char *buffer, int len)
{
    int id = toInt32(buffer+5);
    std::string channelName = toString(buffer+9, len-9);
    std::string payload = toString(buffer+9+channelName.length(), len-9-channelName.length());
    return type + "[" + std::to_string(id) + ", "+channelName +", "+payload + "]";
}

std::string ParameterDescriptionParser::parse(char *buffer, int len)
{
    short n = toInt16(buffer+5);
    if (!n){
        return type;
    }
    else {
        return type + "[" + toString(toIntN<int, 4>(buffer+7, n, toInt32)) + "]";
    }
}

std::string ParameterStatusParser::parse(char *buffer, int len)
{
    std::string nameOfPar = toString(buffer+5, len-5);
    int n = nameOfPar.length();
    std::string value = toString(buffer+5+n, len-n-5);
    return type + "["+nameOfPar + ", " + value + "]";
}

std::string ParseParser::parse(char *buffer, int len)
{
    int offset =5;
    std::string nameOfStatement = toString(buffer+offset, len-offset);
    offset += nameOfStatement.length();

    std::string query = toString(buffer+offset, len-offset);
    offset += query.length();

    short n = toInt16(buffer+offset);
    if (!n) {
        return type + "[" + nameOfStatement + ", " + query + "]";
    }
    else {
        offset += 2;
        return type + "[" + nameOfStatement + ", " + query + toString(toIntN<int, 4>(buffer+offset, n, toInt32)) + "]";
    }
}



std::string PasswordMessageParser::parse(char *buffer, int len)
{
    return type +"[" + toString(buffer+5, len-5) + "]";
}


std::string QueryParser::parse(char *buffer, int len)
{
    return type +"[" + toString(buffer+5, len-5) + "]";
}

std::string ReadyForQueryParser::parse(char *buffer, int len)
{
    std::string status = "";
    switch(buffer[5]) {
    case 'I':
        status = "idle";
        break;
    case 'T':
        status = "transaction block";
        break;
    case 'E':
        status = "failed transation block";
    }

    return type + status;
}

std::string RowDescriptionParser::parse(char *buffer, int len)
{
    short n = toInt16(buffer+5);
    int offset = 9;

    std::string result = "(";

    for(int i=0; i<n; i++) {
        std::string fieldName = toString(buffer+offset, len-offset);
        offset += fieldName.length() + 6;
        int id = toInt32(buffer+offset);
        result += "{" + fieldName +", " + std::to_string(id) + "}";
        offset += 12;
    }
    result += ")";
    return type + result;
}

std::string WithoutFirstByteParser::parse(char *buffer, int len)
{
    int code = toInt32(buffer+4);
    switch(code) {
    case 80877103: // SSLRequst
        return "SSLRequest";
    case 196608:
        return _parseStatup(buffer, len);
    case 80877102:
        return _parseCancelRequest(buffer, len);
    }

    return "";
}

std::string WithoutFirstByteParser::_parseStatup(char *buffer, int len)
{
     std::string paramName = toString(buffer+8, len-8);
     int offset = 8 + paramName.length() + 1;
     std::string value = toString(buffer+offset, len-offset);
     return "Startup [" + paramName + ", " + value + "]";
}

std::string WithoutFirstByteParser::_parseCancelRequest(char *buffer, int len)
{
    int processId = toInt32(buffer+8);
    int key = toInt32(buffer+12);
    return "CancelRequst [" + std::to_string(processId) + " " + std::to_string(key)+"]";
}

