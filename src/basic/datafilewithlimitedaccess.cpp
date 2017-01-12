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
#include <QFileInfo>
#include <QProcess>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "logmanager.h"
#include "applicationsettings.h"
#include "resourcemanager.h"
#include "datafilewithlimitedaccess.h"

DataFileWithLimitedAccess::DataFileWithLimitedAccess(const Request &request,
                                                     const QString &projectName,
                                                     const QString &fileName) :
    HtmlPage(request)
{
    if (request.get("ajax").isEmpty()) {
        generateHtmlTemplate(projectName, fileName);
    }
    else {
        generateAjaxResponse(request, projectName, fileName);
    }
    show();
}

bool DataFileWithLimitedAccess::isAllowedAccess(const QString &projectName,
                                                const QString &fileName) const
{
    if (isAdmin())
        return true;

    const UserSettings &us = userSettings();
    const QJsonObject &&obj = us.getObject("projects");

    if (!obj.contains(projectName))
        return false;

    const QString &&role = obj[projectName].toString();

    if (role == "tester") {
        static const QStringList logFiles = {
            "build.log",
            "cppcheck.log",
            "cppcheck.html"
        };
        for (const auto &f : logFiles) {
            if (fileName.contains(f)) {
                return false;
            }
        }
        return true;
    }

    return (role == "developer" || role == "owner");
}

void DataFileWithLimitedAccess::generateHtmlTemplate(const QString &projectName,
                                                     const QString &fileName)
{
    if (isAllowedAccess(projectName, fileName)) {
        ResourceManager res(APP_S().cacheDirectory() + "/projects");
        res.addPath(APP_S().buildServerBinDir());
        if (!res.contains(fileName) && fileName.endsWith(".zip")) {
            makeArchive(projectName, fileName);
        }
        if (res.contains(fileName)) {
            setData(res.read(fileName));
            autodetectContentType(fileName);
        }
        else {
            update();
        }
    }
    else {
        forbidAccess();
        if (!isAutorizedUser()) {
            forceAuthorization();
        }
        update();
    }
}

void DataFileWithLimitedAccess::generateAjaxResponse(const Request &request,
                                                     const QString &projectName,
                                                     const QString &fileName)
{
    if (!request.isPost())
        return;

    if (request.post("id").isEmpty())
        return;

    if (!isAllowedAccess(projectName, fileName)) {
        const QJsonObject out = {
            {"id",          request.post("id")},
            {"file_name",   QFileInfo(fileName).fileName()},
            {"size",        0},
            {"sha256sum",   "?"}
        };
        setData(QJsonDocument(out).toJson());
        return;
    }

    const QString &&cacheDirPath = APP_S().cacheDirectory() + "/projects";
    const QString &&binDirPath   = APP_S().buildServerBinDir();

    ResourceManager res(cacheDirPath);
    res.addPath(binDirPath);
    if (!res.contains(fileName) && fileName.endsWith(".zip")) {
        makeArchive(projectName, fileName);
    }

    if (!res.contains(fileName))
        return;

    const QByteArray &&data = res.read(fileName);

    const auto sha256sum = [&data]() {
        const QByteArray &&out =
                QCryptographicHash::hash(data, QCryptographicHash::Sha256)
                .toHex();
        return QString::fromLatin1(out);
    };

    const QJsonObject out = {
        {"id",          request.post("id")},
        {"file_name",   QFileInfo(fileName).fileName()},
        {"size",        data.size()},
        {"sha256sum",   sha256sum()}
    };
    setData(QJsonDocument(out).toJson());
}

bool DataFileWithLimitedAccess::makeArchive(const QString &projectName,
                                            const QString &fileName) const
{
    if (projectName.isEmpty() || fileName.isEmpty())
        return false;

    const int idx = fileName.lastIndexOf("/");
    if (idx < 0)
        return false;

    // Directory with files for archiving:
    const QString in = APP_S().buildServerBinDir() + "/" + fileName.mid(0, idx);

    if (!QDir(in).exists())
        return false;

    // Archive file:
    const QString out = APP_S().cacheDirectory() + "/projects/" + fileName;

    QFile f(out + ".lock");
    if (f.exists()) {
        return false;
    }
    else {
        QDir().mkpath(QFileInfo(f).absolutePath());
        if (f.open(QIODevice::OpenModeFlag::WriteOnly)) {
            f.close();
        }
        else {
            return false;
        }
    }

    const QString program = "zip";
    const QStringList arguments = {
        "-rq9",
        QFileInfo(out).absoluteFilePath(),
        QFileInfo(in).fileName(),
        "-x '*.log'"
    };
    QProcess process;
    process.setWorkingDirectory(QFileInfo(in).absolutePath());
    process.start(program, arguments);
    process.waitForFinished(300000); // limit by 5 minutes

    f.remove();
    return (process.exitStatus() == QProcess::NormalExit);
}

