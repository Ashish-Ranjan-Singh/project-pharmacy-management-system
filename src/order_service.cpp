#include "order_service.h"

#include <algorithm>

int OrderItem::totalPrice() const {
    return unitPrice * quantity;
}

bool OrderService::addItem(const Medicine& medicine, int quantity) {
    if (quantity <= 0 || quantity > medicine.quantity()) {
        return false;
    }

    std::vector<OrderItem>::iterator existing =
        std::find_if(items_.begin(), items_.end(),
                     [&medicine](const OrderItem& item) {
                         return item.medicineId == medicine.id();
                     });

    if (existing != items_.end()) {
        if (existing->quantity + quantity > medicine.quantity()) {
            return false;
        }

        existing->quantity += quantity;
        return true;
    }

    OrderItem item;
    item.medicineId = medicine.id();
    item.medicineName = medicine.name();
    item.companyName = medicine.company();
    item.type = medicine.type();
    item.expiryDate = medicine.expiryDate();
    item.unitPrice = medicine.price();
    item.quantity = quantity;
    items_.push_back(item);
    return true;
}

bool OrderService::removeItemByMedicineId(int medicineId) {
    std::vector<OrderItem>::iterator it =
        std::find_if(items_.begin(), items_.end(),
                     [medicineId](const OrderItem& item) {
                         return item.medicineId == medicineId;
                     });

    if (it == items_.end()) {
        return false;
    }

    items_.erase(it);
    return true;
}

void OrderService::clear() {
    items_.clear();
}

const std::vector<OrderItem>& OrderService::items() const {
    return items_;
}

int OrderService::total() const {
    int runningTotal = 0;

    for (std::size_t i = 0; i < items_.size(); ++i) {
        runningTotal += items_[i].totalPrice();
    }

    return runningTotal;
}

bool OrderService::checkout(InventoryService& inventory) {
    if (items_.empty()) {
        return false;
    }

    for (std::size_t i = 0; i < items_.size(); ++i) {
        const OrderItem& item = items_[i];
        const Medicine* medicine = inventory.findById(item.medicineId);
        if (medicine == 0 || medicine->quantity() < item.quantity) {
            return false;
        }
    }

    for (std::size_t i = 0; i < items_.size(); ++i) {
        const OrderItem& item = items_[i];
        Medicine* medicine = inventory.findById(item.medicineId);
        medicine->setQuantity(medicine->quantity() - item.quantity);
    }

    clear();
    return true;
}
