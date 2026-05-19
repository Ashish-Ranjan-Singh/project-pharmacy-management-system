#include "orders_page.h"

#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QFrame>
#include <QBrush>
#include <QColor>
#include <QDateTime>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QIcon>

#include "checkout_dialog.h"
#include "medicine_status.h"

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
            item->setBackground(QBrush(color));
        }
    }
}
}

OrdersPage::OrdersPage(InventoryService& inventoryService,
                       MedicineRepository& repository,
                       const std::string& dataFilePath,
                       QWidget* parent)
    : QWidget(parent),
      inventoryService_(inventoryService),
      repository_(repository),
      dataFilePath_(dataFilePath),
      searchEdit_(new QLineEdit(this)),
      medicinesTable_(new QTableWidget(this)),
      cartTable_(new QTableWidget(this)),
      totalValueLabel_(new QLabel("0", this)),
      clearCartButton_(new QPushButton("Clear Cart", this)),
      checkoutButton_(new QPushButton("Checkout", this)) {
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    QFrame* topCard = new QFrame(this);
    QVBoxLayout* topCardLayout = new QVBoxLayout(topCard);
    QLabel* titleLabel = new QLabel("Orders", this);
    QLabel* subtitleLabel = new QLabel("Build a cart from current inventory and complete sales.", this);
    QHBoxLayout* searchLayout = new QHBoxLayout();
    QLabel* searchLabel = new QLabel("Search Medicines:", this);
    QHBoxLayout* contentLayout = new QHBoxLayout();
    QVBoxLayout* medicinesLayout = new QVBoxLayout();
    QVBoxLayout* cartLayout = new QVBoxLayout();
    QHBoxLayout* totalLayout = new QHBoxLayout();
    QHBoxLayout* actionLayout = new QHBoxLayout();

    titleLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #f6fbff;");
    subtitleLabel->setStyleSheet("color: #8fa2b7;");
    topCard->setObjectName("topCard");

    searchEdit_->setPlaceholderText("Search by medicine name, company, or ID");
    clearCartButton_->setProperty("class", "secondary");
    clearCartButton_->setIcon(QIcon(":/resources/icons/delete.svg"));
    checkoutButton_->setIcon(QIcon(":/resources/icons/checkout.svg"));
    clearCartButton_->setIconSize(QSize(16, 16));
    checkoutButton_->setIconSize(QSize(16, 16));

    rootLayout->setContentsMargins(22, 22, 22, 22);
    rootLayout->setSpacing(16);
    searchLayout->setSpacing(12);
    medicinesTable_->setColumnCount(8);
    medicinesTable_->setHorizontalHeaderLabels(QStringList()
                                               << "ID"
                                               << "Medicine"
                                               << "Company"
                                               << "Type"
                                               << "In Stock"
                                               << "Price"
                                               << "Status"
                                               << "Add");
    medicinesTable_->setSelectionMode(QAbstractItemView::NoSelection);
    medicinesTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    medicinesTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    medicinesTable_->verticalHeader()->setVisible(false);
    medicinesTable_->setAlternatingRowColors(true);
    medicinesTable_->setShowGrid(false);
    medicinesTable_->verticalHeader()->setDefaultSectionSize(52);

    cartTable_->setColumnCount(7);
    cartTable_->setHorizontalHeaderLabels(QStringList()
                                          << "ID"
                                          << "Medicine"
                                          << "Unit Price"
                                          << "Quantity"
                                          << "Total"
                                          << "Remove"
                                          << "");
    cartTable_->setSelectionMode(QAbstractItemView::NoSelection);
    cartTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    cartTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    cartTable_->verticalHeader()->setVisible(false);
    cartTable_->setAlternatingRowColors(true);
    cartTable_->setShowGrid(false);
    cartTable_->verticalHeader()->setDefaultSectionSize(52);

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchEdit_, 1);
    topCardLayout->setContentsMargins(18, 18, 18, 18);
    topCardLayout->setSpacing(8);
    topCardLayout->addWidget(titleLabel);
    topCardLayout->addWidget(subtitleLabel);
    topCardLayout->addLayout(searchLayout);

    medicinesLayout->addWidget(new QLabel("Available Medicines", this));
    medicinesLayout->addWidget(medicinesTable_);

    cartLayout->addWidget(new QLabel("Cart", this));
    cartLayout->addWidget(cartTable_);
    totalLayout->addWidget(new QLabel("Total Price:", this));
    totalLayout->addWidget(totalValueLabel_);
    totalLayout->addStretch();
    actionLayout->addWidget(clearCartButton_);
    actionLayout->addWidget(checkoutButton_);
    actionLayout->addStretch();
    cartLayout->addLayout(totalLayout);
    cartLayout->addLayout(actionLayout);
    totalValueLabel_->setStyleSheet("font-size: 24px; font-weight: 700; color: #8cd0ff;");

    contentLayout->addLayout(medicinesLayout, 3);
    contentLayout->addLayout(cartLayout, 2);

    rootLayout->addWidget(topCard);
    rootLayout->addLayout(contentLayout, 1);
    setStyleSheet("#topCard { background-color: #121c27; border: 1px solid #233243; border-radius: 16px; }");

    connect(searchEdit_, &QLineEdit::textChanged, this, &OrdersPage::updateFilter);
    connect(clearCartButton_, &QPushButton::clicked, this, &OrdersPage::clearCart);
    connect(checkoutButton_, &QPushButton::clicked, this, &OrdersPage::checkout);

    refreshView();
}

