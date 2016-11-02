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

#include <QStringList>

#include "applicationsettings.h"
#include "resourcemanager.h"
#include "sessionsmanager.h"
#include "commonsettings.h"
#include "usersettings.h"
#include "localization.h"
#include "htmlpage.h"

struct HtmlPage::HtmlPagePrivate
{
    QByteArray page;
    QByteArray head;
    QByteArray body;
    QByteArray title;
    QByteArray content;
    QByteArray redirect;
    QByteArray prefix;

    CommonSettings commonSettings;
    UserSettings   userSettings;
    Localization   localization;
};

HtmlPage::HtmlPage(const Request &request) :
    d(new HtmlPagePrivate)
{
    d->prefix = ApplicationSettings::instance().prefixString().toUtf8();
    d->redirect = request.scriptName().toUtf8();

    const ResourceManager res;
    if (!request.get("ajax").isEmpty())
        d->page = "%body%";
    else
        d->page = res.read("/html/page-template.html");

    setTitle("%basic_title%");
    setHead(res.read("/html/head-template.html"));
    setBody(res.read("/html/body-template.html"));
    setContent(res.read("/html/404-template.html"));

    checkAutorization(request);

    d->localization.setFileName(d->commonSettings.l10nFile());
    d->localization.readSettings();

    setContentType("text/html");
    update();
}

HtmlPage::~HtmlPage()
{
    delete d;
}

void HtmlPage::setHead(const QByteArray &head)
{
    d->head = head;
}

void HtmlPage::setBody(const QByteArray &body)
{
    d->body = body;
}

void HtmlPage::setTitle(const QByteArray &title)
{
    d->title = title;
}

void HtmlPage::setContent(const QByteArray &content)
{
    d->content = content;
}

void HtmlPage::addToHead(const QByteArray &head)
{
    setHead(d->head + "\n" + head);
}

void HtmlPage::addToBody(const QByteArray &body)
{
    setBody(d->body + "\n" + body);
}

void HtmlPage::addToTitle(const QByteArray &title)
{
    setTitle(d->title + title);
}

void HtmlPage::addToContent(const QByteArray &content)
{
    setContent(d->content + "\n" + content);
}

void HtmlPage::checkAutorization(const Request &request)
{
    QByteArray out;
    out += "        <script>\n";

    if (SessionsManager(request).isAutorized()) {
        d->userSettings.setFileName("users/" +
                                    request.cookie("user_name") +
                                    ".json");
        d->userSettings.readSettings();

        out += "            sign_in.remove();\n";

        if (!d->userSettings.getBool("admin")) {
            out += "            settings_page.remove();\n";
        }
    }
    else {
        out += "            user_name.remove();\n";
        out += "            sign_out.remove();\n";
        out += "            settings_page.remove();\n";
    }

    out += "        </script>\n";
    addToBody(out);
}

void HtmlPage::update()
{
    QByteArray out = d->page;

    out.replace("%head%",       d->head);
    out.replace("%body%",       d->body);
    out.replace("%title%",      d->title);
    out.replace("%content%",    d->content);
    out.replace("%page_style%", d->userSettings.get("page_style").toUtf8());
    out.replace("%user_email%", d->userSettings.get("user_email").toUtf8());
    const QStringList names = {
        d->userSettings.get("desirable_user_name"),
        d->userSettings.get("full_user_name"),
        d->userSettings.get("user_name")
    };
    for (const QString &name : names) {
        if (!name.isEmpty()) {
            out.replace("%user_name%", name.toUtf8());
        }
    }
    for (const auto &key : d->localization.keys()) {
        out.replace("%" + key.toUtf8() + "%",
                    d->localization.get(key).toUtf8());
    }
    out.replace("%redirect%",   d->redirect);
    out.replace("%prefix%",     d->prefix);

    setData(out);
}

