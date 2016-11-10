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

    const QDir dir(APP_S().buildServerBinDir() + "/" + projectName + "/" + version);
    const auto &&subdirs = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot,
                                             QDir::Time | QDir::Reversed);

    QString started  = subdirs.first().lastModified().toString(dateTimeFormat);
    QString finished = subdirs.last() .lastModified().toString(dateTimeFormat);

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
    const QString &&status   = detectBuildStatus(subdirs);


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

