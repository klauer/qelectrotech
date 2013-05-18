#include <QRegExp>
#include "numparteditorw.h"
#include "ui_numparteditorw.h"

/**
 * Constructor
 */
NumPartEditorW::NumPartEditorW(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NumPartEditorW),
	intValidator (new QIntValidator(0,99999,this))
{
	ui -> setupUi(this);
	setType(NumPartEditorW::unit);
}

/**
 * Constructor
 * Build with value of @context at position i
 */
NumPartEditorW::NumPartEditorW (NumerotationContext &context, int i, QWidget *parent):
	QWidget(parent),
	ui(new Ui::NumPartEditorW),
	intValidator (new QIntValidator(0,99999,this))
{
	ui -> setupUi(this);

	//if @context contains nothing build with default value
	if(context.size()==0) setType(NumPartEditorW::unit);

	else {
		QStringList strl = context.itemAt(i);
		if (strl.at(0)=="unit") setType(NumPartEditorW::unit);
		else if (strl.at(0)=="ten") setType(NumPartEditorW::ten);
		else if (strl.at(0)=="hundred") setType(NumPartEditorW::hundred);
		else if (strl.at(0)=="string") setType(NumPartEditorW::string);
		else if (strl.at(0)== "folio") setType(NumPartEditorW::folio);
		ui -> value_field -> setText(strl.at(1));
		ui -> increase_spinBox -> setValue(strl.at(2).toInt());
	}
}

/**
 * Destructor
 */
NumPartEditorW::~NumPartEditorW()
{
	delete intValidator;
	delete ui;
}

/**
 * @brief NumPartEditorW::toNumContext
 * @return the display to NumerotationContext
 */
NumerotationContext NumPartEditorW::toNumContext() {
	NumerotationContext nc;
	nc.addValue(type_str, ui -> value_field -> displayText(), ui -> increase_spinBox -> value());
	return nc;
}

/**
 * @brief NumPartEditorW::on_type_combo_activated
 * Action when user change the type comboBox
 */
void NumPartEditorW::on_type_combo_activated(int index) {
	emit changed();
	switch (index) {
		case unit:
			setType(unit);
			break;
		case ten:
			setType(ten);
			break;
		case hundred:
			setType(hundred);
			break;
		case string:
			setType(string);
			break;
		case folio:
			setType(folio);
			break;
	};
}

/**
 * @brief NumPartEditorW::on_value_field_textChanged
 * emit changed when @value_field text changed
 */
void NumPartEditorW::on_value_field_textChanged() {
	emit changed();
}

/**
 * @brief NumPartEditorW::on_increase_spinBox_valueChanged
 *emit changed when @increase_spinBox value changed
 */
void NumPartEditorW::on_increase_spinBox_valueChanged() {
	emit changed();
}

/**
 * @brief NumPartEditorW::setType
 * Set good behavior by type @t
 */
void NumPartEditorW::setType(NumPartEditorW::type t) {
	ui -> type_combo -> setCurrentIndex(t);
	ui -> value_field -> clear();
	ui -> increase_spinBox -> setValue(1);
	switch (t) {
		case unit:
			ui -> value_field -> setEnabled(true);
			ui ->value_field -> setValidator(intValidator);
			ui -> increase_spinBox -> setEnabled(true);
			type_str = "unit";
			break;
		case ten:
			ui -> value_field -> setEnabled(true);
			ui ->value_field -> setValidator(intValidator);
			ui -> increase_spinBox -> setEnabled(true);
			type_str = "ten";
			break;
		case hundred:
			ui -> value_field -> setEnabled(true);
			ui ->value_field -> setValidator(intValidator);
			ui -> increase_spinBox -> setEnabled(true);
			type_str = "hundred";
			break;
		case string:
			ui -> value_field -> setValidator(0);
			ui -> value_field -> setEnabled(true);
			ui -> increase_spinBox -> setDisabled(true);
			type_str = "string";
			break;
		case folio:
			ui -> value_field -> setDisabled(true);
			ui -> increase_spinBox -> setDisabled(true);
			type_str = "folio";
			break;
	};
}
