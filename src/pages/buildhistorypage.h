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
#include <QStringList>

#include "htmlpage.h"
#include "request.h"

class BuildHistoryPage : public HtmlPage
{
public:
    explicit BuildHistoryPage(const Request &request, const QString &projectName);
    BuildHistoryPage(const BuildHistoryPage &in) = delete;
    BuildHistoryPage(BuildHistoryPage &&in) = delete;
    BuildHistoryPage& operator=(const BuildHistoryPage &in) = delete;
    virtual ~BuildHistoryPage() = default;

protected:
    inline void generateHtmlTemplate(const QString &projectName);
    inline void generateAjaxResponse(const Request &request,
                                     const QString &projectName);
    inline bool isAllowedAccess(const QString &projectName) const;

    /// \brief Unsorted list of builds.
    inline QStringList allBuilds(const QString &projectName) const;

    /// \brief Natural sort of string list in reverse order.
    inline void sortBuilds(QStringList &builds) const;
};

