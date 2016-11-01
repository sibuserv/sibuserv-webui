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

#pragma once

#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QJsonObject>

class AbstractSettings
{
    friend class ApplicationSettings;
    friend class Localization;

public:
    explicit AbstractSettings(const QString &path, const QString &fileName);
    AbstractSettings(const AbstractSettings &in) = delete;
    AbstractSettings(AbstractSettings &&in) = delete;
    AbstractSettings& operator=(const AbstractSettings &in) = delete;
    virtual ~AbstractSettings();

    void setPath(const QString &path);
    void setFileName(const QString &fileName);
    bool readSettings();
    bool writeSettings() const;

    bool contains(const QString &key) const;
    QString get(const QString &key) const;
    QStringList keys() const;

protected:
    inline void clear();
    inline void setSettings(QJsonObject &object);
    inline bool readSetting(const QByteArray &data,
                            QJsonObject &settings);

private:
    struct AbstractSettingsPrivate;
    AbstractSettingsPrivate * const d;
};

