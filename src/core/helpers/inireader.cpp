#include "IniParser.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

std::map<std::string, std::string> IniParser::parseIniFile(const std::string& filename, const std::string& section, const std::string& subsection) {
    std::map<std::string, std::string> keyValuePairs;

    // Create a property tree object
    boost::property_tree::ptree pt;

    // Parse the INI file
    boost::property_tree::ini_parser::read_ini(filename, pt);

    // Iterate over the sections and subsections
    for (const auto& sec : pt) {
        if (sec.first == section) {
            for (const auto& subsec : sec.second) {
                if (subsec.first == subsection || subsection.empty()) {
                    for (const auto& item : subsec.second) {
                        if (item.first == "value") {
                            keyValuePairs[subsec.first] = item.second.get_value<std::string>();
                        }
                    }
                }
            }
        }
    }

    return keyValuePairs;
}
