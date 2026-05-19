#include "medicine_dialog.h"
#include "inventory_page.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QIcon>

namespace {
int comboIndexForType(MedicineType type) {
    const std::vector<MedicineType>& types = Medicine::availableTypes();
    for (int i = 0; i < static_cast<int>(types.size()); ++i) {
        if (types[static_cast<std::size_t>(i)] == type) {
            return i;
        }
    }

    return 0;
}
}

MedicineDialog::MedicineDialog(QWidget* parent)
    : QDialog(parent),
      companyEdit_(0),
      nameEdit_(0),
      idSpinBox_(0),
      quantitySpinBox_(0),
      priceSpinBox_(0),
      typeComboBox_(0),
      expiryDateEdit_(0),
      buttonBox_(0),
      originalId_(0) {
    buildUi();
}

void MedicineDialog::setWindowTitleText(const QString& title) {
    setWindowTitle(title);
}

void MedicineDialog::setMedicine(const Medicine& medicine) {
    originalId_ = medicine.id();

    if (!idSpinBox_) {
        qDebug() << "ERROR: idSpinBox_ is null!";
        return;
    }

    companyEdit_->setText(QString::fromStdString(medicine.company()));
    nameEdit_->setText(QString::fromStdString(medicine.name()));
    idSpinBox_->setValue(medicine.id());
    quantitySpinBox_->setValue(medicine.quantity());
    priceSpinBox_->setValue(medicine.price());
    typeComboBox_->setCurrentIndex(comboIndexForType(medicine.type()));
    expiryDateEdit_->setDate(QDate(medicine.expiryDate().year(),
                                   medicine.expiryDate().month(),
                                   medicine.expiryDate().day()));
}

Medicine MedicineDialog::medicine() const {
    const std::vector<MedicineType>& types = Medicine::availableTypes();
    MedicineType type = MedicineType::Tablet;
    const int currentIndex = typeComboBox_->currentIndex();
    if (currentIndex >= 0 && currentIndex < static_cast<int>(types.size())) {
        type = types[static_cast<std::size_t>(currentIndex)];
    }
    const QDate qDate = expiryDateEdit_->date();

    int id = originalId_;
    if (id == 0) {
        id = idSpinBox_->value();   // fallback
    }

    qDebug() << "Returning ID:" << id;

    return Medicine(companyEdit_->text().trimmed().toStdString(),
                    nameEdit_->text().trimmed().toStdString(),
                    quantitySpinBox_->value(),
                    id,
                    priceSpinBox_->value(),
                    type,
                    Date(qDate.day(), qDate.month(), qDate.year()));
}

void MedicineDialog::buildUi() {
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    QLabel* titleLabel = new QLabel("Medicine Details", this);
    QFormLayout* formLayout = new QFormLayout();

    companyEdit_ = new QLineEdit(this);
    nameEdit_ = new QLineEdit(this);

    idSpinBox_ = new QSpinBox(this);
    idSpinBox_->setRange(0, 1000000000);
    idSpinBox_->setEnabled(false);  // ID is auto-generated and should not be edited by the user

    quantitySpinBox_ = new QSpinBox(this);
    priceSpinBox_ = new QSpinBox(this);
    typeComboBox_ = new QComboBox(this);
    expiryDateEdit_ = new QDateEdit(this);
    buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    idSpinBox_->setRange(0, 1000000000);
    quantitySpinBox_->setRange(0, 1000000);
    priceSpinBox_->setRange(0, 1000000);

    const std::vector<MedicineType>& types = Medicine::availableTypes();
    for (std::size_t i = 0; i < types.size(); ++i) {
        typeComboBox_->addItem(QString::fromStdString(Medicine::typeToString(types[i])));
    }

    expiryDateEdit_->setCalendarPopup(true);
    expiryDateEdit_->setDisplayFormat("dd/MM/yyyy");
    expiryDateEdit_->setDate(QDate::currentDate());
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #f6fbff;");
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(14);
    formLayout->setSpacing(12);
    buttonBox_->button(QDialogButtonBox::Cancel)->setProperty("class", "secondary");
    buttonBox_->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/resources/icons/add.svg"));
    buttonBox_->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/resources/icons/delete.svg"));

    rootLayout->addWidget(titleLabel);
    formLayout->addRow("Company", companyEdit_);
    formLayout->addRow("Medicine", nameEdit_);
    formLayout->addRow("ID", idSpinBox_);
    formLayout->addRow("Quantity", quantitySpinBox_);
    formLayout->addRow("Price", priceSpinBox_);
    formLayout->addRow("Type", typeComboBox_);
    formLayout->addRow("Expiry", expiryDateEdit_);

    rootLayout->addLayout(formLayout);
    rootLayout->addWidget(buttonBox_);

    connect(buttonBox_, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox_, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
