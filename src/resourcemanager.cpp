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
    // Empty paths are not allowed due to security reasons.
    if (path.isEmpty())
        return;

    // Duplicates are allowed.
    d->paths.prepend(path);
}

bool ResourceManager::contains(const QString &fileName) const
{
    if (fileName.isEmpty())
        return false;

    QFile file;
    for (const auto &path : d->paths) {
        if (fileName.startsWith("/")) {
            file.setFileName(path + fileName);
        }
        else {
            file.setFileName(path + "/" + fileName);
        }
        if (file.exists()) {
            return true;
        }
    }

    return false;
}

QByteArray ResourceManager::read(const QString &fileName) const
{
    QByteArray out;

    if (fileName.isEmpty())
        return out;

    QFile file;
    for (const auto &path : d->paths) {
        if (fileName.startsWith("/")) {
            file.setFileName(path + fileName);
        }
        else {
            file.setFileName(path + "/" + fileName);
        }
        if (file.open(QIODevice::ReadOnly)) {
            out = file.readAll();
            file.close();
            return out;
        }
    }

    return out;
}

bool ResourceManager::write(const QString &fileName,
                            const QByteArray &data,
                            const bool append) const
{
    if (data.isEmpty() || fileName.isEmpty())
        return false;



    const QIODevice::OpenModeFlag flag = append ?
                QIODevice::OpenModeFlag::Append :
                QIODevice::OpenModeFlag::WriteOnly;

    qint64 len = -1;
    QFile file(fileName);
    QDir().mkpath(QFileInfo(file).absolutePath());
    if (file.open(flag)) {
        len = file.write(data);
        file.close();
    }

    return (len > 0);
}

