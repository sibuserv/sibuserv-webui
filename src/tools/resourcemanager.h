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

#pragma once

#include <QList>
#include <QString>
#include <QByteArray>

class ResourceManager
{
public:
    explicit ResourceManager();
    explicit ResourceManager(const QString &path);
    explicit ResourceManager(const QList<QString> &paths);
    ResourceManager(const ResourceManager &in) = delete;
    ResourceManager(ResourceManager &&in) = delete;
    ResourceManager& operator=(const ResourceManager &in) = delete;
    virtual ~ResourceManager();

    /// \brief Clear all earlier specified paths.
    /// \details Have in mind that this will affect to results of contains"()",
    /// find"()" and read"()" functions.
    void clearPaths();

    /// \brief Add path to the list of specified paths.
    /// \details Empty paths are not allowed due to security reasons.
    /// Duplicates in the list are allowed.
    /// \param path should include absolute path.
    void addPath(const QString &path);

    /// \param path should include absolute path.
    void setPath(const QString &path);

    /// \details Check file existence in specified paths.
    /// \param fileName may include relative path.
    /// \return \code{.cpp} false \endcode despite of \b fileName value if the
    /// list of specified paths is empty.
    bool contains(const QString &fileName) const;

    /// \details Find file in specified paths and return path to first found
    /// file.
    /// \return empty string despite of \b fileName value if the list of
    /// specified paths is empty.
    QString find(const QString &fileName) const;

    /// \details Find file in specified paths and read data from first found
    /// file.
    /// \return empty string despite of \b fileName value if the list of
    /// specified paths is empty.
    QByteArray read(const QString &fileName) const;

    /// \details "file" variable should include absolute path to the file.
    static bool write(const QString &file,
                      const QByteArray &data,
                      const bool append = false);

    /// \details "file" and "target" variables should include absolute paths to
    /// files.
    static bool copy(const QString &file,
                     const QString &target);

    /// \details "fileName" variable may include relative path.
    /// \details "sourcePath" and "destinationPath" variables should include
    /// absolute paths.
    static bool copy(const QString &fileName,
                     const QString &sourcePath,
                     const QString &destinationPath);

    /// \details "fileName" variable may include relative path.
    /// \details "destinationPath" variable should include absolute path.
    static bool unpack(const QString &fileName,
                       const QString &destinationPath);

private:
    struct ResourceManagerPrivate;
    ResourceManagerPrivate * const d;
};

