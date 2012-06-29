/*
	Copyright 2006-2012 Xavier Guerrin
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
#include "titleblockproperties.h"
#include "qet.h"

/**
	Constructeur. Initialise un objet TitleBlockProperties avec tous les champs
	vides (date vide + useDate a UseDateValue).
*/
TitleBlockProperties::TitleBlockProperties() :
	date(),
	useDate(UseDateValue)
{
}

/**
	Destructeur
*/
TitleBlockProperties::~TitleBlockProperties() {
}

/**
	@param ip autre conteneur TitleBlockProperties
	@return true si ip et ce conteneur sont identiques, false sinon
*/
bool TitleBlockProperties::operator==(const TitleBlockProperties &ip) {
	return(
		ip.title == title &&\
		ip.author == author &&\
		ip.date == date &&\
		ip.filename == filename &&\
		ip.folio == folio &&\
		ip.template_name == template_name &&\
		ip.context == context
	);
}

/**
	@param ip autre conteneur TitleBlockProperties
	@return false si ip et ce conteneur sont identiques, true sinon
*/
bool TitleBlockProperties::operator!=(const TitleBlockProperties &ip) {
	return(!(*this == ip));
}


/**
	Exporte le cartouche sous formes d'attributs XML ajoutes a l'element e.
	@param e Element XML auquel seront ajoutes des attributs
*/
void TitleBlockProperties::toXml(QDomElement &e) const {
	e.setAttribute("author",   author);
	e.setAttribute("title",    title);
	e.setAttribute("filename", filename);
	e.setAttribute("folio",    folio);
	e.setAttribute("date",     exportDate());
	if (!template_name.isEmpty()) {
		e.setAttribute("titleblocktemplate", template_name);
	}
	
	if (context.keys().count()) {
		QDomElement properties = e.ownerDocument().createElement("properties");
		foreach (QString key, context.keys()) {
			QDomElement property = e.ownerDocument().createElement("property");
			property.setAttribute("name", key);
			QDomText value = e.ownerDocument().createTextNode(context[key].toString());
			property.appendChild(value);
			properties.appendChild(property);
		}
		e.appendChild(properties);
	}
}

/**
	Importe le cartouche a partir des attributs XML de l'element e
	@param e Element XML dont les attributs seront lus
*/
void TitleBlockProperties::fromXml(const QDomElement &e) {
	// reads the historical fields
	if (e.hasAttribute("author"))      author   = e.attribute("author");
	if (e.hasAttribute("title"))       title    = e.attribute("title");
	if (e.hasAttribute("filename"))    filename = e.attribute("filename");
	if (e.hasAttribute("folio"))       folio    = e.attribute("folio");
	if (e.hasAttribute("date"))        setDateFromString(e.attribute("date"));
	
	// reads the template used to render the title block
	if (e.hasAttribute("titleblocktemplate")) template_name = e.attribute("titleblocktemplate");
	
	// reads the additional fields used to fill the title block
	foreach (QDomElement property, QET::findInDomElement(e, "properties", "property")) {
		if (!property.hasAttribute("name")) continue;
		context.addValue(property.attribute("name"), QVariant(property.text()));
	}
}

/**
	Exporte le cartouche dans une configuration.
	@param settings Parametres a ecrire
	@param prefix prefixe a ajouter devant les noms des parametres
*/
void TitleBlockProperties::toSettings(QSettings &settings, const QString &prefix) const {
	settings.setValue(prefix + "title",    title);
	settings.setValue(prefix + "author",   author);
	settings.setValue(prefix + "filename", filename);
	settings.setValue(prefix + "folio",    folio);
	settings.setValue(prefix + "date",     exportDate());
	
	settings.beginWriteArray(prefix + "properties");
	int i = 0;
	foreach (QString key, context.keys()) {
		settings.setArrayIndex(i);
		settings.setValue("name", key);
		settings.setValue("value", context[key].toString());
		++ i;
	}
	settings.endArray();
}

/**
	Importe le cartouche depuis une configuration.
	@param settings Parametres a lire
	@param prefix prefixe a ajouter devant les noms des parametres
*/
void TitleBlockProperties::fromSettings(QSettings &settings, const QString &prefix) {
	title    = settings.value(prefix + "title").toString();
	author   = settings.value(prefix + "author").toString();
	filename = settings.value(prefix + "filename").toString();
	folio    = settings.value(prefix + "folio", "%id/%total").toString();
	setDateFromString(settings.value(prefix + "date").toString());
	
	int size = settings.beginReadArray(prefix + "properties");
	for (int i = 0 ; i < size; ++ i) {
		settings.setArrayIndex(i);
		QString key = settings.value("name").toString();
		if (key.isEmpty()) continue;
		context.addValue(key, settings.value("value").toString());
	}
	settings.endArray();
}

/**
	@return La date a utiliser
*/
QDate TitleBlockProperties::finalDate() const {
	if (useDate == UseDateValue) {
		return(date);
	} else {
		return(QDate::currentDate());
	}
}

/**
	@return une chaine de caracteres decrivant comment gerer la date dans le
	cartouche : la chaine peut valoir :
	  * null pour ne pas afficher de date
	  * now pour afficher la date courante (a la creation du schema)
	  * une date au format yyyyMMdd pour utiliser une date fixe
*/
QString TitleBlockProperties::exportDate() const {
	QString date_setting_value;
	if (useDate == UseDateValue) {
		if (date.isNull()) date_setting_value = "null";
		else date_setting_value = date.toString("yyyyMMdd");
	} else {
		date_setting_value = "now";
	}
	return(date_setting_value);
}

/**
	Charge les attributs date et useDate a partir d'une chaine de caracteres.
	@param date_string Chaine de caracteres a analyser
	@see exportDate
*/
void TitleBlockProperties::setDateFromString(const QString &date_string) {
	if (date_string == "now") {
		date = QDate::currentDate();
		useDate = CurrentDate;
	} else if (date_string.isEmpty() || date_string == "null") {
		date = QDate();
		useDate = UseDateValue;
	} else {
		date = QDate::fromString(date_string, "yyyyMMdd");
		useDate = UseDateValue;
	}
}
