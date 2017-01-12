/*****************************************************************************
 *                                                                           *
 *  Copyright (c) 2016-2017 Boris Pek <tehnick-8@yandex.ru>                  *
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
#include <QDateTime>

#include "resourcemanager.h"
#include "applicationsettings.h"
#include "buildresultsitem.h"
#include "buildhistoryitem.h"

static const QString dateTimeFormat = "yyyy-MM-dd hh:mm:ss";
static const int pkgVer = 2;  // Current version of json structure

BuildHistoryItem::BuildHistoryItem(const QString &projectName,
                                   const QString &version) :
    AbstractTableItem(APP_S().cacheDirectory() + "/projects/" + projectName,
                      version + ".json")
{
    if (requiresUpdate(pkgVer)) {
        generate(projectName, version);
        saveJsonObjectToCache();
    }
}

void BuildHistoryItem::generate(const QString &projectName,
                                const QString &version)
{
    if (projectName.isEmpty() || version.isEmpty())
        return;

    QDir dir(APP_S().buildServerBinDir() + "/" + projectName + "/" + version);
    dir.setSorting(QDir::Time | QDir::Reversed);
    const auto &&targets = dir.entryInfoList(QDir::AllDirs |
                                             QDir::NoDotAndDotDot);

    // The most accurate detection of beginning time: time when top
    // subdirectory was created. Unfortunately created() method returns wrong
    // result, so an empty string is used here.
    QString started;

    // The most accurate detection of ending time: last modified file or
    // subdirectory in last target (by default it should be subdirectory
    // "StaticCodeAnalysis/cppcheck.html").
    dir.setPath(targets.last().absoluteFilePath());
    QString &&finished = dir.entryInfoList(QDir::AllEntries |
                                           QDir::NoDotAndDotDot).last()
            .lastModified().toString(dateTimeFormat);

    if (QFile(APP_S().buildServerLogFile()).exists()) {
        QString t1;
        QString t2;
        QPair<QString&, QString&> (t1, t2) = getTimestampsFromLogFile(projectName,
                                                                      version);
        if (!t1.isEmpty())
            started = t1;
        if (!t2.isEmpty())
            finished = t2;
    }

    const qint64  &&duration = calcDuration(started, finished);
    const QString &&status   = detectBuildStatus(projectName, version, targets);

    const QJsonObject tmp = {
        { "project_name",   projectName },
        { "version",        version },
        { "status",         status },
        { "started",        started },
        { "finished",       finished },
        { "duration",       duration },
        { "pkg_ver",        pkgVer }
    };
    setSettings(tmp);
}

QPair<QString, QString> BuildHistoryItem::getTimestampsFromLogFile(const QString &projectName,
                                                                   const QString &version) const
{
    if (projectName.isEmpty() || version.isEmpty())
        return qMakePair(QString(), QString());

    QFile f(APP_S().buildServerLogFile());
    if (f.size() <= 0)
        return qMakePair(QString(), QString());

    const QString &&testStarted  = "build: " + projectName + ": " + version;
    const QString &&testFinished = "done:  " + projectName + ": " + version;

    QByteArray ch;
    QString started, finished, line;
    if (f.open(QIODevice::ReadOnly)) {
        f.seek(f.size());
        while (f.pos() > 21) {
            while ((f.pos() > 21) && (ch != "\n")) {
                f.seek(f.pos() - 2);
                ch = f.read(1);
            }
            line = QString::fromUtf8(f.readLine());

            if (line.contains(testFinished)) {
                finished = line.mid(1, 19);
            }
            else if (line.contains(testStarted)) {
                started = line.mid(1, 19);
            }

            if (!finished.isEmpty() && !started.isEmpty()) {
                break;
            }

            f.seek(f.pos() - line.size());
            ch.clear();
        }
        f.close();
    }

    return qMakePair(started, finished);
}

QString BuildHistoryItem::detectBuildStatus(const QString &projectName,
                                            const QString &version,
                                            const QFileInfoList &subdirs) const
{
    if (projectName.isEmpty() || version.isEmpty() || subdirs.isEmpty())
        return "?";

    QString status;
    for (const auto &it : subdirs) {
        if (it.isDir()) {
            BuildResultsItem bri(projectName, version, it.fileName());
            status = bri.get("status");
            if (status == "failed") {
                return status;
            }
            else if (status == "started") {
                return status;
            }
        }
    }
    return "passed";
}

