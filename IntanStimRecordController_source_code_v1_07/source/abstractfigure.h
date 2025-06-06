//  ------------------------------------------------------------------------
//
//  This file is part of the Intan Technologies RHS2000 Interface
//  Version 1.01
//  Copyright (C) 2013-2017 Intan Technologies
//
//  ------------------------------------------------------------------------
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ABSTRACTFIGURE_H
#define ABSTRACTFIGURE_H

#include <QWidget>
#include "stimparameters.h"

/* Abstract Figure is a superclass that DigFigure, StimFigure, and AnOutFigure inherit from */
/* Contains general functions that are inherited, and virtual functions that are reimplemented uniquely for each class */

class AbstractFigure : public QWidget
{
    Q_OBJECT

protected:
    //explicit AbstractFigure(QWidget *parent = 0);
    void generalSetup(StimParameters *stimParameters, QWidget*);
    void generalRedraw();
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent* event);
    void drawDashedHorizontalLine(QPainter &painter, int xLeft, int xRight, int y, QColor color, int dashSize = 4);
    QRect drawHorizontalArrow(QPainter &painter, int xLeft, int xRight, int y, QColor color, QString label = 0, bool labelTop = true);
    void drawStimPulse(QPainter &painter, int xLeft, int xRight, int y, int yMaxAmplitude);
    QRect drawVerticalArrow(QPainter &painter, int x, int yTop, int yBottom, QColor color, QString label, bool labelLeft);
    QRect drawDiagonalArrow(QPainter &painter, int x1, int y1, int x2, int y2, QColor color, QString label, bool labelLeft);
    
    /* Colors used to draw the figure */
    const QColor backgroundColor = Qt::white;
    const QColor frameColor = Qt::darkGray;
    const QColor currentColor = Qt::darkGray;
    const QColor currentColorHL = Qt::darkRed;
    const QColor durationLabelColor = Qt::darkGray;
    const QColor durationLabelColorHL = Qt::red;
    const QColor amplitudeLabelColor = Qt::darkGray;
    const QColor amplitudeLabelColorHL = Qt::red;
    const QColor ampSettleColor = Qt::darkGray;
    const QColor ampSettleColorHL = Qt::darkYellow;
    const QColor chargeRecovColor = Qt::darkGray;
    const QColor chargeRecovColorHL = Qt::darkGreen;

    /* Sizes used to draw the figure */
    const int figureXSize = 320;
    const int figureYSize = 250; 
    const int ArrowSize = 4;

    /* Variables used to draw the figure */
    QPixmap pixmap;
    QRect frame;

    /* Access to the StimParameters object from any function in this class or any inherited class */
    StimParameters* parameters;

    /* Boolean variables used to store the state of the figure and draw it */
    bool highlitFirstPhaseDuration;
    bool highlitPostTriggerDelay;
    bool highlitPulseTrainPeriod;
    bool highlitRefractoryPeriod;
    bool highlitStimTrace;
    bool highlitSecondPhaseDuration;
    bool highlitInterphaseDelay;
    bool highlitFirstPhaseAmplitude;
    bool highlitSecondPhaseAmplitude;
    bool highlitPreStimAmpSettle;
    bool highlitPostStimAmpSettle;
    bool highlitPostStimChargeRecovOn;
    bool highlitPostStimChargeRecovOff;
    bool highlitBaselineVoltage;
    bool noAmplitude;

    /* Local variables used to store the state of the figure and draw it */
    StimParameters::StimShapeValues localStimShape;
    StimParameters::StimPolarityValues localStimPolarity;
    StimParameters::PulseOrTrainValues localPulseOrTrain;

    /* Variables accessible to this class and any inherited classes, used to draw the figure */
    int triggerLabelWidth;
    int endLabelWidth;
    int x0;
    int xEnd;
    int xLength;
    QRect stimFrame;
    int xStimBegin;
    int y0;
    QString triggerLabel;
    QString endLabel;

private:
    virtual void uniqueRedraw(QPainter& painter) = 0;

public slots:
    void updateStimShape(int stimShape);
    void updateStimPolarity(int stimPolarity);
    void updatePulseOrTrain(int pulseOrTrain);
    void highlightPostTriggerDelay(bool highlight);
    void highlightPulseTrainPeriod(bool highlight);
    void highlightRefractoryPeriod(bool highlight);
    void highlightStimTrace(bool highlight);
    void highlightFirstPhaseDuration(bool highlight);
};

#endif // ABSTRACTFIGURE_H
