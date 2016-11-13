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
#include <QDateTime>
#include <QCollator>

#include "logmanager.h"
#include "applicationsettings.h"
#include "buildhistoryitem.h"
#include "projectstableitem.h"

static const QString dateTimeFormat = "yyyy-MM-dd hh:mm:ss";
static const int pkgVer = 1;  // Current version of json structure

ProjectsTableItem::ProjectsTableItem(const QString &projectName) :
    AbstractSettings(APP_S().cacheDirectory() + "/projects",
                     projectName + ".json")
{
    if (requiresUpdate(projectName)) {
        generate(projectName);

        const QString &&lastStatus = get("last_status");
        if (lastStatus == "passed" || lastStatus == "failed") {
            writeSettings();
        }
    }
}

QJsonObject &ProjectsTableItem::getJsonObject() const
{
    return AbstractSettings::getSettings();
}

void ProjectsTableItem::generate(const QString &projectName)
{
    const QString &&lastVersion = getLastVersion(projectName);
    QString lastStatus = "?";
    QString lastTimestamp;
    int buildsNumber = 0;

    if (!lastVersion.isEmpty()) {
        const QDir projectBinDir(APP_S().buildServerBinDir() + "/" + projectName);
        buildsNumber = projectBinDir.entryList(QDir::AllDirs |
                                               QDir::NoDotAndDotDot).size();

        BuildHistoryItem buildHistoryItem(projectName, lastVersion);
        if (!buildHistoryItem.getJsonObject().isEmpty()) {
            lastStatus = buildHistoryItem.get("status");

            const QString &&finished = buildHistoryItem.get("finished");
            if (!finished.isEmpty()) {
                lastTimestamp = finished;
            }
            else {
                lastTimestamp = buildHistoryItem.get("started");
            }
        }
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

bool ProjectsTableItem::requiresUpdate(const QString &projectName) const
{
    if (getSettings().isEmpty())
        return true;

    if (getInt("pkg_ver") < pkgVer)
        return true;

    return (get("last_version") != getLastVersion(projectName));
}

QString ProjectsTableItem::getLastVersion(const QString &projectName) const
{
    const QDir projectBinDir(APP_S().buildServerBinDir() + "/" + projectName);
    auto builds = projectBinDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    if (!builds.isEmpty()) {
        QCollator coll;
        coll.setNumericMode(true);
        const auto comp = [&coll](const QString &s1, const QString &s2) -> bool {
            return coll.compare(s1, s2) < 0;
        };
        std::sort(builds.begin(), builds.end(), comp);
        return builds.last();
    }
    return "";
}

