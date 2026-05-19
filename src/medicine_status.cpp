#include "medicine_status.h"

#include <QDate>

namespace {
QDate toQDate(const Date& date) {
    return QDate(date.year(), date.month(), date.day());
}
}

MedicineStatus calculateMedicineStatus(const Medicine& medicine) {
    const QDate today = QDate::currentDate();
    const QDate expiryDate = toQDate(medicine.expiryDate());

    if (expiryDate.isValid()) {
        if (expiryDate < today) {
            return MedicineStatus::Expired;
        }

        if (today.daysTo(expiryDate) <= 30) {
            return MedicineStatus::ExpiringSoon;
        }
    }

    if (medicine.quantity() == 0) {
        return MedicineStatus::OutOfStock;
    }

    if (medicine.quantity() < 10) {
        return MedicineStatus::LowStock;
    }

    return MedicineStatus::InStock;
}

std::string medicineStatusText(MedicineStatus status) {
    switch (status) {
    case MedicineStatus::InStock:
        return "In Stock";
    case MedicineStatus::LowStock:
        return "Low Stock";
    case MedicineStatus::ExpiringSoon:
        return "Expiring Soon";
    case MedicineStatus::Expired:
        return "Expired";
    }

    return "In Stock";
}
