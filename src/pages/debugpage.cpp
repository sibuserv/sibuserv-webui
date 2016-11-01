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

#include <QByteArray>

#include "debugpage.h"

DebugPage::DebugPage(const Request &request) :
    HtmlPage(request)
{
    QByteArray body;
    for (const auto &k : request.environment()) {
        body += k.toUtf8() + "<br>\n";
    }
    body += "POST_DATA=" + request.post() + "<br>\n";

    setCookie("user_id", "shdkjvh6545dsgdfg5465s4f65sg");
    setCookie("session_id", "1dsgdxv78786642134vkbvbds");
    setCookie("user_name", "user");

    const QString userId = request.cookie("user_id");
    if (!userId.isEmpty()) {
        setCookie("user_id", userId + "6");
    }
    const QString userName = request.get("user_name");
    if (!userName.isEmpty()) {
        setCookie("user_name", userName);
    }

    addToTitle(" - %debug%");
    setBody(body);
    update();
    show();
}

