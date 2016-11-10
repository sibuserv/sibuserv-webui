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
#include "resourcemanager.h"
#include "applicationsettings.h"
#include "buildhistoryitem.h"

static const QString dateTimeFormat = "yyyy-MM-dd hh:mm:ss";

BuildHistoryItem::BuildHistoryItem(const QString &projectName,
                                   const QString &version) :
    AbstractSettings(APP_S().cacheDirectory() + "/projects/" + projectName,
                     version + ".json")
{
    if (getSettings().isEmpty()) {
        generate(projectName, version);

        const QString &&status = get("status");
        if (status == "passed" || status == "failed") {
            writeSettings();
        }
    }
}

QJsonObject &BuildHistoryItem::getJsonObject() const
{
    return AbstractSettings::getSettings();
}

void BuildHistoryItem::generate(const QString &projectName,
                                const QString &version)
{
    if (projectName.isEmpty() || version.isEmpty())
        return;

    const auto getTimestamp = [](const QFileInfo &fileInfo) -> QString {
        return fileInfo.lastModified().toString(dateTimeFormat);
    };

    QDir dir(APP_S().buildServerBinDir() + "/" + projectName + "/" + version);
    dir.setSorting(QDir::Time | QDir::Reversed);

    const auto &&targets = dir.entryInfoList(QDir::AllDirs |
                                             QDir::NoDotAndDotDot);
    const QString started = getTimestamp(targets.first());

    // The most accurate detection of ending time: last modified file or
    // subdirectory in last target (by default it should be subdirectory
    // "StaticCodeAnalysis/cppcheck.html").
    dir.setPath(targets.last().absoluteFilePath());
    const QString finished = getTimestamp(dir.entryInfoList(QDir::AllEntries |
                                                            QDir::NoDotAndDotDot)
                                          .last());

    const qint64  &&duration = calcDuration(started, finished);
    const QString &&status   = detectBuildStatus(targets);

    const QJsonObject tmp = {
        { "project_name",   projectName },
        { "version",        version },
        { "status",         status },
        { "started",        started },
        { "finished",       finished },
        { "duration",       duration }
    };
    setSettings(tmp);
}

qint64 BuildHistoryItem::calcDuration(const QString &started,
                                      const QString &finished) const
{
    const QDateTime t1 = QDateTime::fromString(started,  dateTimeFormat);
    const QDateTime t2 = QDateTime::fromString(finished, dateTimeFormat);
    return t1.secsTo(t2);
}

QString BuildHistoryItem::detectBuildStatus(const QFileInfoList &subdirs) const
{
    if (subdirs.isEmpty())
        return "";

    const QString &&codeAnalysis = APP_S().staticCodeAnalysisLogsSubdir();

    for (const auto &it : subdirs) {
        if (it.isDir()) {
            if ( it.fileName() == codeAnalysis) {
                if (isStaticCodeAnalysisFailed(it.absoluteFilePath())) {
                    return "failed";
                }
            }
            else {
                int counter = 0;
                const QDir dir(it.absoluteFilePath());
                for (const auto &k : dir.entryList(QDir::AllEntries |
                                                   QDir::NoDotAndDotDot)) {
                    if (k.endsWith("Makefile")) {
                        return "started";
                    }
                    else if (!k.endsWith(".log")) {
                        ++counter;
                    }
                }
                if (counter == 0) {
                    return "failed";
                }
            }
        }
    }

    return "passed";
}

bool BuildHistoryItem::isStaticCodeAnalysisFailed(const QString &dir) const
{
    const ResourceManager rm(dir);
    const QByteArray data = rm.read("cppcheck.stats.txt");
    if (data.contains(" error")) {
        return true;
    }
    return false;
}

