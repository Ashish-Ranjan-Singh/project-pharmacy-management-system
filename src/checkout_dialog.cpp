#include "checkout_dialog.h"

#include <QAbstractItemView>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QVBoxLayout>
#include <QIcon>

namespace {
QString toQString(const std::string& value) {
    return QString::fromStdString(value);
}
}

CheckoutDialog::CheckoutDialog(const std::vector<OrderItem>& items,
                               int total,
                               const std::string& dateTimeText,
                               QWidget* parent)
    : QDialog(parent),
      items_(items),
      total_(total),
      dateTimeText_(dateTimeText),
      table_(new QTableWidget(this)),
      totalLabel_(new QLabel(this)),
      dateTimeLabel_(new QLabel(this)),
      exportButton_(new QPushButton("Export to Text", this)) {
    setWindowTitle("Checkout Receipt");
    resize(560, 460);

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    QLabel* titleLabel = new QLabel("Pharmacy Receipt", this);
    QLabel* dividerLabel = new QLabel("------------------------", this);
    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    titleLabel->setStyleSheet("font-size: 22px; font-weight: 700; color: #f6fbff;");
    dividerLabel->setStyleSheet("color: #8fa2b7;");

    table_->setColumnCount(3);
    table_->setHorizontalHeaderLabels(QStringList() << "Medicine" << "Qty" << "Price");
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->verticalHeader()->setVisible(false);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionMode(QAbstractItemView::NoSelection);
    table_->setAlternatingRowColors(true);
    table_->setShowGrid(false);
    table_->verticalHeader()->setDefaultSectionSize(44);

    for (std::size_t i = 0; i < items_.size(); ++i) {
        const OrderItem& item = items_[i];
        const int row = table_->rowCount();
        table_->insertRow(row);
        table_->setItem(row, 0, new QTableWidgetItem(toQString(item.medicineName)));
        table_->setItem(row, 1, new QTableWidgetItem(QString::number(item.quantity)));
        table_->setItem(row, 2, new QTableWidgetItem(QString::number(item.totalPrice())));
    }

    totalLabel_->setText("Total: " + QString::number(total_));
    totalLabel_->setStyleSheet("font-size: 20px; font-weight: 700; color: #8cd0ff;");
    dateTimeLabel_->setText("Date: " + toQString(dateTimeText_));
    exportButton_->setProperty("class", "secondary");
    exportButton_->setIcon(QIcon(":/resources/icons/checkout.svg"));
    buttonBox->button(QDialogButtonBox::Cancel)->setProperty("class", "secondary");
    buttonBox->button(QDialogButtonBox::Ok)->setText("Confirm Checkout");
    buttonBox->button(QDialogButtonBox::Ok)->setIcon(QIcon(":/resources/icons/checkout.svg"));
    buttonBox->button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/resources/icons/delete.svg"));

    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(12);
    rootLayout->addWidget(titleLabel);
    rootLayout->addWidget(dividerLabel);
    rootLayout->addWidget(table_);
    rootLayout->addWidget(totalLabel_);
    rootLayout->addWidget(dateTimeLabel_);
    rootLayout->addWidget(exportButton_, 0, Qt::AlignLeft);
    rootLayout->addWidget(buttonBox);

    connect(exportButton_, &QPushButton::clicked, this, &CheckoutDialog::exportReceipt);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

std::string CheckoutDialog::buildReceiptText() const {
    std::string receipt;
    receipt += "Pharmacy Receipt\n";
    receipt += "------------------------\n";
    receipt += "Medicine\tQty\tPrice\n";
    receipt += "------------------------\n";

    for (std::size_t i = 0; i < items_.size(); ++i) {
        const OrderItem& item = items_[i];
        receipt += item.medicineName + "\t" +
                   std::to_string(item.quantity) + "\t" +
                   std::to_string(item.totalPrice()) + "\n";
    }

    receipt += "------------------------\n";
    receipt += "Total: " + std::to_string(total_) + "\n";
    receipt += "Date: " + dateTimeText_ + "\n";
    return receipt;
}

void CheckoutDialog::exportReceipt() {
    const QString filePath = QFileDialog::getSaveFileName(
        this, "Export Receipt", "receipt.txt", "Text Files (*.txt)");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Export Failed", "The receipt could not be written to file.");
        return;
    }

    QTextStream stream(&file);
    stream << toQString(buildReceiptText());
    file.close();

    QMessageBox::information(this, "Receipt Exported", "Receipt exported successfully.");
}
