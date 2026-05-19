#include "dashboard_page.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

DashboardPage::DashboardPage(InventoryService& inventoryService,
                             MedicineRepository& repository,
                             const std::string& dataFilePath,
                             QWidget* parent)
    : QWidget(parent),
      inventoryService_(inventoryService),
      repository_(repository),
      dataFilePath_(dataFilePath),
      totalMedicinesValue_(0),
      lowStockValue_(0),
      totalOrdersValue_(0),
      totalRevenueValue_(0) {
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    QFrame* headerCard = new QFrame(this);
    QVBoxLayout* headerLayout = new QVBoxLayout(headerCard);
    QLabel* titleLabel = new QLabel("Dashboard", this);
    QLabel* subtitleLabel = new QLabel(
        "Overview of stock health, order volume, and sales performance.", this);
    QGridLayout* cardsLayout = new QGridLayout();

    rootLayout->setContentsMargins(22, 22, 22, 22);
    rootLayout->setSpacing(16);
    headerCard->setObjectName("dashboardHeaderCard");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #f6fbff;");
    subtitleLabel->setStyleSheet("color: #8fa2b7;");

    headerLayout->setContentsMargins(18, 18, 18, 18);
    headerLayout->setSpacing(8);
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(subtitleLabel);

    cardsLayout->setHorizontalSpacing(16);
    cardsLayout->setVerticalSpacing(16);
    cardsLayout->addWidget(createCard("Total Medicines", totalMedicinesValue_, "#4ea1ff"), 0, 0);
    cardsLayout->addWidget(createCard("Low Stock Items", lowStockValue_, "#ff8f6b"), 0, 1);
    cardsLayout->addWidget(createCard("Total Orders", totalOrdersValue_, "#6dd3a0"), 1, 0);
    cardsLayout->addWidget(createCard("Total Revenue", totalRevenueValue_, "#ffd166"), 1, 1);

    rootLayout->addWidget(headerCard);
    rootLayout->addLayout(cardsLayout);
    rootLayout->addStretch();

    setStyleSheet(
        "#dashboardHeaderCard { background-color: #121c27; border: 1px solid #233243; border-radius: 16px; }"
        "QFrame[dashboardCard='true'] { background-color: #121c27; border: 1px solid #233243; border-radius: 18px; }");

    refreshSummary();
}

void DashboardPage::refreshView() {
    refreshSummary();
}

QFrame* DashboardPage::createCard(const QString& title,
                                  QLabel*& valueLabel,
                                  const QString& accentColor) {
    QFrame* card = new QFrame(this);
    QVBoxLayout* layout = new QVBoxLayout(card);
    QHBoxLayout* topRow = new QHBoxLayout();
    QLabel* titleLabel = new QLabel(title, card);
    QLabel* iconLabel = new QLabel(card);
    valueLabel = new QLabel("0", card);
    QLabel* accentBar = new QLabel(card);
    QString iconPath = ":/resources/icons/dashboard.svg";

    if (title.contains("Medicines")) {
        iconPath = ":/resources/icons/medicine.svg";
    } else if (title.contains("Stock")) {
        iconPath = ":/resources/icons/stock.svg";
    } else if (title.contains("Orders")) {
        iconPath = ":/resources/icons/orders.svg";
    } else if (title.contains("Revenue")) {
        iconPath = ":/resources/icons/revenue.svg";
    }

    card->setProperty("dashboardCard", true);
    titleLabel->setStyleSheet("font-size: 15px; font-weight: 600; color: #9fb3c8;");
    valueLabel->setStyleSheet("font-size: 34px; font-weight: 700; color: #f6fbff;");
    accentBar->setFixedHeight(4);
    accentBar->setStyleSheet("background-color: " + accentColor +
                             "; border-radius: 2px;");
    iconLabel->setPixmap(QIcon(iconPath).pixmap(24, 24));
    iconLabel->setFixedSize(28, 28);

    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(10);
    topRow->addWidget(titleLabel);
    topRow->addStretch();
    topRow->addWidget(iconLabel);
    layout->addLayout(topRow);
    layout->addWidget(valueLabel);
    layout->addWidget(accentBar);
    layout->addStretch();

    return card;
}

void DashboardPage::refreshSummary() {
    const std::vector<Medicine>& medicines = inventoryService_.medicines();
    int lowStockCount = 0;
    for (std::size_t i = 0; i < medicines.size(); ++i) {
        if (medicines[i].quantity() < 10) {
            ++lowStockCount;
        }
    }

    totalMedicinesValue_->setText(QString::number(static_cast<int>(medicines.size())));
    lowStockValue_->setText(QString::number(lowStockCount));
    totalOrdersValue_->setText(QString::number(repository_.orderCount(dataFilePath_)));
    totalRevenueValue_->setText(
        QString("Rs. %1").arg(repository_.totalRevenue(dataFilePath_), 0, 'f', 2));
}
