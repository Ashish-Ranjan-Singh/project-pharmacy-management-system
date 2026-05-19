#ifndef MEDICINE_REPOSITORY_H
#define MEDICINE_REPOSITORY_H

#include <string>
#include <vector>

#include "medicine.h"
#include "order_service.h"

class MedicineRepository {
public:
    virtual ~MedicineRepository() = default;

    // CRUD
    virtual bool addMedicine(const std::string& filePath, const Medicine& medicine) const = 0;
    virtual bool updateMedicine(const std::string& filePath, const Medicine& medicine) const = 0;
    virtual bool deleteMedicine(const std::string& filePath, int id) const = 0;

    // Core persistence
    virtual std::vector<Medicine> load(const std::string& filePath) const = 0;
    virtual bool save(const std::string& filePath,
                      const std::vector<Medicine>& medicines) const = 0;

    // Orders (optional default implementations)
    virtual bool saveOrder(const std::string&, const std::string&, double,
                           const std::vector<OrderItem>&) const {
        return false;
    }

    virtual bool saveCheckout(const std::string&, const std::vector<Medicine>&,
                              const std::string&, double,
                              const std::vector<OrderItem>&) const {
        return false;
    }

    virtual int orderCount(const std::string&) const { return 0; }
    virtual double totalRevenue(const std::string&) const { return 0.0; }
};

#endif