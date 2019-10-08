/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#ifndef HELPERS_LOCATION_H_
#define HELPERS_LOCATION_H_

#include <string>

class Localization
{
  public:
    static std::string getLocalizedString(const std::string& key);
    static std::string getLocalizedString(const std::string& key, const std::string& value);
    static bool replace(std::string& str, const std::string& from, const std::string& to);
};

#endif  // HELPERS_LOCATION_H_
