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

#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "debugpage.h"

DebugPage::DebugPage(const Request &request) :
    HtmlPage(request)
{
    if (request.get("ajax").isEmpty()) {
        addToTitle(" - %debug%");
        generateHtmlTemplate(request);
        update();
    }
    else {
        generateAjaxResponse(request);
    }
    show();
}

void DebugPage::generateHtmlTemplate(const Request &request)
{
    if (isAutorizedUser() && isAdmin()) {
        QByteArray content;
        for (const auto &k : request.environment()) {
            content += k.toUtf8() + "<br>\n";
        }
        content += "POST_DATA=<pre style=\"display: inline;\">";
        content += request.post();
        content += "</pre><br>\n";
        setContent(content);
    }
    else {
        forbidAccess();
        if (!isAutorizedUser()) {
            forceAuthorization();
        }
    }
}

void DebugPage::generateAjaxResponse(const Request &request)
{
    if (isAutorizedUser() && isAdmin()) {
        QJsonObject out;
        int idx = -1;
        for (const auto &k : request.environment()) {
            idx = k.indexOf("=");
            if (idx >= 0 && idx < k.size()-1) {
                out[k.mid(0, idx)] = k.mid(idx + 1);
            }
        }
        out["POST_DATA"] = QString::fromUtf8(request.post());
        setData(QJsonDocument(out).toJson());
    }
}

