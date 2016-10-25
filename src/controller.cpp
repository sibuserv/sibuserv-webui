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

#include <QString>
#include <QByteArray>

#include "applicationsettings.h"
#include "request.h"
#include "logmanager.h"
#include "controller.h"

#include "datafile.h"
#include "projectspage.h"
#include "debugpage.h"
#include "counterpage.h"

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

        static const QString prefixString =
                ApplicationSettings::instance().prefixString();
        const QString sriptName = d->request.scriptName();

        // Pages order: most frequently viewed.
        if (sriptName == prefixString) {
            ProjectsPage(d->request);
        }
        else if (sriptName == prefixString + "projects") {
            ProjectsPage(d->request);
        }
        else if (sriptName.startsWith(prefixString + "project/")) {
            DebugPage(d->request);
        }
        else if (sriptName == prefixString + "profile/settings") {
            DebugPage(d->request);
        }
        else if (sriptName == prefixString + "settings") {
            DebugPage(d->request);
        }
        else if (sriptName == prefixString + "counter") {
            CounterPage(d->request, QByteArray::number(counter));
        }
        else {
            DataFile(d->request);
        }
    }
}

