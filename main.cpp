/*
    Copyright (c) 2012 Qtrac Ltd. All rights reserved.
    This program or module is free software: you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version 2 of
    the License, or (at your option) any later version. This program is
    distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
    for more details.
*/

#include "mainwindow.hpp"
#include <QApplication>
#include <QIcon>
#include <QTextCodec>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setCursorFlashTime(0);
    app.setOrganizationName("Qtrac Ltd.");
    app.setOrganizationDomain("qtrac.eu");
    app.setApplicationName("ViewPDF");
    app.setWindowIcon(QIcon(":/icon.png"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QString filename;
    int page = 0;
    if (argc > 1) {
        filename = argv[1];
        if (argc > 2)
            page = QString(argv[2]).toInt();
    }
    MainWindow window(filename, page);
    window.show();
    return app.exec();
}

