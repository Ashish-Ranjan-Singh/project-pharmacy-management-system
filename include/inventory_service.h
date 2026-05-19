#ifndef INVENTORY_SERVICE_H
#define INVENTORY_SERVICE_H

#include <string>
#include <vector>

#include "medicine.h"

class InventoryService {
public:
    bool addMedicine(const Medicine& medicine);
    bool removeMedicineById(int id);
    bool updateMedicine(const Medicine& medicine);
    bool updateQuantity(int id, int quantity);
    bool updatePrice(int id, int price);

    Medicine* findById(int id);
    const Medicine* findById(int id) const;
    Medicine* findByName(const std::string& name);
    const Medicine* findByName(const std::string& name) const;

    std::vector<Medicine>& medicines();
    const std::vector<Medicine>& medicines() const;
    std::vector<Medicine> sortedByName() const;

    void replaceAll(const std::vector<Medicine>& medicines);
    void clear();

private:
    static std::string normalizeText(const std::string& value);

    std::vector<Medicine> medicines_;
};

#endif
