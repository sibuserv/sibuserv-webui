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

#include "request.h"

struct Request::RequestPrivate
{
    bool isPostDataStored = false;
    QByteArray postData;
};

Request::Request() :
    d(new RequestPrivate)
{
    ;
}

Request::~Request()
{
    delete d;
}

bool Request::next() const
{
    return (FCGI_Accept() >= 0);
}

bool Request::isGet() const
{
    const QString request_method = getEnv("REQUEST_METHOD");
    return (request_method == "GET");
}

bool Request::isPost() const
{
    const QString request_method = getEnv("REQUEST_METHOD");
    return (request_method == "POST");
}

QList<QString> Request::environment() const
{
    QList<QString> out;

    QString var;
    extern char **environ;
    for (char **current = environ; *current != nullptr; ++current) {
        out.append(QString(*current));
    }

    return out;
}

QString Request::get(const QString &key) const
{
    const QString buff = getEnv("QUERY_STRING");
    return getValue(buff, key, "&");
}

QString Request::post(const QString &key) const
{
    const QString buff = QString::fromUtf8(post());
    return getValue(buff, key, "&");
}

QByteArray Request::post() const
{
    if (d->isPostDataStored)
        return d->postData;

    const int len = getEnv("CONTENT_LENGTH").toInt();
    if (len > 0) {
        char *buff = new char[len];
        FCGI_fread(buff, len, 1, FCGI_stdin);
        d->postData = QByteArray(buff, len);
        delete [] buff;
    }
    d->isPostDataStored = true;

    return d->postData;
}

QString Request::cookie(const QString &key) const
{
    const QString buff = getEnv("HTTP_COOKIE");
    return getValue(buff, key, ";");
}

QString Request::scriptName() const
{
    return getEnv("SCRIPT_NAME");
}

QString Request::getEnv(const QString &var) const
{
    const char *ptr = getenv(var.toUtf8().data());
    return (ptr ? QString(ptr) : "");
}

QString Request::getValue(const QString &buff,
                          const QString &key,
                          const QString &separator) const
{
    if (buff.isEmpty() || key.isEmpty())
        return "";

    int begin = buff.indexOf(key + "=");
    if (begin < 0)
        return "";

    begin += key.size() + 1;
    if (begin >= buff.size())
        return "";

    int end = buff.indexOf(separator, begin);
    if (end < 0)
        end = buff.size();

    return buff.mid(begin, end - begin);
}

