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

#include <fcgi_stdio.h>

#include <QMimeDatabase>
#include <QVector>

#include "response.h"

struct Response::ResponsePrivate
{
    QString contentType = "text/html";
    QVector<QString> cookies;
    QByteArray data;
};

Response::Response() :
    d(new ResponsePrivate)
{
    ;
}

Response::~Response()
{
    delete d;
}

void Response::setData(const QByteArray &data)
{
    d->data = data;
}

void Response::setContentType(const QString &contentType)
{
    if (contentType.isEmpty())
        return;

    d->contentType = contentType;
}

void Response::setCookie(const QString &key,
                         const QString &value,
                         const QString &date,
                         const QString &path,
                         const QString &domain,
                         const bool secure,
                         const bool httpOnly)
{
    if (key.isEmpty())
        return;

    QString cookie = key + "=";

    for (int k = d->cookies.size()-1; k >= 0; --k) {
        if (d->cookies.at(k).contains(cookie)) {
            d->cookies.removeAt(k);
        }
    }

    if (!value.isEmpty())
        cookie += value;
    if (!date.isEmpty())
        cookie += "; expires=" + date;
    if (!path.isEmpty())
        cookie += "; path=" + path;
    if (!domain.isEmpty())
        cookie += "; domain=" + domain;
    if (secure)
        cookie += "; Secure";
    if (httpOnly)
        cookie += "; HttpOnly";

    d->cookies.append(cookie);
}

void Response::show() const
{
    showHeaders();
    showCookies();
    showData();
}

void Response::autodetectContentType(const QString &fileName)
{
    if (d->data.isEmpty())
        return;

    const QMimeDatabase mimeDatabase;
    if (fileName.isEmpty())
        d->contentType = mimeDatabase.mimeTypeForData(d->data).name();
    else
        d->contentType = mimeDatabase
                .mimeTypeForFileNameAndData(fileName, d->data).name();
}

void Response::showHeaders() const
{
    FCGI_printf("Content-type: %s\n", d->contentType.toUtf8().data());
}

void Response::showCookies() const
{
    for (const QString &cookie: d->cookies) {
        FCGI_printf("Set-Cookie: %s\n", cookie.toUtf8().data());
    }
}

void Response::showData() const
{
    FCGI_printf("\n");
    FCGI_fwrite(d->data.data(), d->data.size(), 1, FCGI_stdout);
}

