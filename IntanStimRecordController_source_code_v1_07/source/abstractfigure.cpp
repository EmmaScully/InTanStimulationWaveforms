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

#include <QtGui>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#endif

#include "abstractfigure.h"

using namespace std;

/* Pseudo-constructor that sets up the common code between all children */
void AbstractFigure::generalSetup(StimParameters *stimParameters, QWidget*)
{
    //Allow this class and all children to access the StimParameters object
    parameters = stimParameters;

    //Initialize figure-drawing variables to be false, only made true under correct conditions
    highlitFirstPhaseDuration = false;
    highlitPostTriggerDelay = false;
    highlitPulseTrainPeriod = false;
    highlitRefractoryPeriod = false;
    highlitSecondPhaseDuration = false;
    highlitInterphaseDelay = false;
    highlitFirstPhaseAmplitude = false;
    highlitSecondPhaseAmplitude = false;
    highlitPreStimAmpSettle = false;
    highlitPostStimAmpSettle = false;
    highlitPostStimChargeRecovOn = false;
    highlitPostStimChargeRecovOff = false;
    highlitBaselineVoltage = false;
    noAmplitude = false;
    highlitStimTrace = false;

    //Figure settings
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFocusPolicy(Qt::StrongFocus);
}

/* Redraw function that does the common redrawing between all children, and calls the unique redraw function implemented by each child */
void AbstractFigure::generalRedraw()
{
    //Initialize painter
    pixmap = QPixmap(size());
    pixmap.fill();
    QPainter painter(&pixmap);
    painter.initFrom(this);
    frame = rect();
    painter.fillRect(frame, backgroundColor);
    frame.adjust(0, 0, -1, -1);

    //Draw box outline.
    painter.setPen(frameColor);
    painter.drawRect(frame);
    stimFrame = QRect(frame.left() + 10, frame.top() + fontMetrics().height() + 3, frame.width() - 20, frame.height() * 0.7);

    //Initialize variables that will be used in the unique redraw functions
    triggerLabel = "TRIG";
    endLabel = "END";
    triggerLabelWidth = fontMetrics().width(triggerLabel);
    endLabelWidth = fontMetrics().width(endLabel);
    x0 = stimFrame.left() + triggerLabelWidth / 2;
    xEnd = stimFrame.right() - endLabelWidth / 2;
    xLength = xEnd - x0;

    // draw initial I = 0 line
    painter.setPen(highlitStimTrace ? currentColorHL : currentColor);
    xStimBegin = x0 + 0.1 * xLength;

    uniqueRedraw(painter);
    update();
    repaint();
}

/* Public slot that is called when Post Trigger Delay gains or loses focus */
void AbstractFigure::highlightPostTriggerDelay(bool highlight)
{
    highlitPostTriggerDelay = highlight;
    generalRedraw();
}

/* Public slot that is called when Pulse Train gains or loses focus */
void AbstractFigure::highlightPulseTrainPeriod(bool highlight)
{
    highlitPulseTrainPeriod = highlight;
    generalRedraw();
}

/* Public slot that is called when Refractory Period gains or loses focus */
void AbstractFigure::highlightRefractoryPeriod(bool highlight)
{
    highlitRefractoryPeriod = highlight;
    generalRedraw();
}

/* Public slot that is called when enable stim widget is selected or deselected */
void AbstractFigure::highlightStimTrace(bool highlight)
{
    highlitStimTrace = highlight;
    generalRedraw();
}


/* Public slot that is called when First Phase Duration gains or loses focus */
void AbstractFigure::highlightFirstPhaseDuration(bool highlight)
{
    highlitFirstPhaseDuration = highlight;
    generalRedraw();
}

/* Inherited function that allows painting */
void AbstractFigure::paintEvent(QPaintEvent*)
{
    QStylePainter stylePainter(this);
    stylePainter.drawPixmap(0, 0, pixmap);
}

/* Inherited function that allows redrawing */
void AbstractFigure::resizeEvent(QResizeEvent*)
{
    generalRedraw();
}

