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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>

#include "logmanager.h"
#include "applicationsettings.h"
#include "projectstableitem.h"

static const QString dateTimeFormat = "yyyy-MM-dd hh:mm:ss";

ProjectsTableItem::ProjectsTableItem(const QString &projectName) :
    AbstractSettings(APP_S().cacheDirectory(), projectName + ".json")
{
    if (getSettings().isEmpty()) {
        generate(projectName);
        writeSettings();
    }
}

QJsonObject &ProjectsTableItem::getJsonObject() const
{
    return AbstractSettings::getSettings();
}

void ProjectsTableItem::generate(const QString &projectName)
{
    const QDir projectBinDir(APP_S().buildServerBinDir() + "/" + projectName);
    const auto &builds = projectBinDir.entryInfoList(QDir::AllDirs |
                                                     QDir::NoDotAndDotDot,
                                                     QDir::Name);
    const int buildsNumber = builds.size();

    QString lastStatus = "?"; // passed, failed, started
    QString lastVersion;
    QString lastTimestamp;

    if (!builds.isEmpty()) {
        lastVersion = builds.last().fileName();
        lastTimestamp = builds.last().lastModified().toString(dateTimeFormat);

        if (QFile(APP_S().buildServerLogFile()).exists()) {
            QString lt = getLastTimestampFromLogFile(projectName, lastVersion);
            if (!lt.isEmpty()) {
                lastTimestamp = lt;
            }
        }

        // Debug mode!
        lastStatus = "passed";
    }

    const QJsonObject tmp = {
        { "project_name",   projectName },
        { "builds_number",  buildsNumber },
        { "last_status",    lastStatus },
        { "last_version",   lastVersion },
        { "last_timestamp", lastTimestamp }
    };
    setSettings(tmp);
}

QString ProjectsTableItem::getLastTimestampFromLogFile(const QString &projectName,
                                                       const QString lastVersion) const
{
    QString out;

    QFile f(APP_S().buildServerLogFile());
    if (f.size() == 0)
        return out;

    const QString &&testString = projectName + ": " + lastVersion;

    QString line;
    QByteArray ch;
    if (f.open(QIODevice::ReadOnly)) {
        f.seek(f.size());
        while (f.pos() > 21) {
            while ((f.pos() > 2) && (ch != "\n")) {
                f.seek(f.pos() - 2);
                ch = f.read(1);
            }
            line = QString::fromUtf8(f.readLine());

            LOG("ProjectsTableItem.log", line.toUtf8());

            if (line.contains(testString)) {
                out = line.mid(1, 19);
                break;
            }
            else {
                f.seek(f.pos() - line.size());
                ch.clear();
            }
        }
        f.close();
    }

    return out;
}

