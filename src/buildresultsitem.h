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
#include <QJsonObject>
#include <QFileInfoList>

#include "abstractsettings.h"

class BuildResultsItem : public AbstractSettings
{
public:
    explicit BuildResultsItem(const QString &projectName,
                              const QString &version,
                              const QString &target);
    BuildResultsItem(const BuildResultsItem &in) = delete;
    BuildResultsItem(BuildResultsItem &&in) = delete;
    BuildResultsItem& operator=(const BuildResultsItem &in) = delete;
    virtual ~BuildResultsItem() = default;

    QJsonObject& getJsonObject() const;

private:
    inline void generate(const QString &projectName,
                         const QString &version,
                         const QString &target);
    inline QString getStartTimeFromLogFile(const QString &absoluteFilePath) const;
    inline qint64 calcDuration(const QString &started, const QString &finished) const;
    inline bool isStaticCodeAnalysisFailed(const QString &dir) const;
    inline bool requiresUpdate();
};

