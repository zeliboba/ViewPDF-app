#ifndef ANNOTATION_HPP
#define ANNOTATION_HPP
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

#include "common.hpp"

struct RectDetails {

    RectDetails(const QRectF &rect_, const bool isHighlighted_,
                const QColor &color_)
        : rect(rect_), isHighlighted(isHighlighted_), color(color_) {}

    const QRectF rect;
    const bool isHighlighted;
    const QColor color;

#if QT_VERSION < 0x040700
    RectDetails() : isHighlighted(false) {}
    RectDetails operator=(const RectDetails &other)
    {
        return RectDetails(other.rect, other.isHighlighted, other.color);
    }
#endif
};


struct AnnotationDetails {

    AnnotationDetails(const QStringList &texts_, const QRectF &rect_,
                      const bool isHighlighted_, const QColor &color_
#ifdef SHOW_DELETED
                      , const QList<QRectF> quadRects_
#endif
                      )
        : texts(texts_), rectDetails(rect_, isHighlighted_, color_)
#ifdef SHOW_DELETED
          , quadRects(quadRects_)
#endif
        {}

    const QStringList texts;
    const RectDetails rectDetails;
#ifdef SHOW_DELETED
    const QList<QRectF> quadRects;
#endif

#if QT_VERSION < 0x040700
    AnnotationDetails() {}
#endif
};


#endif // ANNOTATION_HPP
