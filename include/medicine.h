#ifndef MEDICINE_H
#define MEDICINE_H

#include <string>
#include <vector>

#include "date.h"

enum class MedicineType {
    Tablet,
    Liquid,
    Capsule,
    Syrup,
    Injection,
    Ointment
};

class Medicine {
public:
    Medicine();
    Medicine(std::string company,
             std::string name,
             int quantity,
             int id,
             int price,
             MedicineType type,
             const Date& expiryDate);

    const std::string& company() const;
    const std::string& name() const;
    int quantity() const;
    int id() const;
    int price() const;
    MedicineType type() const;
    const Date& expiryDate() const;

    void setCompany(const std::string& company);
    void setName(const std::string& name);
    void setQuantity(int quantity);
    void setId(int id);
    void setPrice(int price);
    void setType(MedicineType type);
    void setExpiryDate(const Date& expiryDate);

    static const std::vector<MedicineType>& availableTypes();
    static std::string typeToString(MedicineType type);
    static bool tryParseType(const std::string& value, MedicineType& type);

private:
    std::string company_;
    std::string name_;
    int quantity_;
    int id_;
    int price_;
    MedicineType type_;
    Date expiryDate_;
};

#endif
