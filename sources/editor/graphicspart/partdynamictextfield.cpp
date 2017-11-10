/*
	Copyright 2006-2017 The QElectroTech Team
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
#include "partdynamictextfield.h"
#include "QPropertyUndoCommand/qpropertyundocommand.h"
#include "qetapp.h"
#include "elementscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QFont>
#include <QColor>
#include <QMatrix>

/**
 * @brief PartDynamicTextField::PartDynamicTextField
 * Return if a dynamic text field can import information from the xml definition of a text field
 * @param editor
 * @param parent
 */
bool PartDynamicTextField::canImportFromTextField(const QDomElement &dom_element)
{
	if(dom_element.tagName() != "input")
		return false;
	
	QString tagg = dom_element.attribute("tagg", "none");
	if(tagg == "none")
		return true;
	else
		return false;
}

PartDynamicTextField::PartDynamicTextField(QETElementEditor *editor, QGraphicsItem *parent) :
	QGraphicsTextItem(parent),
	CustomElementPart(editor),
	m_uuid(QUuid::createUuid())
{
	setDefaultTextColor(Qt::black);
	setFont(QETApp::diagramTextsFont(9));
	setText("_");
	setTextFrom(DynamicElementTextItem::UserText);
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIsMovable);
	
}

QString PartDynamicTextField::name() const
{
	return tr("Champ de texte dynamique", "element part name");
}

QString PartDynamicTextField::xmlName() const
{
	return QString("dynamic_text");
}

/**
 * @brief PartDynamicTextField::startUserTransformation
 * @param initial_selection_rect
 * Start the user-induced transformation, provided this primitive is contained
 * within the initial_selection_rect bounding rectangle.
 */
void PartDynamicTextField::startUserTransformation(const QRectF &initial_selection_rect)
{
	Q_UNUSED(initial_selection_rect)
	m_saved_point = pos(); // scene coordinates, no need to mapFromScene()
}

/**
 * @brief PartDynamicTextField::handleUserTransformation
 * @param initial_selection_rect
 * @param new_selection_rect
 * Handle the user-induced transformation from initial_selection_rect to new_selection_rect
 */
void PartDynamicTextField::handleUserTransformation(const QRectF &initial_selection_rect, const QRectF &new_selection_rect)
{
	QPointF new_pos = mapPoints(initial_selection_rect, new_selection_rect, QList<QPointF>() << m_saved_point).first();
	setPos(new_pos);
}

/**
 * @brief PartDynamicTextField::toXml
 * @param document
 * @return 
 */
const QDomElement PartDynamicTextField::toXml(QDomDocument &dom_doc) const
{
	QDomElement root_element = dom_doc.createElement(xmlName());
	
	root_element.setAttribute("x", QString::number(pos().x()));
	root_element.setAttribute("y", QString::number(pos().y()));
	root_element.setAttribute("z", QString::number(zValue()));
	root_element.setAttribute("rotation", QString::number(QET::correctAngle(rotation())));
	root_element.setAttribute("font_size", font().pointSize());
	root_element.setAttribute("uuid", m_uuid.toString());
	root_element.setAttribute("frame", m_frame? "true" : "false");
	

	QMetaEnum me = DynamicElementTextItem::textFromMetaEnum();
	root_element.setAttribute("text_from", me.valueToKey(m_text_from));
	
    QDomElement dom_text = dom_doc.createElement("text");
    dom_text.appendChild(dom_doc.createTextNode(toPlainText()));
    root_element.appendChild(dom_text);
	
		//Info name
	if(!m_info_name.isEmpty())
	{
		QDomElement dom_info_name = dom_doc.createElement("info_name");
		dom_info_name.appendChild(dom_doc.createTextNode(m_info_name));
		root_element.appendChild(dom_info_name);
	}
	
		//Composite text
	if(!m_composite_text.isEmpty())
	{
		QDomElement dom_comp_text = dom_doc.createElement("composite_text");
		dom_comp_text.appendChild(dom_doc.createTextNode(m_composite_text));
		root_element.appendChild(dom_comp_text);
	}
    
		//tagg
	if (!m_tagg.isEmpty())
	{
		QDomElement dom_tagg = dom_doc.createElement("tagg");
		dom_tagg.appendChild(dom_doc.createTextNode(m_tagg));
		root_element.appendChild(dom_tagg);
	}
	
		//Color
	if(color() != QColor(Qt::black))
	{
		QDomElement dom_color = dom_doc.createElement("color");
		dom_color.appendChild(dom_doc.createTextNode(color().name()));
		root_element.appendChild(dom_color);
	}
    
    return root_element;
}

/**
 * @brief PartDynamicTextField::fromXml
 * @param element
 */
