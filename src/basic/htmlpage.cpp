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

#include <QMap>
#include <QStringList>
#include <QByteArrayList>

#include "applicationsettings.h"
#include "resourcemanager.h"
#include "sessionsmanager.h"
#include "localization.h"
#include "htmlpage.h"

struct HtmlPage::HtmlPagePrivate
{
    bool admin = false;
    bool autorized = false;

    QByteArray page;
    QByteArray head;
    QByteArray body;
    QByteArray title;
    QByteArray content;

    CommonSettings commonSettings;
    UserSettings   userSettings;

    QMap<QByteArray, QByteArray> extraReplacements;
};

HtmlPage::HtmlPage(const Request &request) :
    d(new HtmlPagePrivate)
{
    if (request.get("ajax").isEmpty()) {
        const ResourceManager res;
        setPage(res.read("/html/page-template.html"));
        setTitle("%basic_title%");
        setHead(res.read("/html/head-template.html"));
        setBody(res.read("/html/body-template.html"));
        setContent(res.read("/html/404-template.html"));
        setContentType("text/html");
        addExtraReplacements("script_name", request.scriptName().toUtf8());
    }

    checkAutorization(request);

    if (request.get("ajax") == "sign_out") {
        if (isAutorizedUser()) {
            SessionsManager::closeSession(d->userSettings.get("user_id"));
        }
    }
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

void HtmlPage::addExtraReplacements(const QByteArray &from,
                                    const QByteArray &to)
{
    if (from.isEmpty())
        return;

    d->extraReplacements[from] = to;
}

void HtmlPage::addStyleSheetToHead(const QByteArray &styleSheet)
{
    if (d->head.isEmpty()) {
        setHead(styleSheet);
        return;
    }

    if (!d->head.contains("link rel=\"stylesheet\"")) {
        addToHead(styleSheet);
        return;
    }

    const QByteArray &&html =
            "        <link rel=\"stylesheet\" type=\"text/css\" href=\"" +
            styleSheet + "\">";

    QByteArrayList list = d->head.split('\n');
    for (int k = list.size()-1; k > 0; --k) {
        if (list[k].contains("link rel=\"stylesheet\"")) {
            list.insert(k+1, html);
        }
    }
    setHead(list.join('\n'));
}

void HtmlPage::addScriptToHead(const QByteArray &script)
{
    addToHead("        <script src=\"" + script + "\"></script>");
}

bool HtmlPage::isAutorizedUser() const
{
    return d->autorized;
}

bool HtmlPage::isAdmin() const
{
    return d->admin;
}

void HtmlPage::setPage(const QByteArray &page)
{
    d->page = page;
}

void HtmlPage::checkAutorization(const Request &request)
{
    QByteArray out;
    out += "        <script>\n";

    SessionsManager sm(request);
    if (sm.isAutorized()) {
        d->autorized = true;
        d->userSettings.setFileName("users/" + sm.get("user_name") + ".json");
        d->userSettings.readSettings();
    }
    else if (d->userSettings.isValidAutorizationRequest(request)) {
        d->autorized = true;
        sm.setFileName(d->userSettings.get("user_id") + ".json");
        if (sm.beginNewSession(d->userSettings.get("user_name"))) {
            const bool httpsOnly = d->commonSettings.getBool("https_only");
            setCookie("user_id", d->userSettings.get("user_id"),
                      "", "/", "", httpsOnly, true);
            setCookie("session_id", sm.get("session_id"),
                      "", "/", "", httpsOnly, true);
        }
    }
    else {
        if (request.isPost()) {
            if (!request.post().isEmpty()) {
                if (!request.post("user_name").isEmpty() &&
                        !request.post("password").isEmpty()) {
                    out += "            show_auth_error();\n";
                    out += "            begin_authorization();\n";
                }
            }
        }
        else if (!request.cookie("user_id").isEmpty() &&
                 !request.cookie("session_id").isEmpty()) {
            setCookie("user_id", "", "Thu, 01 Jan 1970 00:00:00 UTC", "/");
            setCookie("session_id", "", "Thu, 01 Jan 1970 00:00:00 UTC", "/");
        }
    }
    if (d->autorized) {
        out += "            authorized_user();\n";
        d->admin = d->userSettings.getBool("admin");
        if (d->admin) {
            out += "            admin();\n";
        }
        else {
            out += "            not_admin();\n";
        }
        if (d->userSettings.get("user_email").isEmpty()) {
            out += "            email_is_unknown();\n";
        }
        if (d->userSettings.getBool("force_password_update")) {
            if (sm.get("authorization_type") == "local") {
                out += "            force_password_update();\n";
            }
        }
    }
    else {
        addStyleSheetToHead("%prefix%css/auth/%page_style%");
        out += "            unauthorized_user();\n";
    }

    out += "        </script>";
    addToBody(out);
}

void HtmlPage::forceAuthorization()
{
    if (!d->autorized) {
        addToBody("        <script>begin_authorization()</script>");
    }
}

void HtmlPage::forbidAccess()
{
    const ResourceManager res;
    setContent(res.read("/html/403-template.html"));
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
    out.replace("%gravatar_icon_url%", d->userSettings.gravatarIconUrl());

    if (!d->autorized) {
        const ResourceManager res;
        out.replace("%auth_form%", res.read("/html/auth-template.html"));
    }
    else {
        out.replace("%auth_form%", "");
    }
    const QStringList names = {
        d->userSettings.get("desirable_user_name"),
        d->userSettings.get("full_user_name"),
        d->userSettings.get("user_name")
    };
    for (const QString &name : names) {
        if (!name.isEmpty()) {
            out.replace("%user_name%", name.toUtf8());
            break;
        }
    }

    Localization localization(d->commonSettings.get("l10n_file"));
    if (!d->userSettings.get("l10n_file").isEmpty()) {
        localization.setFileName(d->userSettings.get("l10n_file"));
        localization.readSettings();
    }
    for (const auto &key : localization.keys()) {
        out.replace("%" + key.toUtf8() + "%",
                    localization.get(key).toUtf8());
    }
    for (const auto &key : d->extraReplacements.keys()) {
        out.replace("%" + key + "%", d->extraReplacements[key]);
    }

    out.replace("%jquery_url%", APP_S().jqueryUrl().toUtf8());
    out.replace("%prefix%",     APP_S().prefixString().toUtf8());

    setData(out);
}

CommonSettings& HtmlPage::commonSettings() const
{
    return d->commonSettings;
}

UserSettings &HtmlPage::userSettings() const
{
    return d->userSettings;
}

QString HtmlPage::get(const QString &key) const
{
    return d->commonSettings.get(key);
}

bool HtmlPage::getBool(const QString &key) const
{
    return d->commonSettings.getBool(key);
}

