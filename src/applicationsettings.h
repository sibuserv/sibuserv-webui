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

#pragma once

#include "options.h"

class ApplicationSettings
{
public:
    static ApplicationSettings &instance();
    void update(const Options &options);
    void finalize();

    QString prefixString() const;
    QString configDirectory() const;
    QString l10nDirectory() const;
    QString cacheDirectory() const;
    QString logDirectory() const;
    QString sessionsDirectory() const;
    QString buildServerBinDir() const;
    QString buildServerLogFile() const;
    QString staticCodeAnalysisLogsSubdir() const;

protected:
    inline void clear();
    inline QString get(const QString &key) const;

private:
    ApplicationSettings();
    ApplicationSettings(const ApplicationSettings &in) = delete;
    ApplicationSettings(ApplicationSettings &&in) = delete;
    ApplicationSettings& operator=(const ApplicationSettings &in) = delete;
    virtual ~ApplicationSettings();

    struct ApplicationSettingsPrivate;
    ApplicationSettingsPrivate * const d;
};

