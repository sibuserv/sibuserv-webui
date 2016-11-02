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
#include <QStringList>

#include "applicationsettings.h"
#include "projectspage.h"

ProjectsPage::ProjectsPage(const Request &request) :
    HtmlPage(request)
{
    if (isAutorizedUser()) {
        // TODO: plorerly check that user have access to project info.
        if (true) { // Debug mode!
            generateContent();
        }
        else if (getBool("logged_in_user_may_view_the_full_list_of_projects")) {
            generateContent();
        }
        else {
            forbidAccess();
        }
    }
    else if (getBool("anonymous_may_view_the_list_of_projects")) {
        generateContent();
    }
    else {
        forbidAccess();
    }

    addToTitle(" - %projects%");
    update();
    show();
}

void ProjectsPage::generateContent()
{
    // Debug mode!
    QByteArray out;
    out += "<ul>\n";

    QByteArray tmp;
    QDir dir(ApplicationSettings::instance().buildServerBinDir());
    for (const auto &it : dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
        tmp = it.toUtf8();
        out +=  "<li><a id=\"" + tmp + "\" href=\"%prefix%projects/" +
                tmp + "\">" + tmp + "</a></li>\n";
    }

    out += "</ul>";
    setContent(out);
}

