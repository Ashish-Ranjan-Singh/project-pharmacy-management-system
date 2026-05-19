#include "text_medicine_repository.h"

#include <fstream>
#include <sstream>

namespace {
bool readLine(std::ifstream& input, std::string& line) {
    return static_cast<bool>(std::getline(input, line));
}

bool parseInt(const std::string& value, int& result) {
    std::istringstream stream(value);
    stream >> result;
    return !stream.fail() && stream.eof();
}
}

std::vector<Medicine> TextMedicineRepository::load(const std::string& filePath) const {
    std::ifstream input(filePath.c_str());
    std::vector<Medicine> medicines;

    if (!input.is_open()) {
        return medicines;
    }

    while (true) {
        std::string name;
        std::string company;
        std::string idText;
        std::string quantityText;
        std::string priceText;
        std::string typeText;
        std::string dayText;
        std::string monthText;
        std::string yearText;

        if (!readLine(input, name)) {
            break;
        }

        if (!readLine(input, company) ||
            !readLine(input, idText) ||
            !readLine(input, quantityText) ||
            !readLine(input, priceText) ||
            !readLine(input, typeText) ||
            !readLine(input, dayText) ||
            !readLine(input, monthText) ||
            !readLine(input, yearText)) {
            break;
        }

        int id = 0;
        int quantity = 0;
        int price = 0;
        int day = 0;
        int month = 0;
        int year = 0;
        MedicineType type = MedicineType::Tablet;

        if (!parseInt(idText, id) ||
            !parseInt(quantityText, quantity) ||
            !parseInt(priceText, price) ||
            !parseInt(dayText, day) ||
            !parseInt(monthText, month) ||
            !parseInt(yearText, year) ||
            !Medicine::tryParseType(typeText, type) ||
            !Date::isValid(day, month, year)) {
            continue;
        }

        medicines.push_back(
            Medicine(company, name, quantity, id, price, type, Date(day, month, year)));
    }

    return medicines;
}

bool TextMedicineRepository::save(const std::string& filePath,
                                  const std::vector<Medicine>& medicines) const {
    std::ofstream output(filePath.c_str(), std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    for (std::size_t i = 0; i < medicines.size(); ++i) {
        const Medicine& medicine = medicines[i];
        output << medicine.name() << '\n';
        output << medicine.company() << '\n';
        output << medicine.id() << '\n';
        output << medicine.quantity() << '\n';
        output << medicine.price() << '\n';
        output << Medicine::typeToString(medicine.type()) << '\n';
        output << medicine.expiryDate().day() << '\n';
        output << medicine.expiryDate().month() << '\n';
        output << medicine.expiryDate().year() << '\n';
    }

    return true;
}
