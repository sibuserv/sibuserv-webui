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

#include <random>
#include <QMessageAuthenticationCode>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>

#include "logmanager.h"
#include "applicationsettings.h"
#include "usersettings.h"

// http://security.stackexchange.com/questions/3959/recommended-of-iterations-when-using-pkbdf2-sha256
static const int rounds = 400004; // 400 004

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

QString UserSettings::getUserRole(const QString &projectName) const
{
    const QJsonObject &&obj = getObject("projects");

    if (!obj.contains(projectName))
        return "none";

    return obj[projectName].toString();
}

QByteArray UserSettings::calcEmailHash(const QString &email)
{
    QByteArray data = email.toUtf8();
    data.replace(" ","");
    return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}

QString UserSettings::generatePasswordHash(const QString &password)
{
    const QByteArray &&salt = randomSalt();
    const int zzz = QTime::currentTime().msec(); // Let's add more fun
    return hash(password.toUtf8(), salt, rounds + zzz).toHex();
}

bool UserSettings::checkPasswordHash(const QString &password) const
{
    const QByteArray &&passwordHash = get("password_hash").toUtf8();

    if (passwordHash.size() < (256/8)*2*2 + 2)
        return false;

    const int rounds        = QByteArray::fromHex(passwordHash.mid(128)).toInt();
    const QByteArray &&salt = QByteArray::fromHex(passwordHash.mid(64, 64));
    const QByteArray &&test = hash(password.toUtf8(), salt, rounds).toHex();

    return slowEquals(test, passwordHash);
}

QByteArray UserSettings::pbkdf2(const QCryptographicHash::Algorithm method,
                                const QByteArray &password,
                                const QByteArray &salt,
                                const int rounds,
                                const int keyLength)
{
    // https://codereview.qt-project.org/#/c/53976/

    QByteArray key;

    if (rounds < 1 || keyLength < 1)
        return key;

    if (salt.size() == 0 || salt.size() > std::numeric_limits<int>::max() - 4)
        return key;

    QByteArray asalt = salt;
    asalt.resize(salt.size() + 4);

    for (int count = 1, remainingBytes = keyLength; remainingBytes > 0; ++count) {
        asalt[salt.size() + 0] = static_cast<char>((count >> 24) & 0xff);
        asalt[salt.size() + 1] = static_cast<char>((count >> 16) & 0xff);
        asalt[salt.size() + 2] = static_cast<char>((count >> 8) & 0xff);
        asalt[salt.size() + 3] = static_cast<char>(count & 0xff);
        QByteArray d1 = QMessageAuthenticationCode::hash(asalt, password, method);
        QByteArray obuf = d1;

        for (int i = 1; i < rounds; ++i) {
            d1 = QMessageAuthenticationCode::hash(d1, password, method);
            for (int j = 0; j < obuf.size(); ++j)
                obuf[j] = obuf[j] ^ d1[j];
        }

        key = key.append(obuf);
        remainingBytes -= obuf.size();

        d1.fill('\0');
        obuf.fill('\0');
    }
    asalt.fill('\0');

    return key.mid(0, keyLength);
}

bool UserSettings::slowEquals(const QByteArray &a, const QByteArray &b)
{
    // https://crackstation.net/hashing-security.htm

    const int len = qMin(a.size(), b.size());
    int diff = a.size() ^ b.size();
    for (int i = 0; i < len; ++i) {
        diff |= a[i] ^ b[i];
    }
    return (diff == 0);
}

QByteArray UserSettings::randomSalt()
{
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_int_distribution<int> distribution(0, 255);

    QByteArray out(32, '?');
    for (int k = 0; k < out.size(); ++k) {
        out[k] = static_cast<unsigned char> (distribution(generator));
    }
    return out;
}

QByteArray UserSettings::hash(const QByteArray &password,
                              const QByteArray &salt,
                              const int rounds)
{
    static const QCryptographicHash::Algorithm method = QCryptographicHash::Sha256;
    return  pbkdf2(method, password, salt, rounds, salt.size()) +
            salt +
            QByteArray::number(rounds);
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
        if (us.checkPasswordHash(password)) {
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

