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

#include <iostream>

#include "Version.h"
#include "help.h"

using std::string;
using std::cout;

void Help::showHelp() const
{
    cout << "Usage: sibuserv-webui [options]\n"
            "\n"
            "FastCGI program which passively listens for incoming connections and\n"
            "generates list of hosts in your IPv6 network. This daemon prepares data\n"
            "which may be put directly into /etc/hosts.\n"
            "\n"
            "Generic options:\n"
            "  -h, --help      show help and exit\n"
            "  -v, --version   show version and exit\n"
            "\n"
            "Special options:\n"
            "  -a, --add-user  add new local user\n"
            "                  His(her) json config will be stored in\n"
            "                  <config directory>/users/ directory.\n"
            "  -d, --del-user  delete local user\n"
            "                  His(her) json config will be removed from\n"
            "                  <config directory>/users/ directory.\n"
            "  -s, --set-pass  set new password for specified user\n"
            "                  Have in mind that user will not be notified about this\n"
            "                  automatically. You will need to do it yourself.\n"
            "  -c <file>, --conf-file <file>  set config file\n"
            "                  (default: /etc/sibuserv/sibuserv-webui.conf)\n"
            "\n"
            "Settings in config file have lower priority than command line options.\n";
}

void Help::showVersion() const
{
    cout << PROJECT_NAME << " version: " << PROJECT_VERSION << "\n";
}
