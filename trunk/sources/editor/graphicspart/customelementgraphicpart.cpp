/*
	Copyright 2006-2015 The QElectroTech Team
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "customelementgraphicpart.h"

/**
 * @brief CustomElementGraphicPart::CustomElementGraphicPart
 * Default constructor.
 * By default, item is selectable, send geometry change (Qt > 4.6),
 * accept mouse left button and accept hover event
 * @param editor QETElement editor that belong this.
 */
CustomElementGraphicPart::CustomElementGraphicPart(QETElementEditor *editor, QGraphicsItem *parent) :
	QGraphicsObject (parent),
	CustomElementPart(editor),
	m_hovered (false),
	_linestyle(NormalStyle),
	_lineweight(NormalWeight),
	_filling(NoneFilling),
	_color(BlackColor),
	_antialiased(false)
{
	setFlags(QGraphicsItem::ItemIsSelectable);
#if QT_VERSION >= 0x040600
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
	setAcceptedMouseButtons(Qt::LeftButton);
	setAcceptHoverEvents(true);
}

/**
 * @brief CustomElementGraphicPart::~CustomElementGraphicPart
 * Destructor
 */
CustomElementGraphicPart::~CustomElementGraphicPart() {}

/**
 * @brief CustomElementGraphicPart::drawCross
 * Draw a cross at pos center
 * @param center : center of cross
 * @param painter : painter to use for draw cross,
 * the painter state is restored at end of this method.
 */
void CustomElementGraphicPart::drawCross(const QPointF &center, QPainter *painter)
{
	painter -> save();
	painter -> setRenderHint(QPainter::Antialiasing, false);
	painter -> setPen((painter -> brush().color() == QColor(Qt::black) && painter -> brush().isOpaque()) ? Qt::yellow : Qt::blue);
	painter -> drawLine(QLineF(center.x() - 2.0, center.y(), center.x() + 2.0, center.y()));
	painter -> drawLine(QLineF(center.x(), center.y() - 2.0, center.x(), center.y() + 2.0));
	painter -> restore();
}

/**
 * @brief CustomElementGraphicPart::setLineStyle
 * Set line style to ls
 * @param ls
 */
void CustomElementGraphicPart::setLineStyle(const LineStyle ls)
{
	if (_linestyle == ls) return;
	_linestyle = ls;
	update();
}

/**
 * @brief CustomElementGraphicPart::setLineWeight
 * Set line weight to lw
 * @param lw
 */
void CustomElementGraphicPart::setLineWeight(const LineWeight lw)
{
	if (_lineweight == lw) return;
	_lineweight = lw;
	update();
}

/**
 * @brief CustomElementGraphicPart::penWeight
 * @return the weight of pen
 */
qreal CustomElementGraphicPart::penWeight() const
{
	if      (_lineweight == NoneWeight || _lineweight == ThinWeight) return 0;
	else if (_lineweight == NormalWeight) return 1;
	else if (_lineweight == UltraWeight)  return 2;
	else if (_lineweight == BigWeight)    return 5;
	return 1;
}

/**
 * @brief CustomElementGraphicPart::setFilling
 * Set filling to f
 * @param f
 */
void CustomElementGraphicPart::setFilling(const Filling f)
{
	if (_filling == f) return;
	_filling = f;
	update();
}

/**
 * @brief CustomElementGraphicPart::setColor
 * Set color to c
 * @param c
 */
void CustomElementGraphicPart::setColor(const Color c)
{
	if (_color == c) return;
	_color = c;
	update();
}

/**
 * @brief CustomElementGraphicPart::setAntialiased
 * Set antialias to b
 * @param b
 */
void CustomElementGraphicPart::setAntialiased(const bool b)
{
	if (_antialiased == b) return;
	_antialiased = b;
	update();
}

/**
 * @brief CustomElementGraphicPart::stylesToXml
 * Write the curent style to xml element.
 * The style are stored like this:
 * name-of-style:value;name-of-style:value
 * Each style separate by ; and name-style/value are separate by :
 * @param qde : QDOmElement used to write the style.
 */
