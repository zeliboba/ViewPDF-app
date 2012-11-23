#ifndef SCROLLAREA_HPP
#define SCROLLAREA_HPP
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

#include <QScrollArea>

class QKeyEvent;

class ScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    ScrollArea(QWidget *parent=0) : QScrollArea(parent) {}

signals:
    void previousPage();
    void nextPage();

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // SCROLLAREA_HPP
