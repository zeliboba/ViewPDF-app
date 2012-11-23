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

#include "aboutform.hpp"
#include "helpform.hpp"
#include "scrollarea.hpp"
#include "mainwindow.hpp"
#include "enchant++.h"
#if defined DEBUG || defined DEBUG_KIND || defined DEBUG_SPELLING
#include <QtDebug>
#endif
#include <cmath>
#include <poppler-version.h>
#include <QApplication>
#include <QDockWidget>
#include <QBoxLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QLocale>
#include <QMap>
#include <QMessageBox>
#include <QPainter>
#include <QPixmapCache>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QTextStream>


static const double Margin = 1.5;
static const double ZigZag = 3.5;
static const double ZigZagX2 = ZigZag * 2;
static const int PointCount = 3;
static const QChar Apostrophe(0x2019); // ’
static const QString ApostropheS(QString(Apostrophe) + "s"); // ’s


MainWindow::MainWindow(const QString &filename, int number,
        QWidget *parent)
    : QMainWindow(parent),
      controlDockArea(Qt::RightDockWidgetArea),
      spellingDockArea(Qt::RightDockWidgetArea),
      annotationsDockArea(Qt::BottomDockWidgetArea), activeAnnotations(0),
      currentPath("."), dictionary(0)
{
    createWidgets();
    createLayouts();
    createConnections();

    QSettings settings;
    restoreGeometry(settings.value("MainWindow/Geometry").toByteArray());
    restoreState(settings.value("MainWindow/State").toByteArray());
    controlDockWidget->resize(controlDockWidget->minimumSizeHint());
    spellingDockWidget->resize(spellingDockWidget->minimumSizeHint());
    annotationDockLocationChanged(static_cast<Qt::DockWidgetArea>(
                settings.value("MainWindow/AnnotationDockArea",
                        static_cast<int>(annotationsDockArea)).toInt()));
    annotationsDockWidget->resize(
            annotationsDockWidget->minimumSizeHint());
    setWindowTitle(qApp->applicationName());
    fileOpenButton->setFocus();
    loadDictionary();
    if (!filename.isEmpty()) {
        fileOpen(filename);
        pageSpinBox->setValue(number ? number : pageSpinBox->minimum());
        view->setFocus();
    }
}


MainWindow::~MainWindow()
{
    delete dictionary;
}


void MainWindow::createWidgets()
{
    QSettings settings;

    view = new QLabel;
    view->setAlignment(Qt::AlignCenter);
    area = new ScrollArea;
    area->setWidget(view);
    area->setWidgetResizable(true);
    setCentralWidget(area);

    setDockOptions(QMainWindow::AnimatedDocks);
    QDockWidget::DockWidgetFeatures features =
            QDockWidget::DockWidgetMovable|
            QDockWidget::DockWidgetFloatable;

    controlDockWidget = new QDockWidget(tr("Controls"), this);
    controlDockWidget->setObjectName("Controls");
    controlDockWidget->setFeatures(features);
    controlDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|
                                       Qt::RightDockWidgetArea);
    fileOpenButton = new QPushButton(tr("&Open..."));
    pageLabel = new QLabel(tr("&Page:"));
    pageSpinBox = new QSpinBox;
    pageSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    pageLabel->setBuddy(pageSpinBox);
    zoomLabel = new QLabel(tr("&Zoom:"));
    zoomSpinBox = new QSpinBox;
    zoomLabel->setBuddy(zoomSpinBox);
    zoomSpinBox->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    zoomSpinBox->setRange(20, 800);
    zoomSpinBox->setSuffix(tr("%"));
    zoomSpinBox->setSingleStep(10);
    zoomSpinBox->setValue(settings.value("MainWindow/Zoom", "100")
            .toInt());
    aboutButton = new QPushButton(tr("A&bout"));
    helpButton = new QPushButton(tr("Help"));
    helpButton->setShortcut(tr("F1"));
    quitButton = new QPushButton(tr("&Quit"));
    statusLabel = new QLabel(tr("Click <b>Open...</b>"));
    statusLabel->setFrameStyle(QFrame::StyledPanel);

    spellingDockWidget = new QDockWidget(tr("Spelling"), this);
    spellingDockWidget->setObjectName("Spelling");
    spellingDockWidget->setFeatures(features|
            QDockWidget::DockWidgetClosable);
    spellingDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|
                                        Qt::RightDockWidgetArea);
    addWordButton = new QPushButton(tr("&Add..."));
    removeWordButton = new QPushButton(tr("&Remove..."));
    languageButton = new QPushButton(tr("&Language..."));

    annotationsDockWidget = new QDockWidget(tr("Annotations"), this);
    annotationsDockWidget->setObjectName("Annotations");
    annotationsDockWidget->setFeatures(features|
            QDockWidget::DockWidgetClosable);
    annotationsView[0] = new QTextBrowser;
    annotationsView[1] = new QTextBrowser;
    previousAnnotatedPageButton[0] = new QPushButton(tr("Pre&vious"));
    previousAnnotatedPageButton[1] = new QPushButton(tr("Pre&vious"));
    nextAnnotatedPageButton[0] = new QPushButton(tr("&Next"));
    nextAnnotatedPageButton[1] = new QPushButton(tr("&Next"));
    annotationStack = new QStackedWidget;

    foreach (QPushButton *button, QList<QPushButton*>() << fileOpenButton
            << aboutButton << helpButton << quitButton << addWordButton
            << removeWordButton << languageButton
            << previousAnnotatedPageButton[0]
            << previousAnnotatedPageButton[1]
            << nextAnnotatedPageButton[0] << nextAnnotatedPageButton[1])
        button->setFocusPolicy(Qt::NoFocus);
}


