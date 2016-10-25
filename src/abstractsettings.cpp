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

#include <QStringList>
#include <QJsonParseError>
#include <QJsonDocument>

#include "resourcemanager.h"
#include "abstractsettings.h"

struct AbstractSettings::AbstractSettingsPrivate
{
    QString fileName;
    QJsonObject settings;
};

AbstractSettings::AbstractSettings(const QString &fileName) :
    d(new AbstractSettingsPrivate)
{
    d->fileName = fileName;
}

AbstractSettings::~AbstractSettings()
{
    delete d;
}

void AbstractSettings::setFileName(const QString &fileName)
{
    if (fileName.isEmpty())
        return;

    d->fileName = fileName;
}

bool AbstractSettings::readSettings()
{
    if (d->fileName.isEmpty())
        return false;

    const ResourceManager res;
    if (res.contains(d->fileName)) {
        if (readSetting(res.read(d->fileName))) {
            return true;
        }
    }

    return false;
}

bool AbstractSettings::writeSettings() const
{
    if (d->fileName.isEmpty() || d->settings.isEmpty())
        return false;

    const ResourceManager res;
    const QJsonDocument doc(d->settings);
    return res.write(doc.toJson(), d->fileName);
}

void AbstractSettings::clear()
{
    for (const auto &key : d->settings.keys()) {
        d->settings.remove(key);
    }
    d->fileName.clear();
}

void AbstractSettings::setSettings(QJsonObject &object)
{
    d->settings = object;
}

bool AbstractSettings::readSetting(const QByteArray &data)
{
    QJsonDocument doc;
    QJsonParseError err;
    doc = QJsonDocument::fromJson(data, &err);

    if (err.error == QJsonParseError::NoError && !doc.isNull()) {
        d->settings = doc.object();
        return true;
    }

    return false;
}

bool AbstractSettings::contains(const QString &key) const
{
    if (key.isEmpty())
        return false;

    if (!d->settings.isEmpty()) {
        return d->settings.contains(key);
    }

    return false;
}

QString AbstractSettings::get(const QString &key) const
{
    QString out;

    if (key.isEmpty())
        return out;

    if (!d->settings.isEmpty()) {
        out = d->settings[key].toString();
    }

    return out;
}

