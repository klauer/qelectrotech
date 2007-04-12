#ifndef NAMES_LIST_H
#define NAMES_LIST_H
#include <QtGui>
/**
	Cette classe represente une interface permettant de saisir les noms des
	categories et elements.
*/
class NamesList : public QWidget {
	Q_OBJECT
	
	// constructeurs, destructeur
	public:
	NamesList(QWidget * = 0);
	virtual ~NamesList();
	
	private:
	NamesList(const NamesList &);
	
	// attributs
	private:
	QTreeWidget *tree_names;
	QPushButton *button_add_line;
	QHash<QString, QString> hash_names;
	
	// methodes
	public:
	bool checkOneName();
	QHash<QString, QString> names();
	void setNames(QHash<QString, QString> &);
	
	private:
	void clean();
	void updateHash();
	
	public slots:
	void addLine();
};
#endif