void MainWindow::createLayouts()
{
    QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->addWidget(fileOpenButton);
    QHBoxLayout *pageLayout = new QHBoxLayout;
    pageLayout->addWidget(pageLabel);
    pageLayout->addWidget(pageSpinBox);
    controlLayout->addLayout(pageLayout);
    QHBoxLayout *zoomLayout = new QHBoxLayout;
    zoomLayout->addWidget(zoomLabel);
    zoomLayout->addWidget(zoomSpinBox);
    controlLayout->addLayout(zoomLayout);
    controlLayout->addWidget(helpButton);
    controlLayout->addWidget(aboutButton);
    controlLayout->addWidget(quitButton);
    controlLayout->addStretch();
    controlLayout->addWidget(statusLabel);
    QWidget *widget = new QWidget;
    widget->setLayout(controlLayout);
    controlDockWidget->setWidget(widget);
    addDockWidget(controlDockArea, controlDockWidget);

    QVBoxLayout *spellingLayout = new QVBoxLayout;
    spellingLayout->addWidget(addWordButton);
    spellingLayout->addWidget(removeWordButton);
    spellingLayout->addWidget(languageButton);
    spellingLayout->addStretch();
    widget = new QWidget;
    widget->setLayout(spellingLayout);
    spellingDockWidget->setWidget(widget);
    addDockWidget(spellingDockArea, spellingDockWidget);

    QVBoxLayout *annotationVLayout = new QVBoxLayout;
    annotationVLayout->addWidget(previousAnnotatedPageButton[0]);
    annotationVLayout->addWidget(nextAnnotatedPageButton[0]);
    annotationVLayout->addWidget(annotationsView[0]);
    widget = new QWidget;
    widget->setLayout(annotationVLayout);
    annotationStack->addWidget(widget);
    QHBoxLayout *annotationHLayout = new QHBoxLayout;
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(previousAnnotatedPageButton[1]);
    buttonLayout->addWidget(nextAnnotatedPageButton[1]);
    buttonLayout->addStretch();
    annotationHLayout->addLayout(buttonLayout);
    annotationHLayout->addWidget(annotationsView[1]);
    widget = new QWidget;
    widget->setLayout(annotationHLayout);
    annotationStack->addWidget(widget);
    annotationsDockWidget->setWidget(annotationStack);
    addDockWidget(annotationsDockArea, annotationsDockWidget);
}


