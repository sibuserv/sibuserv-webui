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

#pragma once

#include <QString>
#include <QByteArray>

class ResourceManager
{
public:
    explicit ResourceManager();
    explicit ResourceManager(const QString &path);
    ResourceManager(const ResourceManager &in) = delete;
    ResourceManager(ResourceManager &&in) = delete;
    ResourceManager& operator=(const ResourceManager &in) = delete;
    virtual ~ResourceManager();

    void clearPaths();
    void addPath(const QString &path);
    bool contains(const QString &fileName) const;
    QString path(const QString &fileName) const;
    QByteArray read(const QString &fileName) const;

    static bool write(const QString &file,
                      const QByteArray &data,
                      const bool append = false);
    static bool copy(const QString &file,
                     const QString &target);
    static bool copy(const QString &fileName,
                     const QString &sourcePath,
                     const QString &destinationPath);
    static bool unpack(const QString &fileName,
                       const QString &destinationPath);

private:
    struct ResourceManagerPrivate;
    ResourceManagerPrivate * const d;
};

