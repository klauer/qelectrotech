#include "nameslist.h"

/**
	Constructeur
	@param parent QObject parent
*/
NamesList::NamesList() {
}

/**
	Constructeur de copie
	@param other La NamesList a copie
*/
NamesList::NamesList(const NamesList &other) : hash_names(other.hash_names) {	
}

/**
	Destructeur
*/
NamesList::~NamesList() {
}

/**
	Ajoute un nom a la liste
	@param lang Sigle de deux lettres representant une langue. Si cela n'est
	pas respecte, l'insertion n'est pas effectuee.
	@param name Nom lui-meme. Ce ne doit pas etre une chaine de caractere vide.
	Si cela n'est pas respecte, l'insertion n'est pas effectuee.
*/
void NamesList::addName(const QString &lang, const QString &name) {
	if (lang.length() != 2) return;
	hash_names.insert(lang, name);
}

/**
	Enleve le nom dont une langue donnee
	@param lang la langue pour laquelle il faut supprimer le nom
*/
void NamesList::removeName(const QString &lang) {
	hash_names.remove(lang);
}

/**
	Supprime tous les noms
*/
void NamesList::clearNames() {
	hash_names.clear();
}

/**
	@return La liste de toutes les langues disponibles
*/
QList<QString> NamesList::langs() const {
	return(hash_names.keys());
}

/**
	@return true si la liste de noms est vide, false sinon
*/
bool NamesList::isEmpty() const {
	return(hash_names.isEmpty());
}

/**
	@return Le nombre de noms dans la liste
*/
int NamesList::count() const {
	return(hash_names.count());
}

/**
	@param lang une langue
	@return Le nom dans la langue donnee ou QString() si ce nom n'est pas
	defini
*/
QString &NamesList::operator[](const QString &lang) {
	return(hash_names[lang]);
}

/**
	@param lang une langue
	@return Le nom dans la langue donnee ou QString() si ce nom n'est pas
	defini
*/
const QString NamesList::operator[](const QString &lang) const {
	return(hash_names.value(lang));
}

/**
	Charge la liste de noms depuis un element XML. Cet element est sense etre
	le parent d'un element "names", qui contient lui meme les "name".
	Les noms precedemment contenus dans la liste ne sont pas effaces mais
	peuvent etre ecrases.
	@param xml_element L'element XML a analyser
*/
void NamesList::fromXml(const QDomElement &xml_element) {
	// parcourt les enfants "names" de l'element XML
	for (QDomNode node = xml_element.firstChild() ; !node.isNull() ; node = node.nextSibling()) {
		QDomElement names = node.toElement();
		if (names.isNull() || names.tagName() != "names") continue;
		// parcourt les petits-enfants "name"
		for (QDomNode n = names.firstChild() ; !n.isNull() ; n = n.nextSibling()) {
			QDomElement name = n.toElement();
			if (name.isNull() || name.tagName() != "name") continue;
			addName(name.attribute("lang"), name.text());
		}
	}
}

/**
	Exporte la liste des noms vers un element XML. Veillez a verifier que la
	liste de noms n'est pas vide avant de l'exporter.
	@param xml_document Le document XML dans lequel l'element XML sera insere
	@return L'element XML correspondant a la section "names"
	@see count()
*/
QDomElement NamesList::toXml(QDomDocument &xml_document) const {
	QDomElement names_elmt = xml_document.createElement("names");
	QHashIterator<QString, QString> names_iterator(hash_names);
	while (names_iterator.hasNext()) {
		names_iterator.next();
		QDomElement name_elmt = xml_document.createElement("name");
		name_elmt.setAttribute("lang", names_iterator.key());
		name_elmt.appendChild(xml_document.createTextNode(names_iterator.value()));
		names_elmt.appendChild(name_elmt);
	}
	return(names_elmt);
}