/* Protected function that allows all children to draw a dashed horizontal line */
void AbstractFigure::drawDashedHorizontalLine(QPainter &painter, int xLeft, int xRight, int y, QColor color, int dashSize)
{
    int xEnd;
    QColor oldColor = painter.pen().color();

    painter.setPen(color);
    for (int x = xLeft; x <= xRight; x += dashSize * 2) {
        xEnd = x + dashSize;
        if (xEnd > xRight) {
            xEnd = xRight;
        }
        painter.drawLine(x, y, xEnd, y);
    }
    painter.setPen(oldColor);
}

/* Protected function that allows all children to draw a labeled horizontal arrow */
QRect AbstractFigure::drawHorizontalArrow(QPainter &painter, int xLeft, int xRight, int y, QColor color, QString label, bool labelTop)
{
    int xLeft1 = xLeft;
    int xRight1 = xRight;

    if (xRight < xLeft) {
        xLeft1 = xRight;
        xRight1 = xLeft;
    }

    QColor oldColor = painter.pen().color();

    painter.setPen(color);
    painter.drawLine(xLeft1 + 1, y, xRight1 - 1, y);
    painter.drawLine(xLeft1 + 1, y, xLeft1 + 1 + ArrowSize, y + ArrowSize);
    painter.drawLine(xLeft1 + 1, y, xLeft1 + 1 + ArrowSize, y - ArrowSize);
    painter.drawLine(xRight1 - 1, y, xRight1 - 1 - ArrowSize, y + ArrowSize);
    painter.drawLine(xRight1 - 1, y, xRight1 - 1 - ArrowSize, y - ArrowSize);
    painter.drawLine(xLeft1, y + ArrowSize, xLeft1, y - ArrowSize);
    painter.drawLine(xRight1, y + ArrowSize, xRight1, y - ArrowSize);

    if (label.length() > 0) {
        int labelWidth = fontMetrics().width(label);
        int labelHeight = fontMetrics().height();
        int yPos = y - 3;
        if (!labelTop) {
            yPos = y + labelHeight - 1;
        }
        painter.drawText((xLeft1 + xRight1 - labelWidth) / 2, yPos, label);
    }

    painter.setPen(oldColor);
    return QRect(xLeft1, y - ArrowSize, xRight1 - xLeft1 + 1, 2 * ArrowSize + 1);
}


/* Protected function that allows all children to draw a labeled vertical arrow */
QRect AbstractFigure::drawVerticalArrow(QPainter &painter, int x, int yTop, int yBottom, QColor color, QString label, bool labelLeft)
{
    int yTop1 = yTop;
    int yBottom1 = yBottom;

    if (yBottom < yTop) {
        yTop1 = yBottom;
        yBottom1 = yTop;
    }

    QColor oldColor = painter.pen().color();

    painter.setPen(color);
    painter.drawLine(x, yTop1 + 1, x, yBottom1 - 1);
    painter.drawLine(x, yTop1 + 1, x + ArrowSize, yTop1 + 1 + ArrowSize);
    painter.drawLine(x, yTop1 + 1, x - ArrowSize, yTop1 + 1 + ArrowSize);
    painter.drawLine(x, yBottom1 - 1, x + ArrowSize, yBottom1 - 1 - ArrowSize);
    painter.drawLine(x, yBottom1 - 1, x - ArrowSize, yBottom1 - 1 - ArrowSize);
    painter.drawLine(x - ArrowSize, yTop1, x + ArrowSize, yTop1);
    painter.drawLine(x - ArrowSize, yBottom1, x + ArrowSize, yBottom1);

    if (label.length() > 0) {
        int labelWidth = fontMetrics().width(label);
        int labelHeight = fontMetrics().height();
        int xPos = x + 3;
        if (labelLeft) {
            xPos = x - labelWidth - 1;
        }
        painter.drawText(xPos, (yTop1 + yBottom1 + labelHeight) / 2, label);
    }

    painter.setPen(oldColor);
    return QRect(x - ArrowSize, yTop1, 2 * ArrowSize + 1, yBottom1 - yTop1 + 1);
}

