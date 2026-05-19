#include "medicine.h"

#include <algorithm>
#include <cctype>

namespace {
int sanitizeNonNegative(int value) {
    return value < 0 ? -value : value;
}
}

Medicine::Medicine()
    : quantity_(0), id_(0), price_(0), type_(MedicineType::Tablet), expiryDate_() {}

Medicine::Medicine(std::string company,
                   std::string name,
                   int quantity,
                   int id,
                   int price,
                   MedicineType type,
                   const Date& expiryDate)
    : company_(company),
      name_(name),
      quantity_(sanitizeNonNegative(quantity)),
      id_(sanitizeNonNegative(id)),
      price_(sanitizeNonNegative(price)),
      type_(type),
      expiryDate_(expiryDate) {}

const std::string& Medicine::company() const {
    return company_;
}

const std::string& Medicine::name() const {
    return name_;
}

int Medicine::quantity() const {
    return quantity_;
}

int Medicine::id() const {
    return id_;
}

int Medicine::price() const {
    return price_;
}

MedicineType Medicine::type() const {
    return type_;
}

const Date& Medicine::expiryDate() const {
    return expiryDate_;
}

void Medicine::setCompany(const std::string& company) {
    company_ = company;
}

void Medicine::setName(const std::string& name) {
    name_ = name;
}

void Medicine::setQuantity(int quantity) {
    quantity_ = sanitizeNonNegative(quantity);
}

void Medicine::setId(int id) {
    id_ = sanitizeNonNegative(id);
}

void Medicine::setPrice(int price) {
    price_ = sanitizeNonNegative(price);
}

void Medicine::setType(MedicineType type) {
    type_ = type;
}

void Medicine::setExpiryDate(const Date& expiryDate) {
    expiryDate_ = expiryDate;
}

const std::vector<MedicineType>& Medicine::availableTypes() {
    static const std::vector<MedicineType> kTypes = {
        MedicineType::Tablet,
        MedicineType::Liquid,
        MedicineType::Capsule,
        MedicineType::Syrup,
        MedicineType::Injection,
        MedicineType::Ointment
    };
    return kTypes;
}

std::string Medicine::typeToString(MedicineType type) {
    switch (type) {
    case MedicineType::Tablet:
        return "TABLET";
    case MedicineType::Liquid:
        return "LIQUID";
    case MedicineType::Capsule:
        return "CAPSULE";
    case MedicineType::Syrup:
        return "SYRUP";
    case MedicineType::Injection:
        return "INJECTION";
    case MedicineType::Ointment:
        return "OINTMENT";
    }

    return "TABLET";
}

bool Medicine::tryParseType(const std::string& value, MedicineType& type) {
    std::string normalized = value;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });

    if (normalized == "TABLET") {
        type = MedicineType::Tablet;
        return true;
    }

    if (normalized == "LIQUID") {
        type = MedicineType::Liquid;
        return true;
    }

    if (normalized == "CAPSULE") {
        type = MedicineType::Capsule;
        return true;
    }

    if (normalized == "SYRUP") {
        type = MedicineType::Syrup;
        return true;
    }

    if (normalized == "INJECTION") {
        type = MedicineType::Injection;
        return true;
    }

    if (normalized == "OINTMENT") {
        type = MedicineType::Ointment;
        return true;
    }

    return false;
}
