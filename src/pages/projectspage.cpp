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
#include <QFileInfo>
#include <QByteArray>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "applicationsettings.h"
#include "resourcemanager.h"
#include "usersettings.h"
#include "projectstableitem.h"
#include "projectspage.h"

ProjectsPage::ProjectsPage(const Request &request) :
    HtmlPage(request)
{
    if (request.get("ajax").isEmpty()) {
        addToTitle(" - %projects%");
        generateHtmlTemplate();
        update();
        show();
    }
    else {
        generateAjaxResponse(request);
        show();
    }
}

void ProjectsPage::generateHtmlTemplate()
{
    const ResourceManager res;
    if (isAutorizedUser()) {
        if (!allProjects().isEmpty() &&
                (getBool("anonymous_may_view_the_list_of_projects") ||
                 getBool("logged_in_user_may_view_the_full_list_of_projects"))) {
            addScriptToHead("%prefix%js/projects-table.js");
            addStyleSheetToHead("%prefix%css/projects/%page_style%");
            setContent(res.read("/html/projects-template.html"));
        }
        else if (allowedProjectsExist()) {
            addScriptToHead("%prefix%js/projects-table.js");
            addStyleSheetToHead("%prefix%css/projects/%page_style%");
            setContent(res.read("/html/projects-template.html"));
        }
        else {
            setContent(res.read("/html/no-projects-template.html"));
        }
    }
    else if (getBool("anonymous_may_view_the_list_of_projects")) {
        if (!allProjects().isEmpty()) {
            addScriptToHead("%prefix%js/projects-table.js");
            addStyleSheetToHead("%prefix%css/projects/%page_style%");
            setContent(res.read("/html/projects-template.html"));
        }
        else {
            setContent(res.read("/html/no-projects-template.html"));
        }
    }
    else {
        forbidAccess();
    }
}

void ProjectsPage::generateAjaxResponse(const Request &request)
{
    const auto &&all = allProjects();
    const auto &&allowed = allowedProjects();

    QStringList projects;
    if (isAutorizedUser()) {
        if (getBool("anonymous_may_view_the_list_of_projects") ||
                 getBool("logged_in_user_may_view_the_full_list_of_projects")) {
            projects = all;
        }
        else if (!allowed.isEmpty()) {
            for (const auto &p : all) {
                if (allowed.contains(p)) {
                    projects.append(p);
                }
            }
        }
    }
    else if (getBool("anonymous_may_view_the_list_of_projects")) {
        projects = all;
    }

    if (projects.isEmpty())
        return;

    if (request.get("ajax") == "projects_list") {
        const int &&fullSize = projects.size();
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
            tmp = ProjectsTableItem(projects[k]).getJsonObject();
            tmp["role"] = QString("none");
            if (allowed.contains(projects[k])) {
                tmp["role"] = userSettings().getProjectRole(projects[k]);
            }
            if (k == fullSize -1) {
                tmp["last_item"] = true;
            }
            out += tmp;
        }
        setData(QJsonDocument(out).toJson());
    }
    else if (request.get("ajax") == "project") {
        QJsonObject out;
        if (!request.post("pos").isEmpty()) {
            const QString &&projectName = request.post("project_name");
            if (projects.contains(projectName)) {
                out = ProjectsTableItem(projectName).getJsonObject();
                out["role"] = QString("none");
                if (allowed.contains(projectName)) {
                    out["role"] = userSettings().getProjectRole(projectName);
                }
            }
        }
        setData(QJsonDocument(out).toJson());
    }
}

bool ProjectsPage::allowedProjectsExist() const
{
    QStringList &&projects = allowedProjects();
    if (projects.isEmpty())
        return false;

    QDir dir(APP_S().buildServerBinDir());
    for (const auto &it : dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
        if (projects.contains(it)) {
            return true;
        }
    }

    return false;
}

QStringList ProjectsPage::allowedProjects() const
{
    const UserSettings &us = userSettings();
    const QJsonObject &&obj = us.getObject("projects");
    return obj.keys();
}

QStringList ProjectsPage::allProjects() const
{
    const QDir dir(APP_S().buildServerBinDir());
    return dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot,
                         QDir::Name | QDir::IgnoreCase);
}

