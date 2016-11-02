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

#include <functional>

#include <QString>
#include <QFileInfo>
#include <QDir>

#include "applicationsettings.h"
#include "resourcemanager.h"
#include "logmanager.h"
#include "unpacker.h"

EmbeddedResourcesUnpacker::EmbeddedResourcesUnpacker()
{
    const ResourceManager rm;
    const ApplicationSettings &as = ApplicationSettings::instance();

    std::function<void(const QString &)> unpackFilesFromDir;
    unpackFilesFromDir = [&](const QString &path) -> void
    {
        QString fileName;
        for (const QString &entry : QDir(path).entryList()) {
            fileName = path.mid(1) + "/" + entry;
            if (fileName.contains("webui-settings.json")) {
                continue;
            }
            else if (fileName.endsWith(".json")) {
                rm.unpack(fileName, as.configDirectory());
            }
            else if (fileName.endsWith(".css")) {
                rm.unpack(fileName, as.cacheDirectory());
            }
            else if (fileName.endsWith("robots.txt")) {
                rm.unpack(fileName, as.cacheDirectory());
            }
            else if (QFileInfo(":" + fileName).isDir()) {
                unpackFilesFromDir(":" + fileName);
            }
        }
    };

    unpackFilesFromDir(":");
}

