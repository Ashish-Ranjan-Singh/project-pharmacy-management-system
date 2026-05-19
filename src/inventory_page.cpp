#include "inventory_page.h"

#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QFrame>
#include <QBrush>
#include <QColor>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QIcon>

#include "medicine_status.h"
#include "medicine_dialog.h"

namespace {
QString toQString(const std::string& value) {
    return QString::fromStdString(value);
}

QTableWidgetItem* alignedItem(const QString& text, Qt::Alignment alignment) {
    QTableWidgetItem* item = new QTableWidgetItem(text);
    item->setTextAlignment(alignment);
    return item;
}

QString statusDisplayText(MedicineStatus status) {
    switch (status) {
    case MedicineStatus::InStock:
        return "In Stock";
    case MedicineStatus::LowStock:
        return "Low Stock";
    case MedicineStatus::ExpiringSoon:
        return QString::fromUtf8("\xE2\x9A\xA0\xEF\xB8\x8F Expiring Soon");
    case MedicineStatus::Expired:
        return QString::fromUtf8("\xE2\x9D\x8C Expired");
    case MedicineStatus::OutOfStock:
        return QString::fromUtf8("\xE2\x9D\x8C Out of Stock");
    }

    return "In Stock";
}

QColor rowColorForStatus(MedicineStatus status) {
    switch (status) {
    case MedicineStatus::LowStock:
        return QColor(80, 32, 36);
    case MedicineStatus::ExpiringSoon:
        return QColor(92, 78, 28);
    case MedicineStatus::Expired:
        return QColor(70, 16, 20);
    case MedicineStatus::OutOfStock:
        return QColor(120, 20, 20);
    case MedicineStatus::InStock:
        return QColor();
    }

    return QColor();
}

void applyRowColor(QTableWidget* table, int row, int itemColumns, MedicineStatus status) {
    const QColor color = rowColorForStatus(status);
    if (!color.isValid()) {
        return;
    }

    for (int column = 0; column < itemColumns; ++column) {
        QTableWidgetItem* item = table->item(row, column);
        if (item != 0) {
            item->setData(Qt::BackgroundRole, color);
        }
    }
}
}

