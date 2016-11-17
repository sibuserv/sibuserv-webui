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

#include <QCryptographicHash>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include "logmanager.h"
#include "applicationsettings.h"
#include "usersettings.h"

UserSettings::UserSettings() :
    AbstractSettings(APP_S().configDirectory(), "user-settings.json")
{
    ;
}

UserSettings::UserSettings(const QString &fileName) :
    AbstractSettings(APP_S().configDirectory(), fileName)
{
    ;
}

QByteArray UserSettings::gravatarIconUrl() const
{
    if (get("user_email").isEmpty())
        return "";

    return "https://secure.gravatar.com/avatar/" +
            calcEmailHash(get("user_email").toLower()) +
            "?s=52&amp;d=blank";
}

QString UserSettings::getProjectRole(const QString &projectName)
{
    return (getObject("projects")[projectName]).toString();
}

QByteArray UserSettings::calcEmailHash(const QString &email)
{
    QByteArray data = email.toUtf8();
    data.replace(" ","");
    return QCryptographicHash::hash(data,
                                    QCryptographicHash::Md5).toHex();
}

QString UserSettings::calcPasswordHash(const QString &password)
{
    return QCryptographicHash::hash(password.toUtf8(),
                                    QCryptographicHash::Sha256).toHex();
}

bool UserSettings::isValidAutorizationRequest(const Request &request)
{
    if (!request.isPost())
        return false;
    if (request.post().isEmpty())
        return false;

    const QString userName = request.post("user_name");
    const QString password = request.post("password");

    if (userName.isEmpty())
        return false;
    if (password.isEmpty())
        return false;

    UserSettings us("users/" + userName + ".json");
    if (!us.get("password_hash").isEmpty()) {
        if (us.get("password_hash") == calcPasswordHash(password)) {
            setFileName("users/" + userName + ".json");
            if (readSettings()) {
                return true;
            }
        }
        else {
            QByteArray out;
            out += "user_name=" + userName.toUtf8() + ", pwd_size=" +
                    QByteArray::number(password.size());
            LOG("autorization-errors.log", out);
        }
    }

    return false;
}