void CustomElementGraphicPart::stylesToXml(QDomElement &qde) const
{
	QString css_like_styles;
	
	css_like_styles += "line-style:";
	if      (_linestyle == DashedStyle)     css_like_styles += "dashed";
	else if (_linestyle == DottedStyle)     css_like_styles += "dotted";
	else if (_linestyle == DashdottedStyle) css_like_styles += "dashdotted";
	else if (_linestyle == NormalStyle)     css_like_styles += "normal";
	
	css_like_styles += ";line-weight:";
	if      (_lineweight == NoneWeight)   css_like_styles += "none";
	else if (_lineweight == ThinWeight)   css_like_styles += "thin";
	else if (_lineweight == NormalWeight) css_like_styles += "normal";
	else if (_lineweight == UltraWeight)  css_like_styles += "hight";
	else if (_lineweight == BigWeight)    css_like_styles += "eleve";


	css_like_styles += ";filling:";
	if      (_filling == NoneFilling)  css_like_styles += "none";
	else if (_filling == BlackFilling) css_like_styles += "black";
	else if (_filling == WhiteFilling) css_like_styles += "white";
	else if (_filling == GreenFilling) css_like_styles += "green";
	else if (_filling == BlueFilling)  css_like_styles += "blue";
	else if (_filling == RedFilling)   css_like_styles += "red";
	else if (_filling == GrayFilling)  css_like_styles += "gray";

	
	css_like_styles += ";color:";
	if      (_color == WhiteColor) css_like_styles += "white";
	else if (_color == BlackColor) css_like_styles += "black";
	else if (_color == GreenColor) css_like_styles += "green";
	else if (_color == RedColor)   css_like_styles += "red";
	else if (_color == BlueColor)  css_like_styles += "blue";
	else if (_color == GrayColor)  css_like_styles += "gray";

	qde.setAttribute("style", css_like_styles);
	qde.setAttribute("antialias", _antialiased ? "true" : "false");
}


/**
 * @brief CustomElementGraphicPart::stylesFromXml
 * Read the style used by this, from a xml element.
 * @param qde : QDomElement used to read the style
 */
void CustomElementGraphicPart::stylesFromXml(const QDomElement &qde)
{
	resetStyles();
	
		//Get the list of pair style/value
	QStringList styles = qde.attribute("style").split(";", QString::SkipEmptyParts);
	
		//Check each pair of style
	QRegExp rx("^\\s*([a-z-]+)\\s*:\\s*([a-z-]+)\\s*$");
	foreach (QString style, styles)
	{
		if (!rx.exactMatch(style)) continue;
		QString style_name = rx.cap(1);
		QString style_value = rx.cap(2);
		if (style_name == "line-style")
		{
			if      (style_value == "dashed")     _linestyle = DashedStyle;
			else if (style_value == "dotted")     _linestyle = DottedStyle;
			else if (style_value == "dashdotted") _linestyle = DashdottedStyle;
			else if (style_value == "normal")     _linestyle = NormalStyle;
		}
		else if (style_name == "line-weight")
		{
			if      (style_value == "none")   _lineweight = NoneWeight;
			else if (style_value == "thin")   _lineweight = ThinWeight;
			else if (style_value == "normal") _lineweight = NormalWeight;
			else if (style_value == "hight")  _lineweight = UltraWeight;
			else if (style_value == "eleve")  _lineweight = BigWeight;
		}
		else if (style_name == "filling")
		{
			if      (style_value == "white") _filling = WhiteFilling;
			else if (style_value == "black") _filling = BlackFilling;
			else if (style_value == "red")   _filling = RedFilling;
			else if (style_value == "green") _filling = GreenFilling;
			else if (style_value == "blue")  _filling = BlueFilling;
			else if (style_value == "gray")  _filling = GrayFilling;
			else if (style_value == "none")  _filling = NoneFilling;
		}
		else if (style_name == "color")
		{
			if      (style_value == "black") _color = BlackColor;
			else if (style_value == "white") _color = WhiteColor;
			else if (style_value == "green") _color = GreenColor;
			else if (style_value == "red")   _color = RedColor;
			else if (style_value == "blue")  _color = BlueColor;
			else if (style_value == "gray")  _color = GrayColor;
		}
	}
		//Get antialiasing
	_antialiased = qde.attribute("antialias") == "true";
}


/**
 * @brief CustomElementGraphicPart::resetStyles
 * Reset the curent style to default,
 * same style of default constructor
 */
void CustomElementGraphicPart::resetStyles()
{
	_linestyle = NormalStyle;
	_lineweight = NormalWeight;
	_filling = NoneFilling;
	_color = BlackColor;
	_antialiased = false;
}

/**
 * @brief CustomElementGraphicPart::applyStylesToQPainter
 * Apply the current style to the QPainter
 * @param painter
 */
