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

#include <QDir>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCollator>

#include "logmanager.h"
#include "resourcemanager.h"
#include "usersettings.h"
#include "applicationsettings.h"
#include "buildhistoryitem.h"
#include "buildhistorypage.h"

BuildHistoryPage::BuildHistoryPage(const Request &request, const QString &projectName) :
    HtmlPage(request)
{
    if (request.get("ajax").isEmpty()) {
        addToTitle(" - %projects% - %current_project_name%");
        generateHtmlTemplate(projectName);
        addExtraReplacements("current_project_name", projectName.toUtf8());
        update();
    }
    else {
        generateAjaxResponse(request, projectName);
    }
    show();
}

void BuildHistoryPage::generateHtmlTemplate(const QString &projectName)
{
    const ResourceManager res;
    if (isAutorizedUser()) {
        if (isAllowedAccess(projectName)) {
            if (!allBuilds(projectName).isEmpty()) {
                addScriptToHead("%prefix%js/build-history.js");
                addStyleSheetToHead("%prefix%css/build-history/%page_style%");
                setContent(res.read("/html/build-history-template.html"));
            }
            else {
                setContent(res.read("/html/no-build-history-template.html"));
            }
        }
        else {
            forbidAccess();
        }
    }
    else {
        forbidAccess();
        forceAuthorization();
    }
}

void BuildHistoryPage::generateAjaxResponse(const Request &request,
                                            const QString &projectName)
{
    if (!isAutorizedUser() || !isAllowedAccess(projectName))
        return;

    if (projectName.isEmpty())
        return;

    auto &&builds = allBuilds(projectName);

    if (builds.isEmpty())
        return;

    sortBuilds(builds);

    if (request.get("ajax") == "builds_list") {
        const int &&fullSize = builds.size();
        int first = 0;
        int end = fullSize;

        if (!request.post("pos").isEmpty() && !request.post("len").isEmpty()) {
            const int pos = request.post("pos").toInt();
            const int len = request.post("len").toInt();
            if (pos >= 0 && len > 0) {
                first = qMin(pos, fullSize);
                end   = qMin(pos + len, fullSize);
            }
        }

        QJsonArray out;
        QJsonObject tmp;
        for (int k = first; k < end; ++k) {
            tmp = BuildHistoryItem(projectName, builds[k]).getJsonObject();
            if (k == fullSize -1) {
                tmp["last_item"] = true;
            }
            out += tmp;
        }
        setData(QJsonDocument(out).toJson());
    }
    else if (request.get("ajax") == "build") {
        QJsonObject out;
        if (!request.post("pos").isEmpty()) {
            const QString &&version = request.post("version");
            if (builds.contains(version)) {
                out = BuildHistoryItem(projectName, version).getJsonObject();
            }
        }
        setData(QJsonDocument(out).toJson());
    }
}

bool BuildHistoryPage::isAllowedAccess(const QString &projectName) const
{
    if (isAdmin())
        return true;

    const UserSettings &us = userSettings();
    const QJsonObject &&obj = us.getObject("projects");
    return obj.contains(projectName);
}

QStringList BuildHistoryPage::allBuilds(const QString &projectName) const
{
    const QDir dir(APP_S().buildServerBinDir() + "/" + projectName);
    return dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::NoSort);
}

void BuildHistoryPage::sortBuilds(QStringList &builds) const
{
    QCollator coll;
    coll.setNumericMode(true);
    const auto comp = [&coll](const QString &s1, const QString &s2) -> bool {
        return coll.compare(s2, s1) < 0;
    };
    std::sort(builds.begin(), builds.end(), comp);
}

