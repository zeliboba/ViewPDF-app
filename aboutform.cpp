/*
    Copyright (c) 2011-12 Qtrac Ltd. All rights reserved.
    This program or module is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 2 of
    the License, or (at your option) any later version. This program is
    distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
    for more details.
*/

#include "aboutform.hpp"
#include <poppler-version.h>
#include <QApplication>
#include <QHBoxLayout>
#include <QSettings>
#include <QShortcut>
#include <QTabWidget>
#include <QTextBrowser>


static const QString Version("0.5.0");


AboutForm::AboutForm(QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QTextBrowser *aboutBrowser = new QTextBrowser;
    aboutBrowser->setReadOnly(true);
    aboutBrowser->setHtml(tr(
    "<table border=0>"
    "<tr><td width=70%><b>%1</a> %2</b> by Mark Summerfield</td>"
    "<td rowspan=2><img align=right src=\":/icon.png\"></td></tr>"
    "<tr><td><tt>&lt;mark@qtrac.eu&gt;</tt>.</td></tr>"
    "<tr><td colspan=2 width=100%>Copyright &copy; 2012 "
    "<a href=\"http://www.qtrac.eu\">Qtrac</a> Ltd. All rights reserved."
    "</td></tr>"
    "<tr><td colspan=2>Built with Qt %3 and Poppler %4.</td></tr>"
    "</table><hr>"
    "<p>This program views PDF files, clearly showing annotations, "
    "double-words, and spelling mistakes."
    "<hr><p>If you like %1 you might like my books:<ul>"
    "<li><a href=\"http://www.qtrac.eu/gobook.html\">"
    "Programming in Go</a></li>"
    "<li><a href=\"http://www.qtrac.eu/aqpbook.html\">"
    "Advanced Qt Programming</a></li>"
    "<li><a href=\"http://www.qtrac.eu/py3book.html\">"
    "Programming in Python 3</a></li>"
    "<li><a href=\"http://www.qtrac.eu/pyqtbook.html\">"
    "Rapid GUI Programming with Python and Qt</a></li>"
    "</ul>"
    "I also provide training and consultancy in C++, Go, Python&nbsp;2, "
    "Python&nbsp;3, C++/Qt, and PyQt4.").arg(qApp->applicationName())
            .arg(Version).arg(qVersion()).arg(POPPLER_VERSION));
    QTextBrowser *licenceBrowser = new QTextBrowser;
    licenceBrowser->setReadOnly(true);
    licenceBrowser->setHtml(tr(
    "This program is free software: you can redistribute it "
    "and/or modify it under the terms of the GNU General Public License "
    "as published by the Free Software Foundation, either version 2 of "
    "the License, or (at your option), any "
    "later version. This program is distributed in the hope that it will "
    "be useful, but WITHOUT ANY WARRANTY; without even the implied "
    "warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
    "See the GNU General Public License (in file <tt>gpl-2.0.txt</tt>) "
    "for more details."));
    QTabWidget *tabWidget = new QTabWidget;
    tabWidget->addTab(aboutBrowser, tr("&About"));
    tabWidget->addTab(licenceBrowser, tr("&License"));
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(tabWidget);
    setLayout(layout);
    resize(480, 400);
    setWindowTitle(tr("%1 — About").arg(qApp->applicationName()));
}
