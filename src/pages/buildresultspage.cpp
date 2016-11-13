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
#include "buildresultsitem.h"
#include "buildresultspage.h"

BuildResultsPage::BuildResultsPage(const Request &request,
                                   const QString &projectName,
                                   const QString &version) :
    HtmlPage(request)
{
    if (request.get("ajax").isEmpty()) {
        addToTitle(" - %projects% - %current_project_name% - %current_build%");
        generateHtmlTemplate(projectName, version);
        addExtraReplacements("current_project_name", projectName.toUtf8());
        addExtraReplacements("current_build", version.toUtf8());
        update();
        show();
    }
    else {
        generateAjaxResponse(request, projectName, version);
        show();
    }
}

void BuildResultsPage::generateHtmlTemplate(const QString &projectName,
                                            const QString &version)
{
    const ResourceManager res;
    if (isAutorizedUser()) {
        if (isAllowedAccess(projectName)) {
            if (!allTargets(projectName, version).isEmpty()) {
                addScriptToHead("%prefix%js/build-results.js");
                addStyleSheetToHead("%prefix%css/build-results/%page_style%");
                setContent(res.read("/html/build-results-template.html"));
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

void BuildResultsPage::generateAjaxResponse(const Request &request,
                                            const QString &projectName,
                                            const QString &version)
{
    if (!isAutorizedUser() || !isAllowedAccess(projectName))
        return;

    if (projectName.isEmpty() || version.isEmpty())
        return;

    auto &&targets = allTargets(projectName, version);

    if (targets.isEmpty())
        return;

    sortTargets(targets);

    if (request.get("ajax") == "results_list") {
        const int &&fullSize = targets.size();
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
            tmp = BuildResultsItem(projectName, version, targets[k]).getJsonObject();
            if (k == fullSize -1) {
                tmp["last_item"] = true;
            }
            out += tmp;
        }
        setData(QJsonDocument(out).toJson());
    }
    else if (request.get("ajax") == "target") {
        QJsonObject out;
        if (!request.post("pos").isEmpty()) {
            const QString &&target = request.post("target");
            if (targets.contains(version)) {
                out = BuildResultsItem(projectName, version, target).getJsonObject();
            }
        }
        setData(QJsonDocument(out).toJson());
    }
}

bool BuildResultsPage::isAllowedAccess(const QString &projectName) const
{
    if (isAdmin())
        return true;

    const UserSettings &us = userSettings();
    const QJsonObject &&obj = us.getObject("projects");
    return obj.contains(projectName);
}

QStringList BuildResultsPage::allTargets(const QString &projectName,
                                         const QString &version) const
{
    const QDir dir(APP_S().buildServerBinDir() + "/" + projectName + "/" + version);
    return dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::NoSort);
}

void BuildResultsPage::sortTargets(QStringList &targets) const
{
    QCollator coll;
    coll.setNumericMode(true);
    const auto comp = [&coll](const QString &s1, const QString &s2) -> bool {
        return coll.compare(s2, s1) < 0;
    };
    std::sort(targets.begin(), targets.end(), comp);
}

