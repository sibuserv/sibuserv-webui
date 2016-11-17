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
#include "buildresultsitem.h"

static const QString dateTimeFormat = "yyyy-MM-dd hh:mm:ss";
static const int pkgVer = 2;  // Current version of json structure

BuildResultsItem::BuildResultsItem(const QString &projectName,
                                   const QString &version,
                                   const QString &target) :
    AbstractSettings(APP_S().cacheDirectory() + "/projects/" + projectName +
                     "/" + version, target + ".json")
{
    if (requiresUpdate()) {
        generate(projectName, version, target);

        const QString &&status = get("status");
        if (status == "passed" || status == "failed") {
            writeSettings();
        }
    }
}

QJsonObject& BuildResultsItem::getJsonObject() const
{
    return AbstractSettings::getSettings();
}

void BuildResultsItem::generate(const QString &projectName,
                                const QString &version,
                                const QString &target)
{
    if (projectName.isEmpty() || version.isEmpty() || target.isEmpty())
        return;

    QDir dir(APP_S().buildServerBinDir() + "/" + projectName + "/" +
             version + "/" + target);
    const auto &&entries = dir.entryList(QDir::AllEntries |
                                         QDir::NoDotAndDotDot);

    if (entries.isEmpty())
        return;

    QString status = "passed";
    QString logFile;
    QString binFile;

    const QString &&codeAnalysis = APP_S().staticCodeAnalysisLogsSubdir();
    if (target == codeAnalysis) {
        logFile = "cppcheck.log";

        if (isStaticCodeAnalysisFailed(dir.absolutePath())) {
            status = "failed";
        }
        if (entries.contains("cppcheck.html")) {
            binFile = "cppcheck.html";
        }
    }
    else {
        logFile = "build.log";

        int counter = 0;
        for (const auto &k : entries) {
            if (k.endsWith("Makefile")) {
                status = "started";
                binFile = logFile;
            }
            else if (!k.endsWith(".log")) {
                binFile = k;
                ++counter;
            }
        }
        if (counter == 0) {
            status = "failed";
            binFile = logFile;
        }
        else if (counter == 1) {
            ; // binFile is already defined above
        }
        else {
            binFile = projectName + "-" + version + "_" + target + ".tar.gz";
        }
    }

    const QFileInfo fileInfo(dir.absolutePath() + "/" + logFile);
    const QString &&started  = getStartTimeFromLogFile(fileInfo.absoluteFilePath());
    const QString &&finished = fileInfo.lastModified().toString(dateTimeFormat);
    const qint64  &&duration = calcDuration(started, finished);

    const QJsonObject tmp = {
        { "project_name",   projectName },
        { "version",        version },
        { "target",         target },
        { "status",         status },
        { "started",        started },
        { "finished",       finished },
        { "duration",       duration },
        { "log_file",       logFile },
        { "bin_file",       binFile },
        { "pkg_ver",        pkgVer }
    };
    setSettings(tmp);
}

QString BuildResultsItem::getStartTimeFromLogFile(const QString &absoluteFilePath) const
{
    QString out;

    static const QByteArray testString = "] start of ";

    QByteArray line;
    QFile f(absoluteFilePath);
    if (f.open(QIODevice::ReadOnly)) {
        line = f.readLine();
        f.close();

        if (line.contains(testString)) {
            out = QString::fromUtf8(line.mid(1, 19));
        }
    }

    return out;
}

qint64 BuildResultsItem::calcDuration(const QString &started, const QString &finished) const
{
    if (started.isEmpty() || finished.isEmpty())
        return 0;

    const QDateTime t1 = QDateTime::fromString(started,  dateTimeFormat);
    const QDateTime t2 = QDateTime::fromString(finished, dateTimeFormat);
    return t1.secsTo(t2);
}

bool BuildResultsItem::isStaticCodeAnalysisFailed(const QString &dir) const
{
    const ResourceManager rm(dir);
    const QByteArray data = rm.read("cppcheck.stats.txt");
    if (data.contains(" error")) {
        return true;
    }
    return false;
}

bool BuildResultsItem::requiresUpdate()
{
    if (getSettings().isEmpty())
        return true;

    return (getInt("pkg_ver") < pkgVer);
}

