#ifndef ORDERS_PAGE_H
#define ORDERS_PAGE_H

#include <QWidget>

#include <string>
#include <vector>

#include "inventory_service.h"
#include "medicine_repository.h"
#include "order_service.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QTableWidget;

class OrdersPage : public QWidget {
public:
    explicit OrdersPage(InventoryService& inventoryService,
                        MedicineRepository& repository,
                        const std::string& dataFilePath,
                        QWidget* parent = 0);

    void refreshView();

private:
    bool saveInventory();
    void refreshMedicineTable();
    void refreshCartTable();
    void refreshTotal();
    void updateFilter(const QString& text);
    void addToCart(int medicineId);
    void removeFromCart(int medicineId);
    void clearCart();
    void checkout();
    void restoreCart(const std::vector<OrderItem>& items);

    InventoryService& inventoryService_;
    MedicineRepository& repository_;
    std::string dataFilePath_;
    OrderService orderService_;
    QLineEdit* searchEdit_;
    QTableWidget* medicinesTable_;
    QTableWidget* cartTable_;
    QLabel* totalValueLabel_;
    QPushButton* clearCartButton_;
    QPushButton* checkoutButton_;
};

#endif
