#pragma once
#include <map>
#include <string>

/// @brief 配置信息
struct SectionInfo
{
    SectionInfo() {};

    SectionInfo(const SectionInfo &other) { items_ = other.items_; }
    SectionInfo &operator=(const SectionInfo &other)
    {
        if (this == &other)
            return *this;
        this->items_ = other.items_;
        return *this;
    }

    ~SectionInfo()
    {
        items_.clear();
    }

    std::map<std::string, std::string> items_;
    std::string operator[](const std::string &key)
    {
        if (items_.find(key) == items_.end())
        {
            return "";
        }
        return items_[key];
    }
};

class ConfigMgr
{
public:
    static ConfigMgr &getInstance()
    {
        static ConfigMgr instance_;
        return instance_;
    }

    ConfigMgr(const ConfigMgr &other) { config_sections_ = other.config_sections_; }
    ConfigMgr &operator=(const ConfigMgr &other)
    {
        if (this == &other)
            return *this;
        this->config_sections_ = other.config_sections_;
        return *this;
    }
    ~ConfigMgr();

    SectionInfo operator[](const std::string &section)
    {
        if (config_sections_.find(section) == config_sections_.end())
        {
            return SectionInfo();
        }

        return config_sections_[section];
    }

private:
    ConfigMgr();
    std::map<std::string, SectionInfo> config_sections_;
};