void MainWindow::createConnections()
{
    connect(fileOpenButton, SIGNAL(clicked()), this, SLOT(fileOpen()));
    connect(zoomSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateUi()));
    connect(pageSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateUi()));
    connect(helpButton, SIGNAL(clicked()), this, SLOT(help()));
    connect(aboutButton, SIGNAL(clicked()), this, SLOT(about()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    connect(addWordButton, SIGNAL(clicked()), this, SLOT(addWord()));
    connect(removeWordButton, SIGNAL(clicked()), this, SLOT(removeWord()));
    connect(languageButton, SIGNAL(clicked()),
            this, SLOT(changeLanguage()));

    connect(area, SIGNAL(previousPage()), this, SLOT(previousPage()));
    connect(area, SIGNAL(nextPage()), this, SLOT(nextPage()));

    connect(previousAnnotatedPageButton[0], SIGNAL(clicked()),
            this, SLOT(previousAnnotatedPage()));
    connect(previousAnnotatedPageButton[1], SIGNAL(clicked()),
            this, SLOT(previousAnnotatedPage()));
    connect(nextAnnotatedPageButton[0], SIGNAL(clicked()),
            this, SLOT(nextAnnotatedPage()));
    connect(nextAnnotatedPageButton[1], SIGNAL(clicked()),
            this, SLOT(nextAnnotatedPage()));

    connect(controlDockWidget, SIGNAL(topLevelChanged(bool)),
            this, SLOT(controlTopLevelChanged(bool)));
    connect(spellingDockWidget, SIGNAL(topLevelChanged(bool)),
            this, SLOT(spellingTopLevelChanged(bool)));
    connect(spellingDockWidget, SIGNAL(visibilityChanged(bool)),
            this, SLOT(updateUi()));
    connect(annotationsDockWidget, SIGNAL(topLevelChanged(bool)),
            this, SLOT(annotationsTopLevelChanged(bool)));
    connect(annotationsDockWidget, SIGNAL(visibilityChanged(bool)),
            this, SLOT(updateUi()));
    connect(annotationsDockWidget,
            SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(annotationDockLocationChanged(Qt::DockWidgetArea)));
}


void MainWindow::closeEvent(QCloseEvent*)
{
    saveWords(false);
    QSettings settings;
    settings.setValue("MainWindow/Geometry", saveGeometry());
    settings.setValue("MainWindow/State", saveState());
    settings.setValue("MainWindow/Zoom", zoomSpinBox->value());
    settings.setValue("Language", dictionaryLanguage);
    settings.setValue("MainWindow/AnnotationDockArea",
                      static_cast<int>(annotationsDockArea));
    QMainWindow::close();
}


void MainWindow::controlTopLevelChanged(bool floating)
{
    controlDockWidget->setWindowTitle(floating
            ? tr("%1 — Controls").arg(qApp->applicationName())
            : tr("Controls"));
}


void MainWindow::spellingTopLevelChanged(bool floating)
{
    spellingDockWidget->setWindowTitle(floating
            ? tr("%1 — Spelling").arg(qApp->applicationName())
            : tr("Spelling"));
}


void MainWindow::annotationsTopLevelChanged(bool floating)
{
    annotationsDockWidget->setWindowTitle(floating
            ? tr("%1 — Annotations").arg(qApp->applicationName())
            : tr("Annotations"));
}


void MainWindow::annotationDockLocationChanged(Qt::DockWidgetArea area)
{
    activeAnnotations = (area == Qt::TopDockWidgetArea ||
                         area == Qt::BottomDockWidgetArea) ? 1 : 0;
    annotationStack->setCurrentIndex(activeAnnotations);
    annotationsDockArea = area;
}


void MainWindow::fileOpen(const QString &filename)
{
    fileName = filename;
    if (fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(this,
                tr("%1 — Choose File").arg(qApp->applicationName()),
                currentPath, tr("PDF files (*.pdf)"));
    if (!fileName.isEmpty()) {
        currentPath = QFileInfo(fileName).canonicalPath();
        loadPdf();
    }
}


void MainWindow::loadPdf()
{
    setWindowTitle(qApp->applicationName());
    view->clear();
#if QT_VERSION >= 0x040600
    pdf.clear();
#else
    pdf.reset();
#endif
    pdf = PdfDocument(Poppler::Document::load(fileName));
    if (pdf) {
        clear();
        saveWords();
        setWindowTitle(tr("%1 — %2").arg(QFileInfo(fileName).fileName())
                .arg(qApp->applicationName()));
        pdf->setRenderHint(Poppler::Document::Antialiasing);
        pdf->setRenderHint(Poppler::Document::TextAntialiasing);
        pdf->setRenderHint(Poppler::Document::TextSlightHinting);
        pageSpinBox->setRange(1, pdf->numPages());
        pageSpinBox->setValue(1);
        pageSpinBox->setFocus();
        wordsFilename = QString("%1/words.lst")
            .arg(QFileInfo(fileName).path());
        loadWords();
    }
    else if (!pdf)
        QMessageBox::warning(this, tr("%1 — Error")
                .arg(qApp->applicationName()),
                tr("Cannot load '%1'.").arg(fileName));
    else if (pdf->isLocked()) {
        QMessageBox::warning(this, tr("%1 — Error")
                .arg(qApp->applicationName()),
                tr("Cannot read a locked PDF ('%1').").arg(fileName));
#if QT_VERSION >= 0x040600
        pdf.clear();
#else
        pdf.reset();
#endif
    }
    updateUi();
    view->setFocus();
}


void MainWindow::clear()
{
    QPixmapCache::clear();
    annotationsForPage.clear();
    textsForPage.clear();
}


void MainWindow::loadWords()
{
    if (QFile::exists(wordsFilename)) {
        QFile file(wordsFilename);
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            QTextStream in(&file);
            in.setCodec("utf-8");
            words.clear();
            QString word = in.readLine();
            while (!word.isNull()) {
                words << word;
                word = in.readLine();
            }
            file.close();
        }
        else
            QMessageBox::warning(this, tr("%1 — Error")
                    .arg(qApp->applicationName()),
                    tr("Failed to load words '%1'.").arg(wordsFilename));
    }
}


void MainWindow::saveWords(bool report)
{
    if (!wordsFilename.isEmpty()) {
        if (words.isEmpty() && QFile::exists(wordsFilename)) {
            QFile::remove(wordsFilename);
            return;
        }
        QFile file(wordsFilename);
        if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
            QTextStream out(&file);
            out.setCodec("utf-8");
            QStringList wordList = words.toList();
            wordList.sort();
            foreach (const QString &word, wordList)
                out << word << "\n";
            file.close();
        }
        else if (report)
            QMessageBox::warning(this, tr("%1 — Error")
                    .arg(qApp->applicationName()),
                    tr("Failed to save words '%1'.")
                    .arg(wordsFilename));
        else {
            QTextStream err(stderr);
            err << tr("%1: Failed to save words '%2'.\n")
                    .arg(qApp->applicationName()).arg(wordsFilename);
        }
    }
}


void MainWindow::updateUi()
{
    bool enable = pdf;
    pageSpinBox->setEnabled(enable);
    zoomSpinBox->setEnabled(enable);
    spellingDockWidget->setEnabled(enable);
    annotationsDockWidget->setEnabled(enable);
    updatePage();
}


