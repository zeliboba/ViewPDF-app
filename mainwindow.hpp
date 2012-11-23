#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP
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

#include "annotation.hpp"
#include "textbox.hpp"
#include <QHash>
#include <QMainWindow>

namespace enchant {
    class Dict;
}
class QDockWidget;
class QLabel;
class QPushButton;
class QScrollArea;
class QSpinBox;
class QTextBrowser;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &filename, int number, QWidget *parent=0);
    ~MainWindow();

    void addLanguage(const QString &language)
        { dictionaryLanguages << language; }

protected:
    void closeEvent(QCloseEvent*);

private slots:
    void fileOpen(const QString &filename=QString());
    void about();
    void help();
    void addWord();
    void removeWord();
    void updateUi();
    void updatePage();
    void controlTopLevelChanged(bool floating);
    void spellingTopLevelChanged(bool floating);
    void annotationsTopLevelChanged(bool floating);
    QPair<QString, QList<RectDetails> > getAnnotations();
    void loadDictionary();
    void changeLanguage();

private:
    void createWidgets();
    void createLayouts();
    void createConnections();
    void loadPdf();
    QString cacheKey() const;
    AnnotationDetails getAnnotationDetails(
            Poppler::Annotation *annotation);
    QMultiMap<QPair<int, int>, TextBox> getTexts();
    void clear();
    void loadWords();
    void saveWords(bool report=true);
    void addAnnotations(QImage *image, const int number);
    void addSpellings(QImage *image, const int number);
    void setLanguage();
    bool validWord(const QString &word);
    bool validExceptionalWord(const QString &word);
    QStringList wordsForText(const QString &text);
    QString depluralize(const QString &word);
    QString trim(const QString &word_);
    QString rtrim(const QString &word_);
    void drawZigZagLine(QPainter *painter, const QRectF &rect);

    QLabel *view;
    QScrollArea *area;
    Qt::DockWidgetArea controlDockArea;
    QDockWidget *controlDockWidget;
    QPushButton *fileOpenButton;
    QLabel *zoomLabel;
    QSpinBox *zoomSpinBox;
    QLabel *pageLabel;
    QSpinBox *pageSpinBox;
    QPushButton *aboutButton;
    QPushButton *helpButton;
    QPushButton *quitButton;
    QLabel *statusLabel;
    Qt::DockWidgetArea spellingDockArea;
    QDockWidget *spellingDockWidget;
    QPushButton *addWordButton;
    QPushButton *removeWordButton;
    QPushButton *languageButton;
    Qt::DockWidgetArea annotationsDockArea;
    QDockWidget *annotationsDockWidget;
    QTextBrowser *annotationsView;

    QString currentPath;
    QString fileName;
    PdfDocument pdf;
    PdfPage page;
    QHash<int, QPair<QString, QList<RectDetails> > > annotationsForPage;
    QHash<int, QMultiMap<QPair<int, int>, TextBox> > textsForPage;
    QSet<QString> words;
    QString wordsFilename;
    QSet<QString> dictionaryLanguages;
    QString dictionaryLanguage;
    enchant::Dict *dictionary; // using a shared pointer doesn't work
};


QRectF adjustRect(const QRectF rect, double width, double height);
int clamped(const double &x);


#endif // MAINWINDOW_HPP
