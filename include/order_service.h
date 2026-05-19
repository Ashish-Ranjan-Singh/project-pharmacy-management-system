#ifndef ORDER_SERVICE_H
#define ORDER_SERVICE_H

#include <vector>

#include "inventory_service.h"

struct OrderItem {
    int medicineId;
    std::string medicineName;
    std::string companyName;
    MedicineType type;
    Date expiryDate;
    int unitPrice;
    int quantity;

    int totalPrice() const;
};

class OrderService {
public:
    bool addItem(const Medicine& medicine, int quantity);
    bool removeItemByMedicineId(int medicineId);
    void clear();

    const std::vector<OrderItem>& items() const;
    int total() const;
    bool checkout(InventoryService& inventory);

private:
    std::vector<OrderItem> items_;
};

#endif
