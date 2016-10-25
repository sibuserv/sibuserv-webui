/*****************************************************************************
 *                                                                           *
 *  Copyright (c) 2016 Boris Pek <tehnick-8@yandex.ru>                       *
 *                                                                           *
 *  Permission is hereby granted, free of charge, to any person obtaining    *
 *  a copy of this software and associated documentation files (the          *
 *  "Software"), to deal in the Software without restriction, including      *
 *  without limitation the rights to use, copy, modify, merge, publish,      *
 *  distribute, sublicense, and/or sell copies of the Software, and to       *
 *  permit persons to whom the Software is furnished to do so, subject to    *
 *  the following conditions:                                                *
 *                                                                           *
 *  The above copyright notice and this permission notice shall be included  *
 *  in all copies or substantial portions of the Software.                   *
 *                                                                           *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,          *
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF       *
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     *
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     *
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   *
 *                                                                           *
 *****************************************************************************/

#include <QString>
#include <QStringList>
#include <QJsonObject>

#include "applicationsettings.h"

#define RETURN_APPLICATIONS_SETTINGS_VARIABLE(KEY) \
    if (d->isReadOnly) { \
    static const QString out = get(KEY); \
    return out; \
    } \
    return get(KEY);

struct ApplicationSettings::ApplicationSettingsPrivate
{
    ApplicationSettingsPrivate()
    {
        settings["prefix_string"]      = "/sibuserv/";
        settings["config_directory"]   = "/home/sibuserv/.config/sibuserv-webui";
        settings["cache_directory"]    = "/home/sibuserv/.local/share/sibuserv-webui/cache";
        settings["log_directory"]      = "/home/sibuserv/.local/share/sibuserv-webui/logs";
        settings["sessions_directory"] = "/home/sibuserv/.local/share/sibuserv-webui/sessions";
    }
    QJsonObject settings;
    bool isReadOnly = false;
};

ApplicationSettings::ApplicationSettings() :
    AbstractSettings("/etc/sibuserv/webui-settings.json"),
    d(new ApplicationSettingsPrivate)
{
    ;
}

ApplicationSettings::~ApplicationSettings()
{
    delete d;
}

ApplicationSettings &ApplicationSettings::instance()
{
    static ApplicationSettings inst;
    return inst;
}

void ApplicationSettings::update(const Options &options)
{
    if (d->isReadOnly)
        return;

    if (options.isConfigFileDefine()) {
        setFileName(options.configFile());
    }
    readSettings();
}

void ApplicationSettings::finalize()
{
    d->isReadOnly = true;

    prefixString();
    configDirectory();
    cacheDirectory();
    logDirectory();
    sessionsDirectory();

    clear();
    AbstractSettings::clear();
}

QString ApplicationSettings::prefixString() const
{
    RETURN_APPLICATIONS_SETTINGS_VARIABLE("prefix_string");
}

QString ApplicationSettings::configDirectory() const
{
    RETURN_APPLICATIONS_SETTINGS_VARIABLE("config_directory");
}

QString ApplicationSettings::cacheDirectory() const
{
    RETURN_APPLICATIONS_SETTINGS_VARIABLE("cache_directory");
}

QString ApplicationSettings::logDirectory() const
{
    RETURN_APPLICATIONS_SETTINGS_VARIABLE("log_directory");
}

QString ApplicationSettings::sessionsDirectory() const
{
    RETURN_APPLICATIONS_SETTINGS_VARIABLE("sessions_directory");
}

void ApplicationSettings::clear()
{
    for (const auto &key : d->settings.keys()) {
        d->settings.remove(key);
    }
}

QString ApplicationSettings::get(const QString &key) const
{
    if (AbstractSettings::contains(key))
        return AbstractSettings::get(key);

    return d->settings[key].toString();
}