void OrdersPage::refreshView() {
    refreshMedicineTable();
    refreshCartTable();
    refreshTotal();
}

bool OrdersPage::saveInventory() {
    const bool saved = repository_.save(dataFilePath_, inventoryService_.medicines());
    if (!saved) {
        QMessageBox::critical(this, "Save Failed",
                              "Inventory could not be saved to the data file.");
    }
    return saved;
}

void OrdersPage::refreshMedicineTable() {
    medicinesTable_->setRowCount(0);

    const QString filterText = searchEdit_->text().trimmed().toLower();
    const std::vector<Medicine> medicines = inventoryService_.sortedByName();

    for (std::size_t i = 0; i < medicines.size(); ++i) {
        const Medicine& medicine = medicines[i];
        const QString idText = QString::number(medicine.id());
        const QString nameText = toQString(medicine.name());
        const QString companyText = toQString(medicine.company());
        const QString typeText = toQString(Medicine::typeToString(medicine.type()));
        const QString stockText = QString::number(medicine.quantity());
        const QString priceText = QString::number(medicine.price());
        const MedicineStatus status = calculateMedicineStatus(medicine);
        const QString statusText = statusDisplayText(status);
        const QString haystack =
            (idText + " " + nameText + " " + companyText + " " + typeText + " " + statusText)
                .toLower();

        if ((!filterText.isEmpty() && !haystack.contains(filterText)) || medicine.quantity() <= 0) {
            continue;
        }

        const int row = medicinesTable_->rowCount();
        medicinesTable_->insertRow(row);
        medicinesTable_->setItem(row, 0, alignedItem(idText, Qt::AlignCenter));
        medicinesTable_->setItem(row, 1, new QTableWidgetItem(nameText));
        medicinesTable_->setItem(row, 2, new QTableWidgetItem(companyText));
        medicinesTable_->setItem(row, 3, alignedItem(typeText, Qt::AlignCenter));
        medicinesTable_->setItem(row, 4, alignedItem(stockText, Qt::AlignCenter));
        medicinesTable_->setItem(row, 5, alignedItem(priceText, Qt::AlignCenter));
        medicinesTable_->setItem(row, 6, alignedItem(statusText, Qt::AlignCenter));

        QPushButton* addButton = new QPushButton("Add to Cart", medicinesTable_);
        addButton->setProperty("class", "secondary");
        addButton->setIcon(QIcon(":/resources/icons/add.svg"));
        addButton->setIconSize(QSize(16, 16));
        connect(addButton, &QPushButton::clicked, this,
                [this, medicine]() { addToCart(medicine.id()); });
        medicinesTable_->setCellWidget(row, 7, addButton);
        applyRowColor(medicinesTable_, row, 7, status);
    }
}

