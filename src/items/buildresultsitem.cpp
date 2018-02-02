/*****************************************************************************
 *                                                                           *
 *  Copyright (c) 2016-2018 Boris Pek <tehnick-8@yandex.ru>                  *
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

#include "logmanager.h"
#include "resourcemanager.h"
#include "applicationsettings.h"
#include "buildresultsitem.h"

static const QString dateTimeFormat = "yyyy-MM-dd hh:mm:ss";
static const int pkgVer = 2;  // Current version of json structure

BuildResultsItem::BuildResultsItem(const QString &projectName,
                                   const QString &version,
                                   const QString &target) :
    AbstractTableItem(APP_S().cacheDirectory() + "/projects/" + projectName +
                      "/" + version, target + ".json")
{
    if (requiresUpdate(pkgVer)) {
        generate(projectName, version, target);
        saveJsonObjectToCache();
    }
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
        binFile = "cppcheck.html/";

        if (isStaticCodeAnalysisFailed(dir.absolutePath())) {
            status = "failed";
        }
        else {
            const ResourceManager rm(dir.absolutePath());
            if (!rm.contains("cppcheck.stats.txt")) {
                status = "started";
            }
        }
    }
    else {
        logFile = "build.log";

        int counter = 0;
        for (const auto &k : entries) {
            if (k.endsWith("Makefile")) {
                status = "started";
                binFile = logFile;
                ++counter;
                break;
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
            if (QFileInfo(dir.absoluteFilePath(binFile)).isDir()) {
                binFile = projectName + "-" + version + ".zip";
            }
        }
        else if (status != "started") {
            binFile = projectName + "-" + version + ".zip";
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

bool BuildResultsItem::isStaticCodeAnalysisFailed(const QString &dir) const
{
    const ResourceManager rm(dir);
    const QByteArray data = rm.read("cppcheck.stats.txt");
    if (data.contains(" error")) {
        return true;
    }
    return false;
}

