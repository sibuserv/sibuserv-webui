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

#include <QString>
#include <QByteArray>

#include "applicationsettings.h"
#include "request.h"
#include "logmanager.h"
#include "datafile.h"
#include "datafilewithlimitedaccess.h"
#include "javascriptredirect.h"
#include "projectspage.h"
#include "buildhistorypage.h"
#include "buildresultspage.h"
#include "profilesettingspage.h"
#include "profilesettingssecuritypage.h"
#include "settingspage.h"
#include "helppage.h"
#include "debugpage.h"
#include "counterpage.h"
#include "controller.h"

struct Controller::ControllerPrivate
{
    Request request;
};

Controller::Controller() :
    d(new ControllerPrivate)
{
    ;
}

Controller::~Controller()
{
    delete d;
}

void Controller::start()
{
    unsigned long long counter = 0;
    while (d->request.next()) {
        ++counter;

        if (!d->request.isGet() && !d->request.isPost()) {
            continue;
        };

        const QString &&prefixString = APP_S().prefixString();

        QString pageName = d->request.scriptName();
        if (pageName.size() >= prefixString.size()) {
            pageName.remove(0, prefixString.size());
        }
        if (pageName.endsWith("/")) {
            pageName.remove(pageName.size()-1, 1);
        }

        // Pages order: the most frequently viewed.
        if (pageName.isEmpty()) {
            ProjectsPage(d->request);
        }
        else if (pageName == "projects") {
            ProjectsPage(d->request);
        }
        else if (pageName.startsWith("projects/")) {
            const int count = pageName.count("/");
            if (count == 1) {
                const int idx = pageName.lastIndexOf("/");
                const QString &&projectName = pageName.mid(idx + 1);
                BuildHistoryPage(d->request, projectName);
            }
            else if (count == 2) {
                const int m = pageName.indexOf("/", 0);
                const int n = pageName.indexOf("/", m + 1);
                const QString &&projectName = pageName.mid(m + 1, n - m - 1);
                const QString &&version = pageName.mid(n + 1);
                BuildResultsPage(d->request, projectName, version);
            }
            else if (count > 2) {
                const int m = pageName.indexOf("/", 0);
                const int n = pageName.indexOf("/", m + 1);
                const QString &&projectName = pageName.mid(m + 1, n - m - 1);
                const QString &&fileName = pageName.mid(m + 1);

                if (d->request.scriptName().endsWith("cppcheck.html")) {
                    JavaScriptRedirect(d->request,
                                       d->request.scriptName() + "/");
                }
                else if (d->request.scriptName().endsWith("cppcheck.html/")) {
                    DataFileWithLimitedAccess(d->request, projectName,
                                              fileName + "/index.html");
                }
                else {
                    DataFileWithLimitedAccess(d->request, projectName,
                                              fileName);
                }
            }
        }
        else if (pageName == "profile/settings") {
            ProfileSettingsPage(d->request);
        }
        else if (pageName == "profile") {
            JavaScriptRedirect(d->request, prefixString + "profile/settings");
        }
        else if (pageName == "profile/settings/security") {
            ProfileSettingsSecurityPage(d->request);
        }
        else if (pageName == "settings") {
            SettingsPage(d->request);
        }
        else if (pageName == "settings/localization") {
            DebugPage(d->request);
        }
        else if (pageName == "settings/users") {
            DebugPage(d->request);
        }
        else if (pageName.startsWith("settings/users/")) {
            DebugPage(d->request);
        }
        else if (pageName == "help") {
            HelpPage(d->request);
        }
        else if (pageName == "counter") {
            CounterPage(d->request, QByteArray::number(counter));
        }
        else if (pageName == "debug") {
            DebugPage(d->request);
        }
        else {
            DataFile(d->request);
        }
    }
}

