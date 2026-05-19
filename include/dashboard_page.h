#ifndef DASHBOARD_PAGE_H
#define DASHBOARD_PAGE_H

#include <QWidget>
#include <string>

#include "inventory_service.h"
#include "medicine_repository.h"

class QLabel;
class QFrame;

class DashboardPage : public QWidget {
public:
    DashboardPage(InventoryService& inventoryService,
                  MedicineRepository& repository,
                  const std::string& dataFilePath,
                  QWidget* parent = 0);

    void refreshView();

private:
    QFrame* createCard(const QString& title, QLabel*& valueLabel, const QString& accentColor);
    void refreshSummary();

    InventoryService& inventoryService_;
    MedicineRepository& repository_;
    std::string dataFilePath_;
    QLabel* totalMedicinesValue_;
    QLabel* lowStockValue_;
    QLabel* totalOrdersValue_;
    QLabel* totalRevenueValue_;
};

#endif
