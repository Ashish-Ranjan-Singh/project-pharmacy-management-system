#ifndef TEXT_MEDICINE_REPOSITORY_H
#define TEXT_MEDICINE_REPOSITORY_H

#include "medicine_repository.h"

class TextMedicineRepository : public MedicineRepository {
public:
    std::vector<Medicine> load(const std::string& filePath) const override;
    bool save(const std::string& filePath,
              const std::vector<Medicine>& medicines) const override;
};

#endif
