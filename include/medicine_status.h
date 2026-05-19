#ifndef MEDICINE_STATUS_H
#define MEDICINE_STATUS_H

#include <string>

#include "medicine.h"

enum class MedicineStatus {
    InStock,
    LowStock,
    ExpiringSoon,
    Expired,
    OutOfStock
};

MedicineStatus calculateMedicineStatus(const Medicine& medicine);
std::string medicineStatusText(MedicineStatus status);

#endif