void CustomElementGraphicPart::applyStylesToQPainter(QPainter &painter) const
{
		//Get the pen and brush
	QPen pen = painter.pen();
	QBrush brush = painter.brush();
	
		//Apply pen style
	if      (_linestyle == DashedStyle)     pen.setStyle(Qt::DashLine);
	else if (_linestyle == DashdottedStyle) pen.setStyle(Qt::DashDotLine);
	else if (_linestyle == DottedStyle)     pen.setStyle(Qt::DotLine);
	else if (_linestyle == NormalStyle)     pen.setStyle(Qt::SolidLine);
	
		//Apply pen width
	if      (_lineweight == NoneWeight)   pen.setColor(QColor(0, 0, 0, 0));
	else if (_lineweight == ThinWeight)   pen.setWidth(0);
	else if (_lineweight == NormalWeight) pen.setWidthF(1.0);
	else if (_lineweight == UltraWeight)  pen.setWidthF(2.0);
	else if (_lineweight == BigWeight)    pen.setWidthF(5.0);

		//Apply brush color
	if (_filling == NoneFilling) brush.setStyle(Qt::NoBrush);
	else
	{
		brush.setStyle(Qt::SolidPattern);
		if (_filling == BlackFilling)      brush.setColor(Qt::black);
		else if (_filling == WhiteFilling) brush.setColor(Qt::white);
		else if (_filling == GreenFilling) brush.setColor(Qt::green);
		else if (_filling == RedFilling)   brush.setColor(Qt::red);
		else if (_filling == BlueFilling)  brush.setColor(Qt::blue);
		else if (_filling == GrayFilling)  brush.setColor(Qt::gray);
	}
	
		//Apply pen color
	if      (_color == WhiteColor) pen.setColor(QColor(255, 255, 255, pen.color().alpha()));
	else if (_color == BlackColor) pen.setColor(QColor(  0,   0,   0, pen.color().alpha()));
	else if (_color == GreenColor) pen.setColor(QColor(Qt::green));
	else if (_color == RedColor)   pen.setColor(QColor(Qt::red));
	else if (_color == BlueColor)  pen.setColor(QColor(Qt::blue));
	else if (_color == GrayColor)  pen.setColor(QColor(Qt::gray));
	
		//Apply antialiasing
	painter.setRenderHint(QPainter::Antialiasing,          _antialiased);
	painter.setRenderHint(QPainter::TextAntialiasing,      _antialiased);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, _antialiased);
	
	painter.setPen(pen);
	painter.setBrush(brush);
}

/**
 * @brief CustomElementGraphicPart::drawShadowShape
 * Draw a transparent blue shadow arround the shape of this item.
 * The QPainterPathStroker used to draw shadows have a width of SHADOWS_HEIGHT
 * Be carefull if penWeight of this item is to 0 the outline of strock is bigger of 0.5
 * @param painter : painter to use for draw this shadows
 */
void CustomElementGraphicPart::drawShadowShape(QPainter *painter)
{
		//@FIXME if pen weight is 0, the strock outline is SHADOWS_HEIGHT/2 + 0.5
		//may be because shape have no line weight
	QPainterPathStroker strock;
	strock.setWidth(SHADOWS_HEIGHT);
	strock.setJoinStyle(Qt::RoundJoin);

	painter->save();
	QColor color(Qt::darkBlue);
	color.setAlpha(50);
	painter -> setBrush (QBrush (color));
	painter -> setPen   (Qt::NoPen);
	painter -> drawPath (strock.createStroke(shape()));
	painter -> restore  ();
}

/**
 * @brief CustomElementGraphicPart::itemChange
 * Reimplemented from QGraphicsObject.
 * If the item position change call updateCurrentPartEditor()
 * the change is always send to QGraphicsObject
 * @param change
 * @param value
 * @return the returned value of QGraphicsObject::itemChange
 */
QVariant CustomElementGraphicPart::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (scene())
		if (change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemPositionHasChanged)
			updateCurrentPartEditor();

	return(QGraphicsObject::itemChange(change, value));
}

/**
 * @brief CustomElementGraphicPart::hoverEnterEvent
 * Reimplemented from QGraphicsObject.
 * Set m_hovered to true
 * @param event
 */
void CustomElementGraphicPart::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	m_hovered = true;
	QGraphicsObject::hoverEnterEvent(event);
}

/**
 * @brief CustomElementGraphicPart::hoverLeaveEvent
 * Reimplemented from QGraphicsObject.
 * Set m_hovered to false
 * @param event
 */
void CustomElementGraphicPart::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	m_hovered = false;
	QGraphicsObject::hoverLeaveEvent(event);
}
