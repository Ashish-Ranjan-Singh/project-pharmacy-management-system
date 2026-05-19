#include "inventory_service.h"

#include <algorithm>
#include <cctype>

bool InventoryService::addMedicine(const Medicine& medicine) {
    if (findById(medicine.id()) != 0) {
        return false;
    }

    medicines_.push_back(medicine);
    return true;
}

bool InventoryService::removeMedicineById(int id) {
    std::vector<Medicine>::iterator it =
        std::find_if(medicines_.begin(), medicines_.end(),
                     [id](const Medicine& medicine) { return medicine.id() == id; });

    if (it == medicines_.end()) {
        return false;
    }

    medicines_.erase(it);
    return true;
}

bool InventoryService::updateMedicine(const Medicine& medicine) {
    Medicine* existing = findById(medicine.id());
    if (existing == 0) {
        return false;
    }

    *existing = medicine;
    return true;
}

bool InventoryService::updateQuantity(int id, int quantity) {
    Medicine* medicine = findById(id);
    if (medicine == 0) {
        return false;
    }

    medicine->setQuantity(quantity);
    return true;
}

bool InventoryService::updatePrice(int id, int price) {
    Medicine* medicine = findById(id);
    if (medicine == 0) {
        return false;
    }

    medicine->setPrice(price);
    return true;
}

Medicine* InventoryService::findById(int id) {
    for (std::size_t i = 0; i < medicines_.size(); ++i) {
        if (medicines_[i].id() == id) {
            return &medicines_[i];
        }
    }

    return 0;
}

const Medicine* InventoryService::findById(int id) const {
    for (std::size_t i = 0; i < medicines_.size(); ++i) {
        if (medicines_[i].id() == id) {
            return &medicines_[i];
        }
    }

    return 0;
}

Medicine* InventoryService::findByName(const std::string& name) {
    const std::string normalizedName = normalizeText(name);

    for (std::size_t i = 0; i < medicines_.size(); ++i) {
        if (normalizeText(medicines_[i].name()) == normalizedName) {
            return &medicines_[i];
        }
    }

    return 0;
}

const Medicine* InventoryService::findByName(const std::string& name) const {
    const std::string normalizedName = normalizeText(name);

    for (std::size_t i = 0; i < medicines_.size(); ++i) {
        if (normalizeText(medicines_[i].name()) == normalizedName) {
            return &medicines_[i];
        }
    }

    return 0;
}

std::vector<Medicine>& InventoryService::medicines() {
    return medicines_;
}

const std::vector<Medicine>& InventoryService::medicines() const {
    return medicines_;
}

std::vector<Medicine> InventoryService::sortedByName() const {
    std::vector<Medicine> sorted = medicines_;
    std::sort(sorted.begin(), sorted.end(),
              [](const Medicine& left, const Medicine& right) {
                  return InventoryService::normalizeText(left.name()) <
                         InventoryService::normalizeText(right.name());
              });
    return sorted;
}

void InventoryService::replaceAll(const std::vector<Medicine>& medicines) {
    medicines_ = medicines;
}

void InventoryService::clear() {
    medicines_.clear();
}

std::string InventoryService::normalizeText(const std::string& value) {
    std::string normalized = value;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
    return normalized;
}
