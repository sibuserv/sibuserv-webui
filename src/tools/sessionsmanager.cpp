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

#include <QFile>
#include <QDateTime>
#include <QJsonObject>
#include <QCryptographicHash>

#include "applicationsettings.h"
#include "sessionsmanager.h"
#include "logmanager.h"

static const QString dateTimeFormat = "yyyy-MM-dd hh-mm-ss t";

struct SessionsManager::SessionsManagerPrivate
{
    QString user_id;
    QString session_id;
};

SessionsManager::SessionsManager(const Request &request) :
    AbstractSettings(APP_S().sessionsDirectory(),
                     request.cookie("user_id") + ".json"),
    d(new SessionsManagerPrivate)
{
    d->user_id = request.cookie("user_id");
    d->session_id = request.cookie("session_id");
}

SessionsManager::~SessionsManager()
{
    delete d;
}

bool SessionsManager::isAutorized() const
{
    if (d->user_id.isEmpty())
        return false;
    if (d->session_id.isEmpty())
        return false;
    if (get("expires").isEmpty())
        return false;
    if (get("user_name").isEmpty())
        return false;
    if (get("session_id") != d->session_id)
        return false;

    const QDateTime expires = QDateTime::fromString(get("expires"),
                                                    dateTimeFormat);
    const QDateTime currentDateTime = QDateTime::currentDateTime();

    if (expires < currentDateTime)
        return true;

    return false;
}

bool SessionsManager::beginNewSession(const QString &userName)
{
    if (userName.isEmpty())
        return false;

    // Session ID will be valid during one day.
    const QDateTime dateTime = QDateTime::currentDateTime().addDays(1);
    const QString expires = dateTime.toString(dateTimeFormat);

    const QString buff = expires + userName + dateTime.toString("z");
    const QByteArray sessionId =
            QCryptographicHash::hash(buff.toUtf8(),
                                     QCryptographicHash::Sha1).toHex();

    QJsonObject obj;
    obj["user_name"]            = userName;
    obj["session_id"]           = QString::fromUtf8(sessionId);
    obj["expires"]              = expires;
    obj["authorization_type"]   = "local";
    setSettings(obj);

    if (!writeSettings())
        return false;

    if (readSettings())
        return true;

    return false;
}

bool SessionsManager::closeSession(const QString &userId)
{
    QFile f(APP_S().sessionsDirectory() + "/" + userId + ".json");
    return f.remove();
}

