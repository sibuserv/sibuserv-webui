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
#include <QList>

#include "applicationsettings.h"
#include "resourcemanager.h"

struct ResourceManager::ResourceManagerPrivate
{
    QList<QString> paths;
};

ResourceManager::ResourceManager() :
    d(new ResourceManagerPrivate)
{
    addPath(":");
}

ResourceManager::ResourceManager(const QString &path) :
    d(new ResourceManagerPrivate)
{
    addPath(path);
}

ResourceManager::ResourceManager(const QList<QString> &paths) :
    d(new ResourceManagerPrivate)
{
    for (const auto &path : paths) {
        addPath(path);
    }
}

ResourceManager::~ResourceManager()
{
    delete d;
}

void ResourceManager::clearPaths()
{
    d->paths.clear();
}

void ResourceManager::addPath(const QString &path)
{
    if (path.isEmpty())
        return;

    d->paths.prepend(path);
}

void ResourceManager::setPath(const QString &path)
{
    clearPaths();
    addPath(path);
}

bool ResourceManager::contains(const QString &fileName) const
{
    if (fileName.isEmpty())
        return false;

    QFile f;
    for (const auto &path : d->paths) {
        if (fileName.startsWith("/")) {
            f.setFileName(path + fileName);
        }
        else {
            f.setFileName(path + "/" + fileName);
        }
        if (f.exists()) {
            return true;
        }
    }

    return false;
}

QString ResourceManager::find(const QString &fileName) const
{
    QString out;

    if (fileName.isEmpty())
        return out;

    QFile f;
    for (const auto &path : d->paths) {
        if (fileName.startsWith("/")) {
            f.setFileName(path + fileName);
        }
        else {
            f.setFileName(path + "/" + fileName);
        }
        if (f.exists()) {
            return path;
        }
    }

    return out;
}

QByteArray ResourceManager::read(const QString &fileName) const
{
    QByteArray out;

    if (fileName.isEmpty())
        return out;

    QFile f;
    for (const auto &path : d->paths) {
        if (fileName.startsWith("/")) {
            f.setFileName(path + fileName);
        }
        else {
            f.setFileName(path + "/" + fileName);
        }
        if (f.open(QIODevice::ReadOnly)) {
            out = f.readAll();
            f.close();
            return out;
        }
    }

    return out;
}

// fileName variable may include relative path
bool ResourceManager::write(const QString &file,
                            const QByteArray &data,
                            const bool append)
{
    if (data.isEmpty() || file.isEmpty())
        return false;

    const QIODevice::OpenModeFlag flag = append ?
                QIODevice::OpenModeFlag::Append :
                QIODevice::OpenModeFlag::WriteOnly;

    QDir().mkpath(QFileInfo(file).absolutePath());

    qint64 len = -1;
    QFile f(file);
    if (f.open(flag)) {
        len = f.write(data);
        f.close();
    }

    return (len > 0);
}

bool ResourceManager::copy(const QString &file, const QString &target)
{
    // Get real path and make directories tree if necessary.
    QDir().mkpath(QFileInfo(target).absolutePath());
    // Existing files will not be overridden.
    return QFile::copy(file, target);
}

bool ResourceManager::copy(const QString &fileName,
                           const QString &sourcePath,
                           const QString &destinationPath)
{
    if (fileName.startsWith("/")) {
        return copy(sourcePath + fileName, destinationPath + fileName);
    }
    return copy(sourcePath + "/" + fileName, destinationPath + "/" + fileName);
}

bool ResourceManager::unpack(const QString &fileName,
                             const QString &destinationPath)
{
    const bool out = copy(fileName, ":", destinationPath);

    if (out) {
        QFile f(destinationPath + fileName);
        const QFileDevice::Permissions permissions =
                f.permissions() |
                QFileDevice::WriteUser |
                QFileDevice::WriteGroup;
        f.setPermissions(permissions);
    }

    return out;
}

