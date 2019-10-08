/****************************************************************************
Copyright (c) 2014-2019 Gabriel Heilig
fresh-engine
funkyzooink@gmail.com
****************************************************************************/
#include "Localization.h"
#include "external/json/document.h"
#include "external/json/filereadstream.h"

#include "../GameData/GameConfig.h"

#include "cocos2d.h"

std::string Localization::getLocalizedString(const std::string& key)
{
    return getLocalizedString(key, "");  // TODO
}

std::string Localization::getLocalizedString(const std::string& key, const std::string& value)
{
    auto localized = GAMECONFIG.getLocalizedString(key);

    if (value.size() > 0)
    {
        if (!replace(localized, "$value", value))
        {
            cocos2d::log("Localization replace error with string %s:, value %s:", localized.c_str(), value.c_str());
        }
    }

    return localized;
}

bool Localization::replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
