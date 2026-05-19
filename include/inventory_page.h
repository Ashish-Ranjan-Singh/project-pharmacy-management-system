#ifndef INVENTORY_PAGE_H
#define INVENTORY_PAGE_H

#include <QWidget>

#include <string>

#include "inventory_service.h"
#include "medicine_repository.h"

class QLineEdit;
class QPushButton;
class QTableWidget;

class InventoryPage : public QWidget {
public:
    explicit InventoryPage(InventoryService& inventoryService,
                           MedicineRepository& repository,
                           const std::string& dataFilePath,
                           bool canDeleteMedicines,
                           QWidget* parent = 0);

    void refreshView();

private:
    bool saveInventory();
    void refreshTable();
    void updateFilter(const QString& text);
    void addMedicine();
    void editMedicine(int medicineId);
    void deleteMedicine(int medicineId);

    std::string dataFilePath_;
    bool canDeleteMedicines_;
    InventoryService& inventoryService_;
    MedicineRepository& repository_;
    QLineEdit* searchEdit_;
    QPushButton* addButton_;
    QTableWidget* table_;
};

#endif