void PartDynamicTextField::fromXml(const QDomElement &dom_elmt)
{
	if (dom_elmt.tagName() != xmlName()) {
		qDebug() << "PartDynamicTextField::fromXml : Wrong tagg name";
		return;
	}
	
	QGraphicsTextItem::setPos(dom_elmt.attribute("x", QString::number(0)).toDouble(),
							  dom_elmt.attribute("y", QString::number(0)).toDouble());
	setZValue(dom_elmt.attribute("z", QString::number(zValue())).toDouble());
	QGraphicsTextItem::setRotation(dom_elmt.attribute("rotation", QString::number(0)).toDouble());
	setFont(QETApp::diagramTextsFont(dom_elmt.attribute("font_size", QString::number(9)).toInt()));
	m_uuid = QUuid(dom_elmt.attribute("uuid", QUuid::createUuid().toString()));
	m_frame = dom_elmt.attribute("frame", "false") == "true"? true : false;
	
	QMetaEnum me = DynamicElementTextItem::textFromMetaEnum();
	m_text_from = DynamicElementTextItem::TextFrom(me.keyToValue(dom_elmt.attribute("text_from").toStdString().data()));

		//Text
    QDomElement dom_text = dom_elmt.firstChildElement("text");
	if (!dom_text.isNull())
	{
		m_text = dom_text.text();
		setPlainText(m_text);
	}
	
		//Info name
	QDomElement dom_info_name = dom_elmt.firstChildElement("info_name");
	if(!dom_info_name.isNull())
		m_info_name = dom_info_name.text();
	
		//Composite text
	QDomElement dom_comp_text = dom_elmt.firstChildElement("composite_text");
	if(!dom_comp_text.isNull())
		m_composite_text = dom_comp_text.text();
    
		//tagg
    QDomElement dom_tagg = dom_elmt.firstChildElement("tagg");
	if (!dom_tagg.isNull())
		m_tagg = dom_tagg.text();

		//Color
	QDomElement dom_color = dom_elmt.firstChildElement("color");
	if(!dom_color.isNull())
		setColor(QColor(dom_color.text()));
}

/**
 * @brief PartDynamicTextField::fromTextFieldXml
 * Setup this text from the xml definition of a text field (The xml tagg of a text field is "input");
 * @param dom_element
 */
void PartDynamicTextField::fromTextFieldXml(const QDomElement &dom_element)
{
	if(canImportFromTextField(dom_element))
	{
		setFont(QETApp::diagramTextsFont(dom_element.attribute("size", QString::number(9)).toInt()));
		setTextFrom(DynamicElementTextItem::UserText);
		setText(dom_element.attribute("text", "_"));
		QGraphicsTextItem::setRotation(dom_element.attribute("rotation", "0").toDouble());
		
			//the origin transformation point of PartDynamicTextField is the top left corner, no matter the font size
			//The origin transformation point of PartTextField is the middle of left edge, and so by definition, change with the size of the font
			//We need to use a QMatrix to find the pos of this text from the saved pos of text item 
		QMatrix matrix;
			//First make the rotation
		matrix.rotate(dom_element.attribute("rotation", "0").toDouble());
		QPointF pos = matrix.map(QPointF(0, -boundingRect().height()/2));
		matrix.reset();
			//Second translate to the pos
		matrix.translate(dom_element.attribute("x", QString::number(0)).toDouble(),
						 dom_element.attribute("y", QString::number(0)).toDouble());
		QGraphicsTextItem::setPos(matrix.map(pos));
	}
}

/**
 * @brief PartDynamicTextField::textFrom
 * @return what the final text is created from.
 */
DynamicElementTextItem::TextFrom PartDynamicTextField::textFrom() const {
	return m_text_from;
}

/**
 * @brief PartDynamicTextField::setTextFrom
 * Set the final text is created from.
 * @param text_from
 */
void PartDynamicTextField::setTextFrom(DynamicElementTextItem::TextFrom text_from)
{
	m_text_from = text_from;
	emit textFromChanged(m_text_from);
}

/**
 * @brief PartDynamicTextField::tagg
 * @return the tagg of this text
 */
QString PartDynamicTextField::tagg() const {
	return m_tagg;
}

/**
 * @brief PartDynamicTextField::setTagg
 * set the taggof this text
 * @param tagg
 */
void PartDynamicTextField::setTagg(const QString &tagg)
{
	m_tagg = tagg;
	emit taggChanged(m_tagg);
}

/**
 * @brief PartDynamicTextField::text
 * @return the text of this text
 */
QString PartDynamicTextField::text() const {
	return m_text;
}

/**
 * @brief PartDynamicTextField::setText
 * Set the text of this text
 * @param text
 */
void PartDynamicTextField::setText(const QString &text)
{
	m_text = text;
	setPlainText(m_text);
	emit textChanged(m_text);
}

