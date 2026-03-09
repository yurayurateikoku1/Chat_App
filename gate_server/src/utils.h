#pragma once
#include <string>
namespace utils
{

    /// @brief 字符转十六进制
    /// @param c
    /// @return
    inline unsigned char char2Hex(unsigned char c)
    {
        return c > '9' ? c - 'A' + 10 : c + 48;
    }

    /// @brief 十六进制转字符
    /// @param h
    /// @return
    inline unsigned char hex2Char(unsigned char h)
    {
        unsigned char c;
        if (h >= 'A' && h <= 'Z')
        {
            c = h - 'A' + 10;
        }
        else if (h >= 'a' && h <= 'z')
        {
            c = h - 'a' + 10;
        }
        else if (h >= '0' && h <= '9')
        {
            c = h - '0';
        }
        else
        {
            c = 0;
        }
        return c;
    }

    /// @brief url编码
    /// @param str
    /// @return
    inline std::string urlEncode(const std::string &str)
    {
        std::string res;
        res.reserve(str.size());
        for (size_t i = 0; i < str.size(); i++)
        {
            if (isalnum((unsigned char)str[i]) || str[i] == '-' || str[i] == '_' || str[i] == '.' || str[i] == '~')
            {
                res += str[i];
            }
            else if (str[i] == ' ')
            {
                res += "+";
            }
            else
            {
                res += '%';
                res += char2Hex((unsigned char)str[i] >> 4);
                res += char2Hex((unsigned char)str[i] % 16);
            }
        }
        return res;
    }

    /// @brief url解码
    /// @param str
    /// @return
    inline std::string urlDecode(const std::string &str)
    {
        std::string res;
        for (size_t i = 0; i < str.size(); i++)
        {
            if (str[i] == '+')
            {
                res += ' ';
            }
            else if (str[i] == '%' && i + 2 < str.size())
            {
                res += (char)(hex2Char(str[i + 1]) << 4 | hex2Char(str[i + 2]));
                i += 2; // 跳过后面两个十六进制字符
            }
            else
            {
                res += str[i];
            }
        }
        return res;
    }

} // namespace utils
