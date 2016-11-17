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

#include <termios.h>
#include <iostream>
#include <string>

#include <QFile>
#include <QFileInfo>

#include "usersettings.h"
#include "applicationsettings.h"
#include "commandlinedialogs.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;

void CommandLineDialogs::addUser() const
{
    const string processOwnedByUser = getenv("USER");
    if (processOwnedByUser.empty()) {
        cout << "Program fails to detect the owner of this process "
                "because environment variable USER is emty. Bye bye!"
             << endl;
        return;
    }
    else {
        cout << "Currently program is launched by \""
             << processOwnedByUser
             << "\" user. "
                "So all unpacked and generated files will be owned by \""
             << processOwnedByUser
             << "\". It may causes to incorrect work of program if "
                "sibuserv-webui daemon will be launched by another user.\n"
                "Do you want to continue [y/N]?"
             << endl;

        string line;
        std::getline(cin, line);
        if (line != "y" && line != "Y") {
            cout << "Bye bye!" << endl;
            return;
        }
    }

    const string allowedCharactersForUserName =
            ".,_-@0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const string allowedCharactersForUserEmail =
            ".,_-@0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const string allowedCharactersForUserId =
            ".,_-@0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string line;

    cout << "Short user name: ";
    std::getline(cin, line);
    if (line.find_first_not_of(allowedCharactersForUserName) != string::npos) {
        cout << "Incorrect user name! Only these characters are allowed:\n";
        for (const auto &c : allowedCharactersForUserName) {
            cout << c << " ";
        }
        cout << endl;
        return;
    }
    const QString userName = QString::fromStdString(line);
    const QString fileName = "users/" + userName + ".json";

    {
        UserSettings us(fileName);
        if (!us.get("user_id").isEmpty()) {
            cout << "User is already exist!" << endl;
            return;
        }
    }

    cout << "Full user name: ";
    std::getline(cin, line);
    const QString fullUserName = QString::fromStdString(line);

    cout << "User email address: ";
    std::getline(cin, line);
    if (line.find_first_not_of(allowedCharactersForUserEmail) != string::npos) {
        cout << "Incorrect user name! Only these characters are allowed:\n";
        for (const auto &c : allowedCharactersForUserEmail) {
            cout << c << " ";
        }
        cout << endl;
        return;
    }
    const QString userEmail = QString::fromStdString(line);

    cout << "Administrator? [y/N]: ";
    std::getline(cin, line);
    bool admin = false;
    if (line == "y" || line == "Y") {
        admin = true;
    }

    cout << "User ID: ";
    std::getline(cin, line);
    if (line.find_first_not_of(allowedCharactersForUserId) != string::npos) {
        cout << "Incorrect user ID! Only these characters are allowed:\n";
        for (const auto &c : allowedCharactersForUserId) {
            cout << c << " ";
        }
        cout << endl;
        return;
    }
    const QString userId = QString::fromStdString(line);

    cout << "Password: ";
    const QString password = getPassword();
    if (password.isEmpty()) {
        cout << "Password is empty!" << endl;
        return;
    }
    cout << "Confirm password: ";
    const QString confirmPassword = getPassword();
    if (confirmPassword != password) {
        cout << "Passwords do not match!" << endl;
        return;
    }
    const QString passwordHash = UserSettings::generatePasswordHash(password);

    UserSettings us;
    us.setFileName(fileName);
    QFile f(APP_S().configDirectory() + "/" + fileName);

    us.set("user_name",      userName);
    us.set("full_user_name", fullUserName);
    us.set("user_email",     userEmail);
    us.set("user_id",        userId);
    us.set("password_hash",  passwordHash);
    us.setBool("admin",      admin);

    if (us.writeSettings()) {
        cout << "User config is successfully written to:\n"
             << QFileInfo(f).absoluteFilePath().toStdString()
             << endl;
    }
    else {
        if (f.open(QIODevice::OpenModeFlag::WriteOnly)) {
            f.close();
            cout << "WTF!?" << endl;
            return;
        }
        else {
            cout << "Failed to save user config:\n"
                 << f.errorString().toStdString()
                 << endl;
            return;
        }
    }


}

void CommandLineDialogs::delUser() const
{
    string line;
    cout << "User name: ";
    std::getline(cin, line);
    const QString userName = QString::fromStdString(line);
    const QString fileName = "users/" + userName + ".json";

    UserSettings us(fileName);
    if (us.get("user_id").isEmpty()) {
        cout << "User not found!" << endl;
        return;
    }

    const QString fullUserName = us.get("full_user_name");
    if (!fullUserName.isEmpty()) {
        cout << "Are you sure that you want to delete user \""
             << fullUserName.toStdString()
             << "\"? [y/N]"
             << endl;
    }
    else {
        cout << "Are you sure that you want to delete user \""
             << userName.toStdString()
             << "\"? [y/N]"
             << endl;
    }

    std::getline(cin, line);
    if (line != "y" && line != "Y") {
        cout << "Bye bye!" << endl;
        return;
    }

    QFile f(APP_S().configDirectory() + "/" + fileName);
    if (f.remove()) {
        cout << "User config \""
             << QFileInfo(f).absoluteFilePath().toStdString()
             << "\" is successfully removed!"
             << endl;
        return;
    }
    else {
        cout << "Failed to remove user config:\n"
             << f.errorString().toStdString()
             << endl;
        return;
    }
}

QString CommandLineDialogs::getPassword() const
{
    struct termios _old, _new;

    // Turn echoing off and fail if we can’t.
    if (tcgetattr(fileno(stdin), &_old) != 0)
        return "";

    _new = _old;
    _new.c_lflag &= ~ECHO;
    if (tcsetattr(fileno(stdin), TCSAFLUSH, &_new) != 0)
        return "";

    // Read the password.
    string line;
    std::getline(cin, line);
    cout << endl;

    // Restore terminal.
    tcsetattr(fileno(stdin), TCSAFLUSH, &_old);

    return QString::fromStdString(line);
}