void PartDynamicTextField::setInfoName(const QString &info_name)
{
	m_info_name = info_name;
	emit infoNameChanged(m_info_name);
}

/**
 * @brief PartDynamicTextField::infoName
 * @return the info name of this text
 */
QString PartDynamicTextField::infoName() const{
	return m_info_name;
}

/**
 * @brief PartDynamicTextField::setCompositeText
 * Set the composite text of this text item to @text
 * @param text
 */
void PartDynamicTextField::setCompositeText(const QString &text)
{
	m_composite_text = text;
	emit compositeTextChanged(m_composite_text);
}

/**
 * @brief PartDynamicTextField::compositeText
 * @return the composite text of this text
 */
QString PartDynamicTextField::compositeText() const{
	return m_composite_text;
}

/**
 * @brief PartDynamicTextField::setColor
 * @param color set text color to color
 */
void PartDynamicTextField::setColor(QColor color)
{
	setDefaultTextColor(color);
	emit colorChanged(color);
}

/**
 * @brief PartDynamicTextField::color
 * @return The color of this text
 */
QColor PartDynamicTextField::color() const {
	return defaultTextColor();
}

/**
 * @brief PartDynamicTextField::setFontSize
 * @param s
 */
void PartDynamicTextField::setFontSize(int s) {
	setFont(QETApp::diagramTextsFont(s));
	emit fontSizeChanged(s);
}

int PartDynamicTextField::fontSize() const {
	return font().pointSize();
}

void PartDynamicTextField::setFrame(bool frame)
{
	m_frame = frame;
	update();
	emit frameChanged(m_frame);
}

bool PartDynamicTextField::frame() const
{
	return m_frame;
}

/**
 * @brief PartDynamicTextField::mouseMoveEvent
 * @param event
 */
void PartDynamicTextField::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if((event->buttons() & Qt::LeftButton) && (flags() & QGraphicsItem::ItemIsMovable))
    {
        QPointF pos = event->scenePos() + (m_origine_pos - event->buttonDownScenePos(Qt::LeftButton));
        event->modifiers() == Qt::ControlModifier ? setPos(pos) : setPos(elementScene()->snapToGrid(pos));
    }
    else
		QGraphicsObject::mouseMoveEvent(event);
}

/**
 * @brief PartDynamicTextField::mousePressEvent
 * @param event
 */
void PartDynamicTextField::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
        m_origine_pos = this->pos();

	QGraphicsObject::mousePressEvent(event);
}

/**
 * @brief PartDynamicTextField::mouseReleaseEvent
 * @param event
 */
void PartDynamicTextField::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if((event->button() & Qt::LeftButton) && (flags() & QGraphicsItem::ItemIsMovable) && m_origine_pos != pos())
    {
        QPropertyUndoCommand *undo = new QPropertyUndoCommand(this, "pos", QVariant(m_origine_pos), QVariant(pos()));
        undo->setText(tr("Déplacer un champ texte"));
        undo->enableAnimation();
        elementScene()->undoStack().push(undo);
    }

	QGraphicsObject::mouseReleaseEvent(event);
}

/**
 * @brief PartDynamicTextField::itemChange
 * @param change
 * @param value
 * @return 
 */
QVariant PartDynamicTextField::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
	if (change == QGraphicsItem::ItemPositionHasChanged || change == QGraphicsItem::ItemSceneHasChanged)
		updateCurrentPartEditor();
	else if ((change == QGraphicsItem::ItemSelectedHasChanged) && (value.toBool() == true))
		updateCurrentPartEditor();
	
	return(QGraphicsTextItem::itemChange(change, value));
}

void PartDynamicTextField::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QGraphicsTextItem::paint(painter, option, widget);
	
	if (m_frame)
	{
		painter->save();
		painter->setFont(QETApp::diagramTextsFont(fontSize()));
		
			//Adjust the thickness according to the font size, 
		qreal w=0.3;
		if(fontSize() >= 5)
		{
			w = (qreal)fontSize()*0.1;
			if(w > 2.5)
				w = 2.5;
		}
		
		QPen pen;
		pen.setColor(color());
		pen.setWidthF(w);
		painter->setPen(pen);
		painter->setRenderHint(QPainter::Antialiasing);
		
			//Get the bounding rectangle of the text 
		QRectF text_bounding = painter->boundingRect(boundingRect(), toPlainText());
			//Center text_bounding in the bounding rect of this
		text_bounding.moveTop((boundingRect().height()-text_bounding.height())/2);
		text_bounding.moveLeft((boundingRect().width() - text_bounding.width())/2);
			//adjust only for better visual
		text_bounding.adjust(-2,0,2,0); 
		
			//Adjust the rounding of the rectangle according to the size of the font
		qreal ro = (qreal)fontSize()/3;
		painter->drawRoundedRect(text_bounding, ro, ro);
		
		painter->restore();
	}
}
