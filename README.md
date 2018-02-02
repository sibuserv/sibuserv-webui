## Sibuserv Web UI – Simple Build Server Web User Interface

### Copyright

2016-2018 Boris Pek \<tehnick-8@yandex.ru\>

### License

MIT (Expat)

### Homepage

https://github.com/sibuserv/sibuserv-webui

### Description

This project provides Web UI for [Sibuserv](https://github.com/sibuserv/sibuserv) build server. Together they form  typical continuous integration system.

Sibuserv Web UI supported features:

* Visual indication of state of projects.
* Convenient navigation: projects list, built versions, results of builds, etc..
* Simple access to build results: assembled programs, logs of builds, logs of static code analyser, etc..
* Flexible management of user access permissions in projects for authorized and unauthorized users.
* Support of website localization
* ...

Sibuserv CI supported features, unsupported features and future plans:

Task        | Status | Priority | Comment
----------- | ------ | -------- | -------
Trigger     | ✔ / ✘  | low      | Daemon regularly (with a fixed period of time) checks changes of project. Triggers for testing of merge requests are not supported yet.
Update      | ✔      | hight    | Currently supported version control systems: git, subversion and mercurial repositories (with native tools or with help of git-svn and git-remote-hg)
Analyse     | ✔      | hight    | Only `cppcheck` static code analyzer is supported for now. Clang Static Analyzer and PVS-Studio are in plans.
Build       | ✔      | hight    | Supported target systems: GNU/Linux, MS Windows, Android. Supported projects: qmake-, cmake- and autotools-based. 
UnitTest    | ✘      | medium   | In plans.
Deploy      | ✘      | none     | Usefulness of this function is questionable in our case.
Test        | ✘      | low      | In far plans.
Archive     | ✔      | hight    | User may download built binaries or *.zip archives (if project installs a number of files) by one mouse click at page with results of build.
Report      | ✘      | medium   | In plans.

For information how to install and how to use Sibuserv Web UI see files [INSTALL](./INSTALL) and [HOWTO](./HOWTO).

### Screenshots

<a href="https://tehnick.github.io/sibuserv/screenshots/03.png" title="Example of page with projects list (authorized user)">
    <img src="https://tehnick.github.io/sibuserv/screenshots/03.png" width="99%">
</a>

<a href="https://tehnick.github.io/sibuserv/screenshots/10.png" title="Example of page with projects list (unauthorized user)">
    <img src="https://tehnick.github.io/sibuserv/screenshots/10.png" width="49.5%">
</a>
<a href="https://tehnick.github.io/sibuserv/screenshots/11.png" title="Example of page with a list of available versions">
    <img src="https://tehnick.github.io/sibuserv/screenshots/11.png" width="49.5%">
</a>

<a href="https://tehnick.github.io/sibuserv/screenshots/05.png" title="Example of page with build results">
    <img src="https://tehnick.github.io/sibuserv/screenshots/05.png" width="49.5%">
</a>
<a href="https://tehnick.github.io/sibuserv/screenshots/12.png" title="Example of page with code analysis results">
    <img src="https://tehnick.github.io/sibuserv/screenshots/12.png" width="49.5%">
</a>

### Translations

[Localization files](https://www.transifex.com/tehnick/sibuserv-webui/) are managed using [Transifex](https://en.wikipedia.org/wiki/Transifex) web servise.

### Useful links

* https://en.wikipedia.org/wiki/Comparison_of_continuous_integration_software
* https://github.com/sibuserv/sibuserv
* https://github.com/sibuserv/lxe
* https://github.com/sibuserv/mxe
* http://mxe.cc/

