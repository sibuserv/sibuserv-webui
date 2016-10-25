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

#include "options.h"

struct Options::OptionsPrivate
{
    bool helpRequest = false;
    bool versionRequest = false;
    QString confFile;
};

Options::Options(int argc, char **argv) :
    d(new OptionsPrivate)
{
    QString arg;
    for (int idx = 1 ; idx < argc ; ++idx) {
        arg = argv[idx];
        if (arg == "-h" || arg == "--help") {
            d->helpRequest = true;
        }
        else if (arg == "-v" || arg == "--version") {
            d->versionRequest = true;
        }
        else if (arg == "-c" || arg == "--conf-file") {
            if (idx < argc - 1) {
                d->confFile = argv[idx + 1];
            }
        }
    }
}

Options::Options(const Options &in) :
    d(new OptionsPrivate(*in.d))
{
    ;
}

Options& Options::operator=(const Options &in)
{
    *d = *in.d;
    return *this;
}

Options::~Options()
{
    delete d;
}

bool Options::isHelpRequest() const
{
    return d->helpRequest;
}

bool Options::isVersionRequest() const
{
    return d->versionRequest;
}

bool Options::isConfigFileDefine() const
{
    return d->confFile.isEmpty();
}

QString Options::configFile() const
{
    if (!isConfigFileDefine())
        return "";

    return d->confFile;
}