void OrdersPage::refreshCartTable() {
    cartTable_->setRowCount(0);

    const std::vector<OrderItem>& items = orderService_.items();
    for (std::size_t i = 0; i < items.size(); ++i) {
        const OrderItem& item = items[i];
        const int row = cartTable_->rowCount();
        cartTable_->insertRow(row);
        cartTable_->setItem(row, 0, alignedItem(QString::number(item.medicineId), Qt::AlignCenter));
        cartTable_->setItem(row, 1, new QTableWidgetItem(toQString(item.medicineName)));
        cartTable_->setItem(row, 2, alignedItem(QString::number(item.unitPrice), Qt::AlignCenter));
        cartTable_->setItem(row, 3, alignedItem(QString::number(item.quantity), Qt::AlignCenter));
        cartTable_->setItem(row, 4, alignedItem(QString::number(item.totalPrice()), Qt::AlignCenter));

        QPushButton* removeButton = new QPushButton("Remove", cartTable_);
        removeButton->setProperty("class", "danger");
        removeButton->setIcon(QIcon(":/resources/icons/delete.svg"));
        removeButton->setIconSize(QSize(16, 16));
        connect(removeButton, &QPushButton::clicked, this,
                [this, item]() { removeFromCart(item.medicineId); });
        cartTable_->setCellWidget(row, 5, removeButton);
    }
}

void OrdersPage::refreshTotal() {
    totalValueLabel_->setText(QString::number(orderService_.total()));
}

void OrdersPage::updateFilter(const QString&) {
    refreshMedicineTable();
}

void OrdersPage::addToCart(int medicineId) {
    const Medicine* medicine = inventoryService_.findById(medicineId);
    if (medicine == 0) {
        QMessageBox::warning(this, "Missing Medicine",
                             "The selected medicine could not be found.");
        return;
    }

    bool ok = false;
    const int quantity = QInputDialog::getInt(this,
                                              "Add to Cart",
                                              "Quantity:",
                                              1,
                                              1,
                                              medicine->quantity(),
                                              1,
                                              &ok);
    if (!ok) {
        return;
    }

    if (!orderService_.addItem(*medicine, quantity)) {
        QMessageBox::warning(this, "Add Failed",
                             "The selected quantity could not be added to the cart.");
        return;
    }

    refreshCartTable();
    refreshTotal();
}

void OrdersPage::removeFromCart(int medicineId) {
    if (!orderService_.removeItemByMedicineId(medicineId)) {
        QMessageBox::warning(this, "Remove Failed",
                             "The selected cart item could not be removed.");
        return;
    }

    refreshCartTable();
    refreshTotal();
}

void OrdersPage::clearCart() {
    if (orderService_.items().empty()) {
        return;
    }

    orderService_.clear();
    refreshCartTable();
    refreshTotal();
}

void OrdersPage::checkout() {
    if (orderService_.items().empty()) {
        QMessageBox::information(this, "Empty Cart",
                                 "Add medicines to the cart before checkout.");
        return;
    }

    const std::vector<OrderItem> itemsSnapshot = orderService_.items();
    const int totalSnapshot = orderService_.total();
    const std::string dateTimeText =
        QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss").toStdString();

    CheckoutDialog dialog(itemsSnapshot, totalSnapshot, dateTimeText, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (!orderService_.checkout(inventoryService_)) {
        QMessageBox::warning(this, "Checkout Failed",
                             "The cart could not be checked out because stock changed or items are invalid.");
        refreshView();
        return;
    }

    if (!repository_.saveCheckout(dataFilePath_,
                                  inventoryService_.medicines(),
                                  dateTimeText,
                                  totalSnapshot,
                                  itemsSnapshot)) {
        inventoryService_.replaceAll(repository_.load(dataFilePath_));
        restoreCart(itemsSnapshot);
        refreshView();
        QMessageBox::warning(this, "Checkout Failed",
                             "Inventory or receipt data could not be saved.");
        return;
    }

    QMessageBox::information(this, "Checkout Complete",
                             "Inventory has been updated and the cart has been cleared.");
    refreshView();
}

void OrdersPage::restoreCart(const std::vector<OrderItem>& items) {
    orderService_.clear();

    for (std::size_t i = 0; i < items.size(); ++i) {
        const Medicine* medicine = inventoryService_.findById(items[i].medicineId);
        if (medicine != 0) {
            orderService_.addItem(*medicine, items[i].quantity);
        }
    }
}
