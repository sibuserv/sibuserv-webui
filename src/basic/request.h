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

#include <QMap>
#include <QString>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>

class Request
{
public:
    Request();
    Request(const Request &in) = delete;
    Request(Request &&in) = delete;
    Request& operator=(const Request &in) = delete;
    ~Request();

    bool next() const;
    bool isGet() const;
    bool isPost() const;

    bool receivedJsonArray() const;
    bool receivedJsonObject() const;

    QList<QString> environment() const;
    QString get(const QString &key) const;
    QString post(const QString &key) const;
    QByteArray& post() const;
    QString cookie(const QString &key) const;
    QString scriptName() const;

    QJsonArray  getJsonArray() const;
    QJsonObject getJsonObject() const;

protected:
    inline QString getEnv(const QString &var) const;
    inline QString getValue(const QString &buff,
                            const QString &key,
                            const QString &separator) const;

private:
    struct RequestPrivate;
    RequestPrivate * const d;
};

