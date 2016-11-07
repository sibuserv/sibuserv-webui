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
        generateAjaxResponse();
        show();
    }
}

void ProjectsPage::generateHtmlTemplate()
{
    const ResourceManager res;
    if (isAutorizedUser()) {
        if (getBool("anonymous_may_view_the_list_of_projects") ||
                getBool("logged_in_user_may_view_the_full_list_of_projects")) {
            setContent(res.read("/html/projects-template.html"));
        }
        else if (allowedProjectsExist()) {
            setContent(res.read("/html/projects-template.html"));
        }
        else {
            setContent(res.read("/html/no-projects-template.html"));
        }
    }
    else if (getBool("anonymous_may_view_the_list_of_projects")) {
        setContent(res.read("/html/projects-template.html"));
    }
    else {
        forbidAccess();
    }
}

void ProjectsPage::generateAjaxResponse()
{
    const QDir dir(ApplicationSettings::instance().buildServerBinDir());
    const auto &&all = dir.entryList(QDir::AllDirs |
                                     QDir::NoDotAndDotDot);
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

    QJsonArray out;

    // Debug mode!
    QJsonObject tmp;
    for (const auto &p : projects) {
        // TODO: make ProjectsTableItem() class.
        tmp = {
            {"project_name", p}
        };
        out += tmp;
    }

    setData(QJsonDocument(out).toJson());
}

bool ProjectsPage::allowedProjectsExist()
{
    QStringList &&projects = allowedProjects();
    if (projects.isEmpty())
        return false;

    QDir dir(ApplicationSettings::instance().buildServerBinDir());
    for (const auto &it : dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
        if (projects.contains(it)) {
            return true;
        }
    }

    return false;
}

QStringList ProjectsPage::allowedProjects()
{
    UserSettings &us = userSettings();
    QJsonObject &&obj = us.getObject("projects");
    return obj.keys();
}