void MainWindow::updatePage()
{
    if (!pdf)
        return;
    const int number = pageSpinBox->value();
    page = PdfPage(pdf->page(number - 1));
    if (!annotationsForPage.contains(number))
        annotationsForPage.insert(number, getAnnotations());
    if (annotationsDockWidget->isVisible()) {
        // Cheaper to duplicate the text than to refresh the page view
        annotationsView[0]->setHtml(
                annotationsForPage.value(number).first);
        annotationsView[1]->setHtml(
                annotationsForPage.value(number).first);
    }
    else {
        annotationsView[0]->clear();
        annotationsView[1]->clear();
    }
    if (!textsForPage.contains(number))
        textsForPage.insert(number, getTexts());
    const int DPI = static_cast<int>(72.0 *
            (zoomSpinBox->value() / 100.0));
    QPixmap pixmap;
    const QString key = cacheKey();
#if QT_VERSION >= 0x040600
    if (!QPixmapCache::find(key, &pixmap)) {
#else
    if (!QPixmapCache::find(key, pixmap)) {
#endif
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        // renderToPainter() doesn't seem to be linkable in poppler 0.18
        QImage image = page->renderToImage(DPI, DPI);
        if (annotationsDockWidget->isVisible() && dictionary)
            addAnnotations(&image, number);
        if (spellingDockWidget->isVisible())
            addSpellings(&image, number);
        pixmap = QPixmap::fromImage(image);
        QPixmapCache::insert(key, pixmap);
        QApplication::restoreOverrideCursor();
    }
    view->setPixmap(pixmap);
    statusLabel->setText(tr("Page %1 of %2").arg(number)
            .arg(pdf->numPages()));
}


void MainWindow::about()
{
    AboutForm *form = new AboutForm(this);
    form->show();
}


void MainWindow::previousPage()
{
    int index = pageSpinBox->value();
    if (index > 0) {
        pageSpinBox->setValue(index - 1);
        area->verticalScrollBar()->setValue(0);
    }
}


void MainWindow::nextPage()
{
    int index = pageSpinBox->value();
    if (index < pageSpinBox->maximum()) {
        pageSpinBox->setValue(index + 1);
        area->verticalScrollBar()->setValue(0);
    }
}


void MainWindow::previousAnnotatedPage()
{
    if (!pdf)
        return;
    int number = pageSpinBox->value();
    while (--number > 0) {
        PdfPage prevPage(pdf->page(number - 1));
        if (!prevPage)
            return;
        QList<Poppler::Annotation*> annotations = prevPage->annotations();
        bool hasAnnotations = hasRecognizedAnnotation(annotations);
        qDeleteAll(annotations);
        if (hasAnnotations) {
            pageSpinBox->setValue(number);
            break;
        }
    }
}


void MainWindow::nextAnnotatedPage()
{
    if (!pdf)
        return;
    int number = pageSpinBox->value();
    while (++number <= pageSpinBox->maximum()) {
        PdfPage nextPage(pdf->page(number - 1));
        if (!nextPage)
            return;
        QList<Poppler::Annotation*> annotations = nextPage->annotations();
        bool hasAnnotations = hasRecognizedAnnotation(annotations);
        qDeleteAll(annotations);
        if (hasAnnotations) {
            pageSpinBox->setValue(number);
            break;
        }
    }
}


void MainWindow::help()
{
    HelpForm *form = new HelpForm("en", this);
    form->show();
}


void MainWindow::addWord()
{
    QStringList wordList = words.toList();
    wordList.sort();
    const QString word = QInputDialog::getItem(this,
            tr("%1 — Add Word").arg(qApp->applicationName()),
            tr("&Add:"), wordList);
    if (!word.isEmpty()) {
        words << word.toLower();
        QPixmapCache::clear(); // May need to repaint all pages
        updatePage();
    }
}


void MainWindow::removeWord()
{
    QStringList wordList = words.toList();
    wordList.sort();
    const QString word = QInputDialog::getItem(this,
            tr("%1 — Remove Word").arg(qApp->applicationName()),
            tr("&Remove:"), wordList, 0, false);
    if (!word.isEmpty()) {
        words.remove(word);
        QPixmapCache::clear(); // May need to repaint all pages
        updatePage();
    }
}


QString MainWindow::cacheKey() const
{
    return QString("%1:%2:%3:%4").arg(pageSpinBox->value())
            .arg(zoomSpinBox->value())
            .arg(annotationsDockWidget->isVisible() ? "1" : "0")
            .arg(spellingDockWidget->isVisible() ? "1" : "0");
}


void MainWindow::addAnnotations(QImage *image, const int number)
{
    const QList<RectDetails> &rects = annotationsForPage.value(number)
        .second;
    if (rects.count()) {
        double prevY = 0.0;
        QPainter painter(image);
        QFont font = painter.font();
        font.setPointSizeF(10.5);
        painter.setFont(font);
        painter.setPen(Qt::magenta);
        const double Y = painter.fontMetrics().height() / 2.0;
        for (int i = 0; i < rects.count(); ++i) {
            const RectDetails &details = rects.at(i);
            QRectF rect = details.rect;
            double y = (rect.y() * image->height()) + Y;
            if (qFuzzyCompare(y, prevY))
                y += Y;
            prevY = y;
            painter.drawText(Margin, y, QString("(%1)").arg(i + 1));
            rect = adjustRect(rect, image->width(), image->height());
            if (!details.isHighlighted)
                painter.fillRect(rect, details.color);
        }
        painter.end();
    }
}


QPair<QString, QList<RectDetails> > MainWindow::getAnnotations()
{
    QPair<QString, QList<RectDetails> > data;
    if (!page)
        return data;
    QList<Poppler::Annotation*> annotations = page->annotations();
    if (annotations.count()) {
        QMultiMap<QPair<double, double>, AnnotationDetails>
                annotationForYx;
        foreach (Poppler::Annotation *annotation, annotations) {
            const AnnotationDetails &details = getAnnotationDetails(
                    annotation);
            if (!details.texts.isEmpty()) {
                const QPointF &point =
                        details.rectDetails.rect.bottomLeft();
                annotationForYx.insert(qMakePair(point.y(), point.x()),
                                       details);
            }
        }
        if (annotationForYx.count()) {
            data.first = "<table border=0>";
            int count = 0;
            QMapIterator<QPair<double, double>, AnnotationDetails> i(
                    annotationForYx);
            while (i.hasNext()) {
                i.next();
                const AnnotationDetails &details = i.value();
                foreach (const QString &text, details.texts)
                    data.first += "<tr><td align=right>"
                            "<font color=\"#FF00FF\">(" +
                            QString::number(++count) +
                            ")</font>&nbsp;</td><td>" + text +
                            "</td></tr>";
                data.second << details.rectDetails;
            }
            data.first += "</table>";
        }
    }
    qDeleteAll(annotations);
    return data;
}


AnnotationDetails MainWindow::getAnnotationDetails(
        Poppler::Annotation *annotation)
{
#if POPPLER_VERSION_MAJOR == 0 && POPPLER_VERSION_MINOR < 20
    QString action = annotation->window.summary;
#else
    QString action = annotation->popup().summary();
#endif
    if (action == "Replacement Text")
        action = tr("Replace with");
    else if (action == "Inserted Text")
        action = tr("Insert");
    else if (action == "Cross-Out")
        action = tr("Delete");
    else if (action == "Comment on Text")
        action = tr("Comment");
#ifdef SHOW_DELETED
    QList<QRectF> quadRects;
#endif
    QColor color = Qt::transparent;
    QStringList texts;
    bool isHighlighted = false;
    QString text = annotation->contents().simplified();
    Poppler::CaretAnnotation *caret =
        dynamic_cast<Poppler::CaretAnnotation*>(annotation);
    if (caret) {
        if (!text.isEmpty())
            texts << tr("<i>%1:</i> <font color=\"blue\">%2</font>")
                    .arg(action.isEmpty() ? tr("Insert") : action)
                    .arg(text);
        else
            texts << tr("<i>%1</i>").arg(action.isEmpty() ? tr("Insert")
                                                          : action);
        color = QColor(0, 0, 0xFF, 0x37); // semi-transparent blue
#ifdef DEBUG_KIND
        kind = "Caret";
#endif
    }
    Poppler::HighlightAnnotation *highlight =
        dynamic_cast<Poppler::HighlightAnnotation*>(annotation);
    if (highlight) {
        if (highlight->highlightType() ==
            Poppler::HighlightAnnotation::StrikeOut) {
#ifdef SHOW_DELETED
            QList<Poppler::HighlightAnnotation::Quad> quads =
                highlight->highlightQuads();
            foreach (const Poppler::HighlightAnnotation::Quad &quad, quads)
// TODO I need to find out what these points mean; they don't seem to be
// top-left & bottom-right?
                quadRects << QRectF(quad.points[0], quad.points[2]);
// This should get the text that is to be deleted so that it can be
// shown in the annotations view; but it doesn't work
            if (text.isEmpty() && view->pixmap() &&
                !view->pixmap()->isNull() && !quadRects.isEmpty()) {
                foreach (QRectF rect, quadRects) {
                    rect = adjustRect(rect, view->pixmap()->width(),
                            view->pixmap()->height());
                    text = page->text(rect);
#ifdef DEBUG
qDebug() << rect.toRect() << text;
#endif
                    if (!text.isEmpty())
                        break;
                }
            }
#endif
            if (!text.isEmpty())
                texts << tr("<i>%1:</i> <font color=\"red\">%2</font>")
                    .arg(action.isEmpty() ? tr("Delete") : action)
                    .arg(text);
            else
                texts << tr("<i>%1</i>").arg(action.isEmpty()
                        ? tr("Delete") : action);
            color = QColor(0xFF, 0, 0, 0x37); // semi-transparent red
#ifdef DEBUG_KIND
            kind = "Highlight";
#endif
        }
        else {
            isHighlighted = true;
            if (!text.isEmpty())
                texts << tr("<i>%1:</i> <font color=\"#8B4513\">%2</font>")
                    .arg(action.isEmpty() ? tr("Highlight Note") : action)
                    .arg(text);
            else
                texts << tr("<i>%1</i>").arg(action.isEmpty()
                        ? tr("Highlight Note") : action);
#ifdef DEBUG_KIND
            kind = "Highlighted Note";
#endif
        }
    }
    Poppler::TextAnnotation *note =
        dynamic_cast<Poppler::TextAnnotation*>(annotation);
    if (note) {
        if (!text.isEmpty())
            texts << tr("<i>%1:</i> <font color=\"#8B4513\">%2</font>")
                .arg(action.isEmpty() ? tr("Note") : action).arg(text);
        else
            texts << tr("<i>%1</i>").arg(action.isEmpty() ? tr("Note")
                                                          : action);
#ifdef DEBUG_KIND
        kind = "Note";
#endif
    }
#ifdef DEBUG_KIND
    Poppler::FileAttachmentAnnotation *attachment =
        dynamic_cast<Poppler::FileAttachmentAnnotation*>(annotation);
    if (attachment)
        kind = "Attachment";
    Poppler::InkAnnotation *ink =
        dynamic_cast<Poppler::InkAnnotation*>(annotation);
    if (ink)
        kind = "Ink";
    Poppler::GeomAnnotation *geom =
        dynamic_cast<Poppler::GeomAnnotation*>(annotation);
    if (geom)
        kind = "Geometry";
    Poppler::LineAnnotation *line =
        dynamic_cast<Poppler::LineAnnotation*>(annotation);
    if (line)
        kind = "Line";
    Poppler::LinkAnnotation *link =
        dynamic_cast<Poppler::LinkAnnotation*>(annotation);
    if (link)
        kind = "Link";
    Poppler::MovieAnnotation *movie =
        dynamic_cast<Poppler::MovieAnnotation*>(annotation);
    if (movie)
        kind = "Movie";
    Poppler::SoundAnnotation *sound =
        dynamic_cast<Poppler::SoundAnnotation*>(annotation);
    if (sound)
        kind = "Sound";
    Poppler::StampAnnotation *stamp =
        dynamic_cast<Poppler::StampAnnotation*>(annotation);
    if (stamp)
        kind = "Stamp";
    qDebug() << kind << ":" << text;
#endif
    QRectF rect = annotation->boundary();
#ifdef MY_BOUNDARY_BUG
    const double Size = 0.015;
    if (rect.width() > 0.1 || rect.height() > 0.1) {
        QPointF topLeft = rect.topRight();
        topLeft.setX(topLeft.x() - Size);
        QPointF bottomRight = rect.topRight();
        bottomRight.setY(bottomRight.y() + Size);
        rect = QRectF(topLeft, bottomRight);
    }
#endif
#ifdef SHOW_DELETED
    return AnnotationDetails(texts, rect, isHighlighted, color, quadRects);
#else
    return AnnotationDetails(texts, rect, isHighlighted, color);
#endif
}


bool MainWindow::hasRecognizedAnnotation(
            const QList<Poppler::Annotation*> &annotations)
{
    foreach (Poppler::Annotation *annotation, annotations) {
        switch (annotation->subType()) {
            case Poppler::Annotation::AText:    // fallthrough
            case Poppler::Annotation::ACaret:   // fallthrough
            case Poppler::Annotation::AHighlight: return true;
            default: break; // ignore other enum values
        }
    }
    return false;
}


void MainWindow::addSpellings(QImage *image, const int number)
{
    QMultiMap<QPair<int, int>, TextBox> textForYx =
        textsForPage.value(number);
    if (textForYx.isEmpty())
        return;
#ifdef DEBUG_SPELLING
QMapIterator<QPair<int, int>, TextBox> i(textForYx);
int count = 0;
while (i.hasNext()) {
    i.next();
    qDebug() << count++ << i.value().word << i.key().first <<
        i.key().second;
}
#endif
    int spellCount = 0;
    int queryCount = 0;
    const double FACTOR = zoomSpinBox->value() / 100.0;
    QPen spellingPen(Qt::red, 1.5);
    QPen queryWordPen(Qt::green, 1.5);
    QPainter painter(image);
    painter.setPen(spellingPen);
    const QList<TextBox> boxes = textForYx.values();
    for (int i = 0; i < boxes.count(); ++i) {
        const TextBox &box = boxes.at(i);
        const QRectF rect = adjustRect(box.rect, FACTOR, FACTOR);
        // double-word check
        if (i > 0 && box.word.length() > 1 &&
            box.word.toLower() == boxes.at(i - 1).word.toLower()) {
            painter.setPen(queryWordPen);
            QRectF lineRect = rect;
            lineRect.setBottom(lineRect.bottom() + Margin);
            drawZigZagLine(&painter, lineRect);
            lineRect = adjustRect(boxes.at(i - 1).rect, FACTOR, FACTOR);
            lineRect.setBottom(lineRect.bottom() + Margin);
            drawZigZagLine(&painter, lineRect);
            painter.setPen(spellingPen);
            ++queryCount;
        }
        if (box.word.length() <= 1)
            continue; // Skip single character "words"
        if (words.contains(box.word.toLower()) ||
            validWord(box.word) || validExceptionalWord(box.word))
            continue;
        if (box.word.endsWith("-")) {
            const QString word = box.word.left(box.word.length() - 1);
            if (words.contains(word.toLower()) || validWord(word) ||
                validExceptionalWord(word))
                continue;
            if (i + 1 < boxes.count() &&
                (words.contains(box.word.toLower() +
                                boxes.at(i + 1).word) ||
                 words.contains(word.toLower() + boxes.at(i + 1).word) ||
                 validWord(box.word + boxes.at(i + 1).word) ||
                 validWord(word + boxes.at(i + 1).word))) {
                ++i;
                continue;
            }
        }
        if (i > 0) {
            const QString &prevWord = boxes.at(i - 1).word;
            if (prevWord.endsWith("-") &&
                (validWord(prevWord + box.word) ||
                 validWord(prevWord.left(prevWord.length() - 1) +
                           box.word)))
                continue;
        }
        // upper-upper-lower check (but only if a misspelling)
        if (box.word.length() > 2 && box.word.at(0).isUpper() &&
            box.word.at(1).isUpper() && box.word.at(2).isLower()) {
            painter.setPen(queryWordPen);
            QRectF lineRect = rect;
            lineRect.setBottom(lineRect.bottom() + Margin);
            drawZigZagLine(&painter, lineRect);
            painter.setPen(spellingPen);
            ++queryCount;
        }
        ++spellCount;
        drawZigZagLine(&painter, rect);
    }
    if (spellCount || queryCount) {
        QFontMetricsF fm = painter.fontMetrics();
        double x = Margin;
        double Y0 = fm.height();
        double Y1 = Y0 + (Margin * 2);
        QString text = QString::number(spellCount);
        painter.setPen(Qt::darkGray);
        painter.drawText(x, Y0, text);
        painter.setPen(spellingPen);
        drawZigZagLine(&painter, QRectF(QPointF(x, Y1 + Margin),
            QPointF(fm.width(text) + Margin, Y1 + Margin)));
        x += fm.width(text) + Margin;
        if (queryCount) {
            painter.setPen(Qt::gray);
            text = QChar(0x2022);
            painter.setPen(Qt::darkGray);
            painter.drawText(x, Y0, text);
            x += fm.width(text) + Margin;
            text = QString::number(queryCount);
            painter.drawText(x, Y0, text);
            double x1 = x + fm.width(text);
            painter.setPen(queryWordPen);
            drawZigZagLine(&painter, QRectF(QPointF(x, Y1 + Margin),
                        QPointF(x1, Y1 + Margin)));
        }
    }
}


void MainWindow::drawZigZagLine(QPainter *painter, const QRectF &rect)
{
    QPointF points[PointCount];
    const double Y0 = rect.bottom();
    const double Y1 = Y0 - ZigZag;
    for (int x = rect.x(); x + ZigZag < rect.right(); x += ZigZagX2) {
        int count = PointCount;
        points[0] = QPointF(x, Y0);
        points[1] = QPointF(x + ZigZag, Y1);
        if (x + ZigZagX2 < rect.right())
            points[2] = QPointF(x + ZigZagX2, Y0);
        else
            --count;
        painter->drawPolyline(points, count);
    }
}


bool MainWindow::validWord(const QString &word)
{
    QTextStream err(stderr);
    try {
        if (dictionary->check(std::string(word.toUtf8().constData())))
            return true;
        if (word.endsWith("-"))
            return dictionary->check(std::string(
                word.left(word.length() - 1).toUtf8().constData()));
    } catch (enchant::Exception &ex) {
        err << qApp->applicationName() << ": " << ex.what() << "\n";
    }
    return false;
}


bool MainWindow::validExceptionalWord(const QString &word_)
{
    const QString word = word_.toLower();
    if (word.startsWith("www.") || word.endsWith(".html"))
        return true;
    // Can't match U+... because wordsForText() splits on +
    QRegExp hexRx("(?:\\u|0x)[\\da-f]+");
    if (hexRx.exactMatch(word))
        return true;
    if (dictionaryLanguage.startsWith("en") &&
        (word == "i.e" || word == "e.g"))
        return true;
    return false;
}


QMultiMap<QPair<int, int>, TextBox> MainWindow::getTexts()
{
    QMultiMap<QPair<int, int>, TextBox> textsForYx;
    if (page) {
        foreach (Poppler::TextBox *box, page->textList()) {
            const QString text = box->text().trimmed();
            if (!text.isEmpty()) {
                const QStringList words = wordsForText(text);
                const QRectF &rect = box->boundingBox();
                int j = 0;
                for (int i = 0; i < words.count(); ++i) {
                    const QString &word = words.at(i);
                    QRectF wordRect = rect;
                    j = text.indexOf(word, j);
                    if (j > -1) {
                        wordRect.setLeft(box->charBoundingBox(j).left());
                        wordRect.setRight(box->charBoundingBox(
                                j + word.length() - 1).right());
                        j += word.length();
                    }
                    textsForYx.insert(qMakePair(
                            // Allow for small baseline variations
                            clamped(wordRect.bottom()),
                            qRound(wordRect.x()) + i),
                            TextBox(wordRect, word));
                    // The + i is to reduce double word false positives
                }
            }
        }
    }
    return textsForYx;
}


QStringList MainWindow::wordsForText(const QString &text)
{
    QString word = trim(text);
    QStringList words;
    if (word.length() == 0)
        return words;

    QString result;
    for (int i = 0; i < word.length(); ++i) {
        const QChar c = word.at(i);
        // If a switch case matches we use continue to return to the
        // loop: not break. If no switch case matches the following if
        // statement is used.
        switch (c.unicode()) {
            // Ligatures
            case 0x0132: result += "IJ"; continue;
            case 0x0133: result += "ij"; continue;
            case 0x0152: result += "OE"; continue;
            case 0x0153: result += "oe"; continue;
            case 0xFB00: result += "ff"; continue;
            case 0xFB01: result += "fi"; continue;
            case 0xFB02: result += "fl"; continue;
            case 0xFB03: result += "ffi"; continue;
            case 0xFB04: result += "ffl"; continue;
            case 0xFB05: result += "st"; continue;
            case 0xFB06: result += "st"; continue;
            // Hyphens
            case 0xAD:   // fallthrough (soft-hyphen)
            case 0x2D:   // fallthrough (hyphen-minus, '-')
            case 0x2010: // fallthrough (hyphen)
            case 0x2011: // fallthrough (non-continueing hyphen)
            case 0x2043: // (hyphen-bullet)
                    result = trim(result);
                    if (!result.isEmpty()) {
                        words << result + "-";
                        result.clear();
                    }
                    continue;
            // Dashes and slashes
            case '/':    // fallthrough
            case '\\':   // fallthrough
            case 0x2013: // fallthrough (en-dash)
            case 0x2014: // (em-dash)
                    result = trim(result);
                    if (!result.isEmpty()) {
                        words << result; // We drop the dash/slash
                        result.clear();
                    }
                    continue;
        }
        if (dictionaryLanguage.startsWith("en")) {
            switch (c.unicode()) {
            case 0xC0:   // fallthrough À
            case 0xC1:   // fallthrough Á
            case 0xC2:   // fallthrough Â
            case 0xC3:   // fallthrough Ã
            case 0xC4:   // fallthrough Ä
            case 0xC5:   result +="A"; continue; // Å
            case 0xC6:   result += "AE"; continue; // Æ
            case 0xC7:   result += "C"; continue; // Ç
            case 0xC8:   // fallthrough È
            case 0xC9:   // fallthrough É
            case 0xCA:   // fallthrough Ê
            case 0xCB:   result += "E"; continue; // Ë
            case 0xCC:   // fallthrough Ì
            case 0xCD:   // fallthrough Í
            case 0xCE:   // fallthrough Î
            case 0xCF:   result += "I"; continue; // Ï
            case 0xD0:   result += "D"; continue; // Ð
            case 0xD1:   result += "N"; continue; // Ñ
            case 0xD2:   // fallthrough Ò
            case 0xD3:   // fallthrough Ó
            case 0xD4:   // fallthrough Ô
            case 0xD5:   // fallthrough Õ
            case 0xD6:   // fallthrough Ö
            case 0xD8:   result += "O"; continue; //Ø
            case 0xD9:   // fallthrough Ù
            case 0xDA:   // fallthrough Ú
            case 0xDB:   // fallthrough Û
            case 0xDC:   result += "U"; continue; // Ü
            case 0xDD:   result += "Y"; continue; // Ý
            case 0xE0:   // fallthrough à
            case 0xE1:   // fallthrough á
            case 0xE2:   // fallthrough â
            case 0xE3:   // fallthrough ã
            case 0xE4:   // fallthrough ä
            case 0xE5:   result += "a"; continue; // å
            case 0xE6:   result += "ae"; continue; // æ
            case 0xE7:   result += "c"; continue; // ç
            case 0xE8:   // fallthrough è
            case 0xE9:   // fallthrough é
            case 0xEA:   // fallthrough ê
            case 0xEB:   result += "e"; continue; // ë
            case 0xEC:   // fallthrough ì
            case 0xED:   // fallthrough í
            case 0xEE:   // fallthrough î
            case 0xEF:   result += "i"; continue; // ï
            case 0xF1:   result += "n"; continue; // ñ
            case 0xF0:   // fallthrough ð
            case 0xF2:   // fallthrough ò
            case 0xF3:   // fallthrough ó
            case 0xF4:   // fallthrough ô
            case 0xF5:   // fallthrough õ
            case 0xF6:   // fallthrough ö
            case 0xF8:   result += "o"; continue; // ø
            case 0xF9:   // fallthrough ù
            case 0xFA:   // fallthrough ú
            case 0xFB:   // fallthrough û
            case 0xFC:   result += "u"; continue; //ü
            case 0xFD:   // fallthrough ý
            case 0xFF:   result += "y"; continue; // ÿ
            }
        }
        if (c == Apostrophe) // for don't, doesn't, etc., enchant expects
            result += '\'';  // a single quote not an apostrophe
        else if (c.isLetterOrNumber() || c == '\'' || c == '_' || c == '.')
            result += c;
        else {
            result = trim(result);
            if (!result.isEmpty()) {
                words << result; // We drop the non-letter and non-digit
                result.clear();
            }
        }
    }
    if (words.isEmpty() || !result.isEmpty())
        words << trim(result);
    return words;
}


QString MainWindow::depluralize(const QString &word)
{
    if (dictionaryLanguage.startsWith("en") &&
        (word.endsWith("'s") || word.endsWith(ApostropheS)))
        return word.left(word.length() - 2);
    return word;
}


QString MainWindow::trim(const QString &word_)
{
    QString word = word_;
    while (word.length() > 0 && !word.at(0).isLetterOrNumber())
        word = word.mid(1); // Trim leading non-word chars
    return rtrim(depluralize(rtrim(word)));
}


QString MainWindow::rtrim(const QString &word_)
{
    QString word = word_;
    while (word.length() > 0 &&
           !(word.at(word.length() - 1).isLetterOrNumber() ||
             word.at(word.length() - 1) == '-')) // Trim trailing non-words
        word.chop(1);
    return word;
}


void addDictionaryLanguage(const char * const language, const char * const,
        const char * const, const char * const, void *user_data)
{
    MainWindow *window = static_cast<MainWindow*>(user_data);
    window->addLanguage(language);
}


void MainWindow::loadDictionary()
{
    QSettings settings;
    dictionaryLanguage = settings.value("Language",
            QLocale::system().name()).toString();
    try {
        enchant::Broker *broker = enchant::Broker::instance();
        broker->list_dicts(addDictionaryLanguage, this);
        if (!dictionaryLanguages.contains(dictionaryLanguage) &&
            !dictionaryLanguages.isEmpty()) {
            QSet<QString>::const_iterator i =
                    dictionaryLanguages.constBegin();
            if (i != dictionaryLanguages.constEnd())
                dictionaryLanguage = *i;
        }
        setLanguage();
    } catch (enchant::Exception &err) {
        QMessageBox::warning(this, tr("%1 — Error")
                .arg(qApp->applicationName()),
                tr("Failed to find any dictionaries: %2").arg(err.what()));
    }
}


void MainWindow::changeLanguage()
{
    QStringList languages = dictionaryLanguages.toList();
    languages.sort();
    int i = 0;
    for ( ; i < languages.count(); ++i)
        if (languages.at(i) == dictionaryLanguage)
            break;
    const QString language = QInputDialog::getItem(this,
            tr("%1 — Set Language").arg(qApp->applicationName()),
            tr("&Language:"), languages, i, false);
    if (!language.isEmpty()) {
        dictionaryLanguage = language;
        setLanguage();
        QPixmapCache::clear(); // May need to repaint all pages
        updatePage();
    }
}


void MainWindow::setLanguage()
{
    try {
        enchant::Broker *broker = enchant::Broker::instance();
        delete dictionary;
        dictionary = broker->request_dict(
                dictionaryLanguage.toStdString());
        languageButton->setText(tr("&Language (%1)...")
                .arg(dictionaryLanguage));
    } catch (enchant::Exception &err) {
        QMessageBox::warning(this, tr("%1 — Error")
                .arg(qApp->applicationName()),
                tr("Failed to load dictionary '%1': %2")
                .arg(dictionaryLanguage).arg(err.what()));
    }
}


QRectF adjustRect(const QRectF rect, double width, double height)
{
    QRectF rectangle(rect);
    double x1, y1, x2, y2;
    rectangle.getCoords(&x1, &y1, &x2, &y2);
    rectangle.setCoords(x1 * width, y1 * height, x2 * width, y2 * height);
    return rectangle;
}


int clamped(const double &x)
{
    // Anything less than 5 produces false-positives
    int y = std::floor(x);
    return y - (y % 5);
}
