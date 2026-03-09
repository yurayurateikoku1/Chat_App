#pragma once
#include <QString>
enum class ReqId
{
    ID_GET_VERIFY_CODE = 1001,
    ID_REG_USER = 1002
};

enum class Modules
{
    REGISTER = 0,
    LOGIN = 1,
    CHAT = 2
};

enum class ErrorCode
{
    SUCCESS = 0,
    ERROR_JSON = 1,
    ERROR_NETWORK = 2
};

inline QString gate_url_prefix;
