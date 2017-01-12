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

#include <QByteArray>

#include "commonsettings.h"
#include "usersettings.h"
#include "request.h"
#include "response.h"

class HtmlPage : public Response
{
public:
    explicit HtmlPage(const Request &request);
    HtmlPage(const HtmlPage &in) = delete;
    HtmlPage(HtmlPage &&in) = delete;
    HtmlPage& operator=(const HtmlPage &in) = delete;
    virtual ~HtmlPage();

    void setHead(const QByteArray &head);
    void setBody(const QByteArray &body);
    void setTitle(const QByteArray &title);
    void setContent(const QByteArray &content);

    void addToHead(const QByteArray &head);
    void addToBody(const QByteArray &body);
    void addToTitle(const QByteArray &title);
    void addToContent(const QByteArray &content);
    void addExtraReplacements(const QByteArray &from,
                              const QByteArray &to);

    void addStyleSheetToHead(const QByteArray &styleSheet);
    void addScriptToHead(const QByteArray &script);

    bool isAutorizedUser() const;
    bool isAdmin() const;

protected:
    void setPage(const QByteArray &page);
    void checkAutorization(const Request &request);
    void forceAuthorization();
    void forbidAccess();
    void update();

    CommonSettings& commonSettings() const;
    UserSettings&   userSettings() const;

    /// \brief Get string value from common settings.
    QString get(const QString &key) const;

    /// \brief Get bool value from common settings.
    bool getBool(const QString &key) const;

    QString getUserRole(const QString &projectName) const;

private:
    struct HtmlPagePrivate;
    HtmlPagePrivate * const d;
};

