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

#include "help.h"
#include "options.h"
#include "controller.h"
#include "commandlinedialogs.h"
#include "applicationsettings.h"
#include "unpacker.h"

int main(int argc, char **argv)
{
    {
        Options options(argc, argv);
        if (options.isInfoRequest()) {
            Help help;

            if (options.isHelpRequest())
                help.showHelp();
            else if (options.isVersionRequest())
                help.showVersion();

            return 0;
        }

        ApplicationSettings::instance().update(options);
        ApplicationSettings::instance().finalize();

        if (options.isUserSettingsRequest()) {
            CommandLineDialogs dialog;

            if (options.isAddUserRequest())
                dialog.addUser();
            else if (options.isDelUserRequest())
                dialog.delUser();
            else if (options.isSetPassRequest())
                dialog.setPassword();

            return 0;
        }

        EmbeddedResourcesUnpacker();
    }

    Controller controller;
    controller.start();

    return 0;
}

