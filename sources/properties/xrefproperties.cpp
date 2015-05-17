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
#include "xrefproperties.h"
#include "qetapp.h"

/**
 * @brief XRefProperties::XRefProperties
 * Default Constructor
 */
XRefProperties::XRefProperties()
{
	m_show_power_ctc = true;
	m_display = Cross;
	m_snap_to = Bottom;
	m_prefix_keys << "power" << "delay" << "switch";
}

/**
 * @brief XRefProperties::toSettings
 * Save to settings
 * @param settings: QSettings to use
 * @param prefix: prefix before properties name
 */
void XRefProperties::toSettings(QSettings &settings, const QString prefix) const {
	settings.setValue(prefix + "showpowerctc", m_show_power_ctc);
	QString display = m_display == Cross? "cross" : "contacts";
	settings.setValue(prefix + "displayhas", display);
	QString snap = m_snap_to == Bottom? "bottom" : "label";
	settings.setValue(prefix + "snapto", snap);
	foreach (QString key, m_prefix.keys()) {
		settings.setValue(prefix + key + "prefix", m_prefix.value(key));
	}
}

/**
 * @brief XRefProperties::fromSettings
 * load from settings
 * @param settings: QSettings to use
 * @param prefix: prefix before properties name
 */
void XRefProperties::fromSettings(const QSettings &settings, const QString prefix) {
	m_show_power_ctc = settings.value(prefix + "showpowerctc", false).toBool();
	QString display = settings.value(prefix + "displayhas", "cross").toString();
	display == "cross"? m_display = Cross : m_display = Contacts;
	QString snap = settings.value(prefix + "snapto", "label").toString();
	snap == "bottom"? m_snap_to = Bottom : m_snap_to = Label;
	foreach (QString key, m_prefix_keys) {
		m_prefix.insert(key, settings.value(prefix + key + "prefix").toString());
	}
}

/**
 * @brief XRefProperties::toXml
 * Save to xml
 * @param xml_element: QDomElement to use for saving
 */
void XRefProperties::toXml(QDomElement &xml_element) const {
	xml_element.setAttribute("showpowerctc", m_show_power_ctc? "true" : "false");
	QString display = m_display == Cross? "cross" : "contacts";
	xml_element.setAttribute("displayhas", display);
	QString snap = m_snap_to == Bottom? "bottom" : "label";
	xml_element.setAttribute("snapto", snap);
	foreach (QString key, m_prefix.keys()) {
		xml_element.setAttribute(key + "prefix", m_prefix.value(key));
	}
}

/**
 * @brief XRefProperties::fromXml
 * Load from xml
 * @param xml_element: QDomElement to use for load
 */
void XRefProperties::fromXml(const QDomElement &xml_element) {
	m_show_power_ctc = xml_element.attribute("showpowerctc")  == "true";
	QString display = xml_element.attribute("displayhas", "cross");
	display == "cross"? m_display = Cross : m_display = Contacts;
	QString snap = xml_element.attribute("snapto", "label");
	snap == "bottom"? m_snap_to = Bottom : m_snap_to = Label;
	foreach (QString key, m_prefix_keys) {
		m_prefix.insert(key, xml_element.attribute(key + "prefix"));
	}
}

/**
 * @brief XRefProperties::defaultProperties
 * @return the default properties stored in the setting file
 * For the xref, there is 2 propreties.
 * For coil, stored with the string "coil" in the returned QHash.
 * For protection, stored with the string "protection" in the returned QHash.
 */
QHash<QString, XRefProperties> XRefProperties::defaultProperties() {
	QHash <QString, XRefProperties> hash;
	QStringList keys;
	keys << "coil" << "protection" << "commutator";

	foreach (QString key, keys) {
		XRefProperties properties;
		QString str("diagrameditor/defaultxref");
		properties.fromSettings(QETApp::settings(), str += key);
		hash.insert(key, properties);
	}

	return hash;
}

bool XRefProperties::operator ==(const XRefProperties &xrp) const{
	return (m_show_power_ctc == xrp.m_show_power_ctc &&
			m_display == xrp.m_display &&
			m_snap_to == xrp.m_snap_to &&
			m_prefix == xrp.m_prefix);
}

bool XRefProperties::operator !=(const XRefProperties &xrp) const {
	return (! (*this == xrp));
}

