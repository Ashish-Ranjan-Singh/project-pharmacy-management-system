#ifndef SQLITE_MEDICINE_REPOSITORY_H
#define SQLITE_MEDICINE_REPOSITORY_H

#include <string>
#include <vector>

#include "medicine_repository.h"

class QString;

class SQLiteMedicineRepository : public MedicineRepository {
public:
    SQLiteMedicineRepository();
    ~SQLiteMedicineRepository() override;

    std::vector<Medicine> load(const std::string& filePath) const override;
    bool save(const std::string& filePath,
              const std::vector<Medicine>& medicines) const override;

    std::vector<Medicine> loadAll(const std::string& filePath) const;
    bool saveAll(const std::string& filePath,
                 const std::vector<Medicine>& medicines) const;

    bool addMedicine(const std::string& filePath, const Medicine& medicine) const override;
    bool updateMedicine(const std::string& filePath, const Medicine& medicine) const override;
    bool deleteMedicine(const std::string& filePath, int id) const override;
    bool saveOrder(const std::string& filePath,
                   const std::string& dateTime,
                   double total,
                   const std::vector<OrderItem>& items) const override;
    bool saveCheckout(const std::string& filePath,
                      const std::vector<Medicine>& medicines,
                      const std::string& dateTime,
                      double total,
                      const std::vector<OrderItem>& items) const override;
    int orderCount(const std::string& filePath) const override;
    double totalRevenue(const std::string& filePath) const override;
    bool authenticateUser(const std::string& filePath,
                          const std::string& username,
                          const std::string& password,
                          std::string& role) const;
    std::string hashPassword(const std::string& password) const;

private:
    bool ensureOpen(const std::string& filePath) const;
    bool ensureSchema() const;
    bool ensureDefaultAdmin() const;
    bool seedMedicinesIfEmpty() const;
    bool insertOrderRecord(const std::string& dateTime,
                           double total,
                           const std::vector<OrderItem>& items,
                           int& orderId) const;
    void logQueryError(const char* operation, const class QSqlQuery& query) const;

    mutable QString* connectionName_;
};

#endif
