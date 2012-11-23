#ifndef HELPFORM_HPP
#define HELPFORM_HPP
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

#include <QMainWindow>

class QMainWindow;

class HelpForm : public QMainWindow
{
    Q_OBJECT

public:
    HelpForm(QWidget *parent=0);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // HELPFORM_HPP