InventoryPage::InventoryPage(InventoryService& inventoryService,
                             MedicineRepository& repository,
                             const std::string& dataFilePath,
                             bool canDeleteMedicines,
                             QWidget* parent)
    : QWidget(parent),
      dataFilePath_(dataFilePath),
      canDeleteMedicines_(canDeleteMedicines),
      inventoryService_(inventoryService),
      repository_(repository),
      searchEdit_(new QLineEdit(this)),
      addButton_(new QPushButton("Add Medicine", this)),
      table_(new QTableWidget(this)) {
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    QFrame* topCard = new QFrame(this);
    QVBoxLayout* topCardLayout = new QVBoxLayout(topCard);
    QHBoxLayout* topBarLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("Inventory", this);
    QLabel* subtitleLabel = new QLabel("Manage medicine records, stock levels, and pricing.", this);
    QLabel* searchLabel = new QLabel("Search", this);
    topCard->setObjectName("topCard");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #f6fbff;");
    subtitleLabel->setStyleSheet("color: #8fa2b7;");

    searchEdit_->setPlaceholderText("Search by medicine name, company, or ID");
    addButton_->setProperty("class", "secondary");
    addButton_->setIcon(QIcon(":/resources/icons/add.svg"));
    addButton_->setIconSize(QSize(16, 16));

    rootLayout->setContentsMargins(22, 22, 22, 22);
    rootLayout->setSpacing(16);
    topBarLayout->setSpacing(12);
    topBarLayout->addWidget(searchLabel);
    topBarLayout->addWidget(searchEdit_, 1);
    topBarLayout->addWidget(addButton_);
    topCardLayout->setContentsMargins(18, 18, 18, 18);
    topCardLayout->setSpacing(8);
    topCardLayout->addWidget(titleLabel);
    topCardLayout->addWidget(subtitleLabel);
    topCardLayout->addLayout(topBarLayout);

    table_->setColumnCount(10);
    table_->setHorizontalHeaderLabels(QStringList()
                                      << "ID"
                                      << "Medicine"
                                      << "Company"
                                      << "Type"
                                      << "Quantity"
                                      << "Price"
                                      << "Expiry"
                                      << "Status"
                                      << "Edit"
                                      << "Delete");
    table_->setSelectionMode(QAbstractItemView::NoSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->verticalHeader()->setVisible(false);
    table_->setAlternatingRowColors(false);
    table_->setShowGrid(false);
    table_->verticalHeader()->setDefaultSectionSize(52);
    table_->setStyleSheet(
        "QTableWidget { background-color: transparent; }");
    setStyleSheet("#topCard { background-color: #121c27; border: 1px solid #233243; border-radius: 16px; }");

    rootLayout->addWidget(topCard);
    rootLayout->addWidget(table_);

    connect(searchEdit_, &QLineEdit::textChanged, this, &InventoryPage::updateFilter);
    connect(addButton_, &QPushButton::clicked, this, &InventoryPage::addMedicine);

    refreshTable();
}

void InventoryPage::refreshView() {
    refreshTable();
}

bool InventoryPage::saveInventory() {
    const bool saved = repository_.save(dataFilePath_, inventoryService_.medicines());
    if (!saved) {
        QMessageBox::critical(this, "Save Failed",
                              "Inventory could not be saved to the data file.");
    }
    return saved;
}

void InventoryPage::refreshTable() {
    table_->setRowCount(0);

    const QString filterText = searchEdit_->text().trimmed().toLower();
    const std::vector<Medicine> medicines = inventoryService_.sortedByName();

    for (std::size_t i = 0; i < medicines.size(); ++i) {
        const Medicine& medicine = medicines[i];

        const QString idText = QString::number(medicine.id());
        const QString nameText = toQString(medicine.name());
        const QString companyText = toQString(medicine.company());
        const QString typeText = toQString(Medicine::typeToString(medicine.type()));
        const QString quantityText = QString::number(medicine.quantity());
        const QString priceText = QString::number(medicine.price());
        const QString expiryText = toQString(medicine.expiryDate().toString());
        const MedicineStatus status = calculateMedicineStatus(medicine);
        const QString statusText = statusDisplayText(status);

        const QString haystack =
            (idText + " " + nameText + " " + companyText + " " + typeText + " " + statusText)
                .toLower();

        if (!filterText.isEmpty() && !haystack.contains(filterText)) {
            continue;
        }

        const int row = table_->rowCount();
        table_->insertRow(row);
        table_->setItem(row, 0, alignedItem(idText, Qt::AlignCenter));
        table_->setItem(row, 1, new QTableWidgetItem(nameText));
        table_->setItem(row, 2, new QTableWidgetItem(companyText));
        table_->setItem(row, 3, alignedItem(typeText, Qt::AlignCenter));
        table_->setItem(row, 4, alignedItem(quantityText, Qt::AlignCenter));
        table_->setItem(row, 5, alignedItem(priceText, Qt::AlignCenter));
        table_->setItem(row, 6, alignedItem(expiryText, Qt::AlignCenter));
        table_->setItem(row, 7, alignedItem(statusText, Qt::AlignCenter));

        QPushButton* editButton = new QPushButton("Edit", table_);
        QPushButton* deleteButton = new QPushButton("Delete", table_);
        editButton->setProperty("class", "secondary");
        deleteButton->setProperty("class", "danger");
        editButton->setIcon(QIcon(":/resources/icons/edit.svg"));
        deleteButton->setIcon(QIcon(":/resources/icons/delete.svg"));
        editButton->setIconSize(QSize(16, 16));
        deleteButton->setIconSize(QSize(16, 16));

        int id = medicine.id(); 

        connect(editButton, &QPushButton::clicked, this,
                [this, id]() { editMedicine(id); });
        connect(deleteButton, &QPushButton::clicked, this,
                [this, id]() { deleteMedicine(id); });
        deleteButton->setEnabled(canDeleteMedicines_);
        if (!canDeleteMedicines_) {
            deleteButton->setToolTip("Only admin can delete medicines.");
        }

        table_->setCellWidget(row, 8, editButton);
        table_->setCellWidget(row, 9, deleteButton);
        applyRowColor(table_, row, table_->columnCount() , status);
    }
}

void InventoryPage::updateFilter(const QString&) {
    refreshTable();
}

void InventoryPage::addMedicine() {
    MedicineDialog dialog(this);
    dialog.setWindowTitleText("Add Medicine");

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Medicine newMedicine = dialog.medicine();
    newMedicine.setId(rand() % 1000000);

    if (!repository_.addMedicine(dataFilePath_, newMedicine)) {
        QMessageBox::warning(this, "Error",
                            "Database insert failed.");
        return;
    }

    inventoryService_.replaceAll(repository_.load(dataFilePath_));  // reload to update in-memory data
    refreshTable();
}

void InventoryPage::editMedicine(int medicineId) {
    qDebug() << "Editing medicine with ID:" << medicineId;
    const Medicine* existing = inventoryService_.findById(medicineId);
    
    if (existing == nullptr) {
        qDebug() << "Medicine not found!";
        QMessageBox::warning(this, "Missing Medicine",
                             "The selected medicine could not be found.");
        return;
    }

    qDebug() << "Medicine found: " << QString::fromStdString(existing->name()) << " with ID:" << existing->id();

    MedicineDialog dialog(this);
    dialog.setWindowTitleText("Edit Medicine");
    dialog.setMedicine(*existing);

    qDebug() << "After dialog.exec()";

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const Medicine updated = dialog.medicine();
    if (updated.id() != medicineId && inventoryService_.findById(updated.id()) != 0) {
        QMessageBox::warning(this, "Duplicate ID",
                             "A medicine with the updated ID already exists.");
        return;
    }

    if (updated.id() != medicineId) {
        inventoryService_.removeMedicineById(medicineId);
        if (!inventoryService_.addMedicine(updated)) {
            QMessageBox::warning(this, "Update Failed",
                                 "The medicine could not be updated.");
            return;
        }
    } else if (!repository_.updateMedicine(dataFilePath_, updated)) {
        QMessageBox::warning(this, "Update Failed",
                             "The medicine could not be updated.");
        return;
    }

    inventoryService_.replaceAll(repository_.load(dataFilePath_));  // reload to update in-memory data
    refreshTable();
}

void InventoryPage::deleteMedicine(int medicineId) {
    if (!canDeleteMedicines_) {
        QMessageBox::warning(this, "Access Denied",
                             "Only admin users can delete medicines.");
        return;
    }

    const Medicine* medicine = inventoryService_.findById(medicineId);
    if (medicine == 0) {
        QMessageBox::warning(this, "Missing Medicine",
                             "The selected medicine could not be found.");
        return;
    }

    const QMessageBox::StandardButton result =
        QMessageBox::question(this, "Delete Medicine",
                              "Delete " + toQString(medicine->name()) + "?");

    if (result != QMessageBox::Yes) {
        return;
    }

    if (!repository_.deleteMedicine(dataFilePath_, medicineId)) {
        QMessageBox::warning(this, "Delete Failed",
                            "Database delete failed.");
        return;
    }

    inventoryService_.replaceAll(repository_.load(dataFilePath_));  // reload to update in-memory data
    refreshTable();
}
