#include "config_mgr.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

std::string ConfigMgr::getValue(const std::string &section, const std::string &key)
{
    if (config_sections_.find(section) == config_sections_.end())
    {
        return std::string();
    }
    return config_sections_[section].items_[key];
}

ConfigMgr::ConfigMgr()
{
    boost::filesystem::path path = boost::filesystem::current_path();
    boost::filesystem::path config_path = path / "config.ini";

    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(config_path.string(), pt);
    for (const auto &section : pt)
    {
        SectionInfo info;
        for (const auto &item : section.second)
        {
            info.items_[item.first] = item.second.get_value<std::string>();
        }
        config_sections_[section.first] = info;
    }
}

ConfigMgr::~ConfigMgr()
{
    config_sections_.clear();
}