/*
 * Copyright (C) 2020 Vladimir "allejo" Jimenez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <set>

#include "bzfsAPI.h"
#include "plugin_config.h"

// Define plugin name
const std::string PLUGIN_NAME = "BZDB Lock List";

// Define plugin version numbering
const int MAJOR = 1;
const int MINOR = 0;
const int REV = 0;
const int BUILD = 1;

typedef std::pair<std::string, std::string> KeyValue;
typedef std::vector<KeyValue> KeyValuePairs;

class BZDBLock : public bz_Plugin, public bz_CustomSlashCommandHandler
{
public:
    virtual const char* Name();
    virtual void Init(const char* config);
    virtual void Cleanup();
    virtual int GeneralCallback(const char* name, void* data);
    virtual void Event(bz_EventData* eventData) {};
    virtual bool SlashCommand(int playerID, bz_ApiString command, bz_ApiString /*message*/, bz_APIStringList *params);

private:
    bool isLocked(std::string variable);
    void loadConfiguration();
    void loadWatchList(KeyValuePairs sectionName, bool whitelistMode);

    std::string configPath;
    std::set<std::string> bzdbWatchList = std::set<std::string>();
    bool whitelistMode = false;
};

BZ_PLUGIN(BZDBLock)

const char* BZDBLock::Name()
{
    static const char* pluginBuild;

    if (!pluginBuild)
    {
        pluginBuild = bz_format("%s %d.%d.%d (%d)", PLUGIN_NAME.c_str(), MAJOR, MINOR, REV, BUILD);
    }

    return pluginBuild;
}

void BZDBLock::Init(const char* config)
{
    // Namespace our clip fields to avoid plug-in conflicts
    bz_setclipFieldString("allejo/bzdbLock", Name());

    bz_registerCustomSlashCommand("bzdblock", this);
    bz_registerCustomSlashCommand("reload", this);
    bz_registerCustomSlashCommand("set", this);

    if (strlen(config) == 0)
    {
        bz_debugMessagef(0, "ERROR :: %s :: This plugin requires a configuration file.", PLUGIN_NAME.c_str());
    }

    configPath = config;

    loadConfiguration();
}

void BZDBLock::Cleanup()
{
    Flush();

    bz_removeCustomSlashCommand("bzdblock");
    bz_removeCustomSlashCommand("reload");
    bz_removeCustomSlashCommand("set");
}

int BZDBLock::GeneralCallback(const char* name, void* data)
{
    if (!name)
    {
        return -9999;
    }

    std::string callback = name;

    if (callback == "isBZDBVarLocked")
    {
        auto variable = static_cast<std::string*>(data);

        return (bool)isLocked(*variable);
    }

    return -9999;
}

bool BZDBLock::SlashCommand(int playerID, bz_ApiString command, bz_ApiString /*message*/, bz_APIStringList *params)
{
    if (command == "bzdblock")
    {
        if (!bz_hasPerm(playerID, "setAll"))
        {
            return true;
        }

        if (params->size() == 0)
        {
            bz_sendTextMessagef(BZ_SERVER, playerID, "Usage: /%s [list]", command.c_str());

            return true;
        }

        auto subCommand = params->get(0);

        if (subCommand == "list")
        {
            bz_sendTextMessagef(BZ_SERVER, playerID, "The following variables can%s be changed:", whitelistMode ? "" : "not");

            for (auto variable : bzdbWatchList)
            {
                bz_sendTextMessagef(BZ_SERVER, playerID, " - %s", variable.c_str());
            }
        }
        else
        {
            bz_sendTextMessagef(BZ_SERVER, playerID, "Unknown subcommand: %s", subCommand.c_str());
        }

        return true;
    }
    else if (command == "reload" && bz_hasPerm(playerID, "setAll"))
    {
        if (params->size() == 0 || params->get(0) == "all")
        {
            loadConfiguration();

            return false;
        }

        std::string subCommand = bz_tolower(params->get(0).c_str());

        if (subCommand == "bzdblocklist")
        {
            loadConfiguration();
            bz_sendTextMessage(BZ_SERVER, playerID, "BZDB lock list reloaded");

            return true;
        }

        return false;
    }
    else if (command == "set")
    {
        if (isLocked(params->get(0)))
        {
            bz_sendTextMessagef(BZ_SERVER, playerID, "The %s BZDB variable has been locked by the server owner and cannot be changed.", params->get(0).c_str());

            return true;
        }

        return false;
    }

    return false;
}

bool BZDBLock::isLocked(std::string variable)
{
    auto location = bzdbWatchList.find(bz_tolower(variable.c_str()));

    if (whitelistMode)
    {
        return location == bzdbWatchList.end();
    }

    return location != bzdbWatchList.end();
}

void BZDBLock::loadConfiguration()
{
    auto config = PluginConfig(configPath);

    if (config.errors != 0)
    {
        bz_debugMessagef(0, "ERROR :: %s :: The configuration file given has errors in it.", PLUGIN_NAME.c_str());

        return;
    }

    auto blacklist = config.getSectionItems("bzdb_blacklist");
    auto whitelist = config.getSectionItems("bzdb_whitelist");

    if (blacklist.size() > 0 && whitelist.size() > 0)
    {
        bz_debugMessagef(0, "ERROR :: %s :: Only either a blacklist or a whitelist may be used.", PLUGIN_NAME.c_str());
    }
    else if (blacklist.size() == 0 && whitelist.size() == 0)
    {
        bz_debugMessagef(0, "ERROR :: %s :: You must specify either a blacklist or whitelist.", PLUGIN_NAME.c_str());
    }

    if (blacklist.size() > 0)
    {
        loadWatchList(blacklist, false);
    }
    else if (whitelist.size() > 0)
    {
        loadWatchList(whitelist, true);
    }
}

void BZDBLock::loadWatchList(KeyValuePairs values, bool isWhitelist)
{
    whitelistMode = isWhitelist;
    bzdbWatchList.clear();

    for (auto value : values)
    {
        bzdbWatchList.insert(bz_tolower(value.first.c_str()));
    }
}
