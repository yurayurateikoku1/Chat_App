#include "config_mgr.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

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