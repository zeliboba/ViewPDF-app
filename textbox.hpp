#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP
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

#include <QRectF>
#include <QString>


struct TextBox {

    TextBox(const QRectF &rect_, const QString &word_)
        : rect(rect_), word(word_) {}

    const QRectF rect;
    const QString word;

#if QT_VERSION < 0x040700
    TextBox() {}
    TextBox operator=(const TextBox &other)
    {
        return TextBox(other.rect, other.word);
    }
#endif
};

#endif // TEXTBOX_HPP
