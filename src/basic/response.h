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

#include <QString>
#include <QByteArray>

class Response
{
public:
    Response();
    Response(const Response &in) = delete;
    Response(Response &&in) = delete;
    Response& operator=(const Response &in) = delete;
    virtual ~Response();

    void setData(const QByteArray &data);
    void setContentType(const QString &contentType);
    void setCookie(const QString &key,
                   const QString &value,
                   const QString &date = "",
                   const QString &path = "/",
                   const QString &domain = "",
                   const bool secure = false,
                   const bool httpOnly = true);
    void show() const;

    void autodetectContentType(const QString &fileName = "");

protected:
    inline void showHeaders() const;
    inline void showCookies() const;
    inline void showData() const;

private:
    struct ResponsePrivate;
    ResponsePrivate * const d;
};

