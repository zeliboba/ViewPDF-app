#ifndef COMMON_HPP
#define COMMON_HPP
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

#include <poppler-qt4.h>
#if QT_VERSION >= 0x040600
#include <QSharedPointer>
#else
#include <tr1/memory>
#endif

#if QT_VERSION >= 0x040600
typedef QSharedPointer<Poppler::Document> PdfDocument;
typedef QSharedPointer<Poppler::Page> PdfPage;
typedef QSharedPointer<Poppler::TextBox> PdfTextBox;
#else
typedef std::tr1::shared_ptr<Poppler::Document> PdfDocument;
typedef std::tr1::shared_ptr<Poppler::Page> PdfPage;
typedef std::tr1::shared_ptr<Poppler::TextBox> PdfTextBox;
#endif

#endif // COMMON_HPP
