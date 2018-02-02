/*****************************************************************************
 *                                                                           *
 *  Copyright (c) 2016-2018 Boris Pek <tehnick-8@yandex.ru>                  *
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

#include <QJsonObject>
#include <QJsonDocument>

#include "resourcemanager.h"
#include "profilesettingssecuritypage.h"

ProfileSettingsSecurityPage::ProfileSettingsSecurityPage(const Request &request) :
    HtmlPage(request)
{
    if (request.get("ajax").isEmpty()) {
        addToTitle(" - %profile_settings% - %security%");
        generateHtmlTemplate(request);
        update();
    }
    else {
        generateAjaxResponse(request);
    }
    show();
}

void ProfileSettingsSecurityPage::generateHtmlTemplate(const Request &request)
{
    if (isAutorizedUser()) {
        const ResourceManager res;
        addScriptToHead("%prefix%js/password-update.js");
        addStyleSheetToHead("%prefix%css/password-update/%page_style%");
        setContent(res.read("/html/password-update-template.html"));
    }
    else {
        forbidAccess();
        forceAuthorization();
    }
}

void ProfileSettingsSecurityPage::generateAjaxResponse(const Request &request)
{
    if (!isAutorizedUser())
        return;
    if (!request.isPost())
        return;
    if (!request.receivedJsonObject())
        return;

    const QJsonObject &&in = request.getJsonObject();
    const QString &&currPass = in["current_password"].toString();
    const QString &&newPass  = in["new_password"].toString();
    const QString &&confPass = in["confirm_password"].toString();

    if (currPass.isEmpty() || newPass.isEmpty() || confPass.isEmpty())
        return;

    QJsonObject out;
    if (confPass != newPass) {
        out["status"] = "mismatch_passwords";
    }
    else if (newPass == currPass) {
        out["status"] = "identical_passwords";
    }
    else if (!userSettings().checkPasswordHash(currPass)) {
        out["status"] = "wrong_password";
    }
    else {
        const QString &&newPassHash = UserSettings::generatePasswordHash(newPass);
        if (newPassHash.isEmpty()) {
            out["status"] = "failed_to_update";
        }
        else {
            userSettings().set("password_hash", newPassHash);
            userSettings().setBool("force_password_update", false);
            userSettings().syncSettings();

            if (userSettings().writeSettings()) {
                out["status"] = "success";
            }
            else {
                out["status"] = "failed_to_save";
            }
        }
    }
    setData(QJsonDocument(out).toJson());
}

