#ifndef MEDICINE_DIALOG_H
#define MEDICINE_DIALOG_H

#include <QDialog>

#include "medicine.h"

class QComboBox;
class QDateEdit;
class QDialogButtonBox;
class QLineEdit;
class QSpinBox;

class MedicineDialog : public QDialog {
public:
    explicit MedicineDialog(QWidget* parent = 0);

    void setWindowTitleText(const QString& title);
    void setMedicine(const Medicine& medicine);
    Medicine medicine() const;

private:
    void buildUi();

    QLineEdit* companyEdit_;
    QLineEdit* nameEdit_;
    QSpinBox* idSpinBox_;
    QSpinBox* quantitySpinBox_;
    QSpinBox* priceSpinBox_;
    QComboBox* typeComboBox_;
    QDateEdit* expiryDateEdit_;
    QDialogButtonBox* buttonBox_;

    int originalId_;
};

#endif
