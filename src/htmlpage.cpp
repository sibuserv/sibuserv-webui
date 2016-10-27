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

#include "applicationsettings.h"
#include "resourcemanager.h"
#include "htmlpage.h"

struct HtmlPage::HtmlPagePrivate
{
    QByteArray page      = "%body%";
    QByteArray head      = "";
    QByteArray body      = "%content%";
    QByteArray content   = "<h1>404</h1>\n"
                           "This is not the web page you are looking for.";
    QByteArray pageStyle = "%prefix%/css/default.css";
    QByteArray script    = "%prefix%/js/script.js";
    QByteArray author    = "Boris Pek";
    QByteArray year      = "2016";
    QByteArray title     = "Sibuserv Web UI";
    QByteArray redirect  = "%prefix%/projects";
    QByteArray prefix    = "/sibuserv/";
};

HtmlPage::HtmlPage(const Request &request) :
    d(new HtmlPagePrivate)
{
    const ResourceManager res;
    if (request.get("ajax").isEmpty()) {
        d->page = res.read("/page-template.html");
    }
    d->head = res.read("/head-template.html");
    d->body = res.read("/body-template.html");
    d->prefix = ApplicationSettings::instance().prefixString().toUtf8();
    d->redirect = request.scriptName().toUtf8();
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
    update();
}

void HtmlPage::setBody(const QByteArray &body)
{
    d->body = body;
    update();
}

void HtmlPage::setContent(const QByteArray &content)
{
    d->content = content;
    update();
}

void HtmlPage::update()
{
    QByteArray out = d->page;
    out.replace("%head%",       d->head);
    out.replace("%body%",       d->body);
    out.replace("%content%",    d->content);
    out.replace("%page_style%", d->pageStyle);
    out.replace("%script%",     d->script);
    out.replace("%author%",     d->author);
    out.replace("%year%",       d->year);
    out.replace("%title%",      d->title);
    out.replace("%author%",     d->author);
    out.replace("%redirect%",   d->redirect);
    out.replace("%prefix%",     d->prefix);
    setData(out);
}

