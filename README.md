## Sibuserv Web UI – Simple Build Server Web User Interface

### Copyright

2016-2017 Boris Pek \<tehnick-8@yandex.ru\>

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

Sibuserv CI supported features and future plans:

Task        | Status | Priority | Comment
----------- | ------ | -------- | -------
Trigger     | ✔ / ✘  | low      | Triggers for testing of merge requests are not supported yet.
Update      | ✔      | hight    | 
Analyse     | ✔      | hight    | 
Build       | ✔      | hight    | 
UnitTest    | ✘      | medium   | 
Deploy      | ✘      | low      | 
Test        | ✘      | low      | 
Archive     | ✔      | hight    | 
Report      | ✘      | medium   | 

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

### Useful links

* https://en.wikipedia.org/wiki/Comparison_of_continuous_integration_software
* https://github.com/sibuserv/sibuserv
* https://github.com/sibuserv/lxe
* https://github.com/sibuserv/mxe
* http://mxe.cc/