QRect AbstractFigure::drawDiagonalArrow(QPainter &painter, int x1, int y1, int x2, int y2, QColor color, QString label, bool labelLeft)
{
    int x = x1;

    QColor oldColor = painter.pen().color();

    painter.setPen(color);
    painter.drawLine(x1, y1, x2, y2);
    

    if (label.length() > 0) {
        int labelWidth = fontMetrics().width(label);
        int labelHeight = fontMetrics().height();
        int xPos = x + 3;
        if (labelLeft) {
            xPos = x - labelWidth - 1;
        }
        painter.drawText(xPos, (y2 + y1 + labelHeight) / 2, label);
    }

    painter.setPen(oldColor);
    return QRect(x - ArrowSize, y2, 2 * ArrowSize + 1, y1 - y2 + 1);
}


/* Protected function that allows all children to draw a stim pulse */
void AbstractFigure::drawStimPulse(QPainter &painter, int xLeft, int xRight, int y, int yMaxAmplitude)
{
    QColor oldColor = painter.pen().color();
    int xLength = xRight - xLeft;
    int xMid = xLeft + xLength / 2;
    int x1 = xLeft + xLength / 3;
    int x2 = xLeft + (2 * xLength) / 3;

    QString duration1Text = "D1";
    QString duration2Text = "D2";
    QString delayText = "DP";
    QString amplitude1Text = "A1";
    QString amplitude2Text = "A2";

    bool polarity = (localStimPolarity == StimParameters::PositiveFirst);
    int sign = polarity ? -1 : 1;

    //Depending on the selected stimShape, draw a corresponding pulse
    switch (localStimShape) {

    case StimParameters::Monophasic:
        drawHorizontalArrow(painter, xLeft, xRight, y - yMaxAmplitude - ArrowSize - 3,
                            highlitFirstPhaseDuration ? durationLabelColorHL : durationLabelColor, duration1Text, true);
        if (!noAmplitude)
        {
            drawVerticalArrow(painter, xMid, y, y + sign * yMaxAmplitude,
                            highlitFirstPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude1Text, true);
        }
        painter.drawLine(xLeft, y, xLeft, y + sign * yMaxAmplitude);
        painter.drawLine(xLeft, y + sign * yMaxAmplitude, xRight, y + sign * yMaxAmplitude);
        painter.drawLine(xRight, y + sign * yMaxAmplitude, xRight, y);
        break;

    case StimParameters::Biphasic:
        drawHorizontalArrow(painter, xLeft, xMid, y - yMaxAmplitude - ArrowSize - 3,
                                    highlitFirstPhaseDuration ? durationLabelColorHL : durationLabelColor, duration1Text, true);
        drawHorizontalArrow(painter, xMid, xRight, y - yMaxAmplitude - ArrowSize - 3,
                            highlitSecondPhaseDuration ? durationLabelColorHL : durationLabelColor, duration2Text, true);
        drawVerticalArrow(painter, (xLeft + xMid) / 2, y, y + sign * yMaxAmplitude,
                          highlitFirstPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude1Text, true);
        drawVerticalArrow(painter, (xRight + xMid) / 2, y, y - sign * yMaxAmplitude,
                          highlitSecondPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude2Text, false);
        painter.drawLine(xLeft, y, xLeft, y + sign * yMaxAmplitude);
        painter.drawLine(xLeft, y + sign * yMaxAmplitude, xMid, y + sign * yMaxAmplitude);
        painter.drawLine(xMid, y + sign * yMaxAmplitude, xMid, y - sign * yMaxAmplitude);
        painter.drawLine(xMid, y - sign * yMaxAmplitude, xRight, y - sign * yMaxAmplitude);
        painter.drawLine(xRight, y - sign * yMaxAmplitude, xRight, y);
        break;

    case StimParameters::BiphasicWithInterphaseDelay:
        drawHorizontalArrow(painter, xLeft, x1, y - yMaxAmplitude - ArrowSize - 3,
                            highlitFirstPhaseDuration ? durationLabelColorHL : durationLabelColor, duration1Text, true);
        drawHorizontalArrow(painter, x1, x2, y - yMaxAmplitude - ArrowSize - 3,
                            highlitInterphaseDelay ? durationLabelColorHL : durationLabelColor, delayText, true);
        drawHorizontalArrow(painter, x2, xRight, y - yMaxAmplitude - ArrowSize - 3,
                            highlitSecondPhaseDuration ? durationLabelColorHL : durationLabelColor, duration2Text, true);
        drawVerticalArrow(painter, (xLeft + x1) / 2, y, y + sign * yMaxAmplitude,
                          highlitFirstPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude1Text, true);
        drawVerticalArrow(painter, (xRight + x2) / 2, y, y - sign * yMaxAmplitude,
                          highlitSecondPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude2Text, false);
        
        painter.drawLine(xLeft, y, xLeft, y + sign * yMaxAmplitude);
        painter.drawLine(xLeft, y + sign * yMaxAmplitude, x1, y + sign * yMaxAmplitude);
        painter.drawLine(x1, y + sign * yMaxAmplitude, x1, y);

        if (highlitBaselineVoltage)
        painter.setPen(durationLabelColorHL);
        painter.drawLine(x1, y, x2, y);
        painter.setPen(oldColor);

        painter.drawLine(x2, y, x2, y - sign * yMaxAmplitude);
        painter.drawLine(x2, y - sign * yMaxAmplitude, xRight, y - sign * yMaxAmplitude);
        painter.drawLine(xRight, y - sign * yMaxAmplitude, xRight, y);
        break;

    case StimParameters::Triphasic:
        drawHorizontalArrow(painter, xLeft, x1, y - yMaxAmplitude - ArrowSize - 3,
                            highlitFirstPhaseDuration ? durationLabelColorHL : durationLabelColor, duration1Text, true);
        drawHorizontalArrow(painter, x1, x2, y - yMaxAmplitude - ArrowSize - 3,
                            highlitSecondPhaseDuration ? durationLabelColorHL : durationLabelColor, duration2Text, true);
        drawHorizontalArrow(painter, x2, xRight, y - yMaxAmplitude - ArrowSize - 3,
                            highlitFirstPhaseDuration ? durationLabelColorHL : durationLabelColor, duration1Text, true);
        drawVerticalArrow(painter, (xLeft + x1) / 2, y, y + sign * yMaxAmplitude,
                          highlitFirstPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude1Text, true);
        drawVerticalArrow(painter, (x1 + x2) / 2, y, y - sign * yMaxAmplitude,
                          highlitSecondPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude2Text, true);
        drawVerticalArrow(painter, (xRight + x2) / 2, y, y + sign * yMaxAmplitude,
                          highlitFirstPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude1Text, false);
        painter.drawLine(xLeft, y, xLeft, y + sign * yMaxAmplitude);
        painter.drawLine(xLeft, y + sign * yMaxAmplitude, x1, y + sign * yMaxAmplitude);
        painter.drawLine(x1, y + sign * yMaxAmplitude, x1, y - sign * yMaxAmplitude);
        painter.drawLine(x1, y - sign * yMaxAmplitude, x2, y - sign * yMaxAmplitude);
        painter.drawLine(x2, y - sign * yMaxAmplitude, x2, y + sign * yMaxAmplitude);
        painter.drawLine(x2, y + sign * yMaxAmplitude, xRight, y + sign * yMaxAmplitude);
        painter.drawLine(xRight, y + sign * yMaxAmplitude, xRight, y);
        break;

        /*
    case StimParameters::noStimulation:
        painter.drawLine(xLeft, y, xRight, y);
        break;
        */

    case StimParameters::Triangular:
        drawHorizontalArrow(painter, xLeft, xMid, y - yMaxAmplitude - ArrowSize - 3,
                                    highlitFirstPhaseDuration ? durationLabelColorHL : durationLabelColor, duration1Text, true);
        drawHorizontalArrow(painter, xMid, xRight, y - yMaxAmplitude - ArrowSize - 3,
                            highlitSecondPhaseDuration ? durationLabelColorHL : durationLabelColor, duration2Text, true);
        drawVerticalArrow(painter, (xLeft + xMid) / 2, y, y + sign * yMaxAmplitude,
                          highlitFirstPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude1Text, true);
        drawVerticalArrow(painter, (xRight + xMid) / 2, y, y - sign * yMaxAmplitude,
                          highlitSecondPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude2Text, false);
        
        
        painter.drawLine(xLeft, y, 3*xMid/4, y + sign * yMaxAmplitude);
        painter.drawLine(3*xMid/4, y + sign * yMaxAmplitude, 5*xMid/4, y - sign * yMaxAmplitude);        
        painter.drawLine(5*xMid/4, y - sign * yMaxAmplitude, xRight, y);

        break;


    case StimParameters::Ramp:
        drawHorizontalArrow(painter, xLeft, xMid, y - yMaxAmplitude - ArrowSize - 3,
                                    highlitFirstPhaseDuration ? durationLabelColorHL : durationLabelColor, duration1Text, true);
        drawHorizontalArrow(painter, xMid, xRight, y - yMaxAmplitude - ArrowSize - 3,
                            highlitSecondPhaseDuration ? durationLabelColorHL : durationLabelColor, duration2Text, true);
        drawVerticalArrow(painter, (xLeft + xMid) / 2, y, y + sign * yMaxAmplitude,
                          highlitFirstPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude1Text, true);
        drawVerticalArrow(painter, (xRight + xMid) / 2, y, y - sign * yMaxAmplitude,
                          highlitSecondPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude2Text, false);
       
       //These lines down here actually draw the shape, the ones above are just labels
        painter.drawLine(xLeft, y, xMid, y + sign * yMaxAmplitude);
        painter.drawLine(xMid, y + sign * yMaxAmplitude, xMid, y - sign * yMaxAmplitude);
        painter.drawLine(xMid, y - sign * yMaxAmplitude, xRight, y );
        break;

    case StimParameters::RampWithInterphaseDelay:
        drawHorizontalArrow(painter, xLeft, x1, y - yMaxAmplitude - ArrowSize - 3,
                            highlitFirstPhaseDuration ? durationLabelColorHL : durationLabelColor, duration1Text, true);
        drawHorizontalArrow(painter, x1, x2, y - yMaxAmplitude - ArrowSize - 3,
                            highlitInterphaseDelay ? durationLabelColorHL : durationLabelColor, delayText, true);
        drawHorizontalArrow(painter, x2, xRight, y - yMaxAmplitude - ArrowSize - 3,
                            highlitSecondPhaseDuration ? durationLabelColorHL : durationLabelColor, duration2Text, true);
        drawVerticalArrow(painter, (xLeft + x1) / 2, y, y + sign * yMaxAmplitude,
                          highlitFirstPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude1Text, true);
        drawVerticalArrow(painter, (xRight + x2) / 2, y, y - sign * yMaxAmplitude,
                          highlitSecondPhaseAmplitude ? amplitudeLabelColorHL : amplitudeLabelColor, amplitude2Text, false);
        
        painter.drawLine(xLeft, y, x1, y + sign * yMaxAmplitude);
        painter.drawLine(x1, y + sign * yMaxAmplitude, x1, y);
        

        if (highlitBaselineVoltage)
            painter.setPen(durationLabelColorHL);
        painter.drawLine(x1, y, x2, y);
        painter.setPen(oldColor);

        
        painter.drawLine(x2, y, x2, y - sign * yMaxAmplitude);
        painter.drawLine(x2, y - sign * yMaxAmplitude, xRight, y );
        break;
    }
}

/* Public slot that updates the localPulseOrTrain variable and redraws the figures based when pulseOrTrain changes */
void AbstractFigure::updatePulseOrTrain(int pulseOrTrain)
{
    localPulseOrTrain = (StimParameters::PulseOrTrainValues)pulseOrTrain;
    generalRedraw();
}

/* Public slot that updates the stimShape variable and redraws the figures based when stimShape changes */
void AbstractFigure::updateStimShape(int stimShape)
{
    localStimShape = (StimParameters::StimShapeValues)stimShape;
    generalRedraw();
}

/* Public slot that updates the stimPolarity variable and redraws the figure when stimPolarity changes */
void AbstractFigure::updateStimPolarity(int stimPolarity)
{
    localStimPolarity = (StimParameters::StimPolarityValues)stimPolarity;
    generalRedraw();
}
