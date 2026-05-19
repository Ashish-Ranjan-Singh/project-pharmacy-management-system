#include "sqlite_medicine_repository.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QCryptographicHash>
#include <QUuid>

namespace {
QString toQString(const std::string& value) {
    return QString::fromStdString(value);
}
}

SQLiteMedicineRepository::SQLiteMedicineRepository()
    : connectionName_(new QString(QUuid::createUuid().toString())) {
        ensureOpen("pharmacy.db");
        seedMedicinesIfEmpty();
    }

SQLiteMedicineRepository::~SQLiteMedicineRepository() {
    const QString connectionName = *connectionName_;
    if (QSqlDatabase::contains(connectionName)) {
        {
            QSqlDatabase database = QSqlDatabase::database(connectionName);
            if (database.isOpen()) {
                database.close();
            }
        }
        QSqlDatabase::removeDatabase(connectionName);
    }
    delete connectionName_;
}

std::vector<Medicine> SQLiteMedicineRepository::load(const std::string& filePath) const {
    return loadAll(filePath);
}

bool SQLiteMedicineRepository::save(const std::string& filePath,
                                    const std::vector<Medicine>& medicines) const{
    return saveAll(filePath, medicines);
}

std::vector<Medicine> SQLiteMedicineRepository::loadAll(const std::string& filePath) const {
    std::vector<Medicine> medicines;
    if (!ensureOpen(filePath)) {
        return medicines;
    }

    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    QSqlQuery query(database);
    if (!query.exec("SELECT id, name, company, type, quantity, price, expiry FROM medicines ORDER BY name ASC")) {
        logQueryError("load medicines", query);
        return medicines;
    }

    while (query.next()) {
        MedicineType type = MedicineType::Tablet;
        const std::string typeValue = query.value(3).toString().toStdString();
        if (!Medicine::tryParseType(typeValue, type)) {
            continue;
        }

        const QString expiry = query.value(6).toString();
        const QStringList expiryParts = expiry.split('-');
        if (expiryParts.size() != 3) {
            continue;
        }

        const int year = expiryParts[0].toInt();
        const int month = expiryParts[1].toInt();
        const int day = expiryParts[2].toInt();
        if (!Date::isValid(day, month, year)) {
            continue;
        }

        medicines.push_back(Medicine(query.value(2).toString().toStdString(),
                                     query.value(1).toString().toStdString(),
                                     query.value(4).toInt(),
                                     query.value(0).toInt(),
                                     query.value(5).toInt(),
                                     type,
                                     Date(day, month, year)));
    }

    return medicines;
}

bool SQLiteMedicineRepository::saveAll(const std::string& filePath,
                                       const std::vector<Medicine>& medicines) const{
    if (!ensureOpen(filePath)) {
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    if (!database.transaction()) {
        qWarning() << "Failed to start transaction:" << database.lastError().text();
        return false;
    }

    QSqlQuery deleteQuery(database);
    if (!deleteQuery.exec("DELETE FROM medicines")) {
        logQueryError("clear medicines", deleteQuery);
        database.rollback();
        return false;
    }

    for (std::size_t i = 0; i < medicines.size(); ++i) {
        if (!addMedicine(filePath, medicines[i])) {
            database.rollback();
            return false;
        }
    }

    if (!database.commit()) {
        qWarning() << "Failed to commit transaction:" << database.lastError().text();
        database.rollback();
        return false;
    }

    return true;
}

bool SQLiteMedicineRepository::addMedicine(const std::string& filePath,
                                           const Medicine& medicine) const {
    if (!ensureOpen(filePath)) {
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    QSqlQuery query(database);
    query.prepare(
        "INSERT INTO medicines (name, company, type, quantity, price, expiry) "
        "VALUES (:name, :company, :type, :quantity, :price, :expiry)");
    query.bindValue(":name", toQString(medicine.name()));
    query.bindValue(":company", toQString(medicine.company()));
    query.bindValue(":type", toQString(Medicine::typeToString(medicine.type())));
    query.bindValue(":quantity", medicine.quantity());
    query.bindValue(":price", medicine.price());
    query.bindValue(":expiry",
                    QString("%1-%2-%3")
                        .arg(medicine.expiryDate().year(), 4, 10, QChar('0'))
                        .arg(medicine.expiryDate().month(), 2, 10, QChar('0'))
                        .arg(medicine.expiryDate().day(), 2, 10, QChar('0')));

    if (!query.exec()) {
        logQueryError("insert medicine", query);
        return false;
    }

    return true;
}

bool SQLiteMedicineRepository::updateMedicine(const std::string& filePath,
                                              const Medicine& medicine) const {
    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    qDebug() << "Updating ID:" << medicine.id();

    if (!database.isOpen()) {
        qDebug() << "Database not open!";
        return false;
    }

    QSqlQuery query(database);
    query.prepare(
        "UPDATE medicines SET name = :name, company = :company, type = :type, "
        "quantity = :quantity, price = :price, expiry = :expiry WHERE id = :id");
    query.bindValue(":id", medicine.id());
    query.bindValue(":name", toQString(medicine.name()));
    query.bindValue(":company", toQString(medicine.company()));
    query.bindValue(":type", toQString(Medicine::typeToString(medicine.type())));
    query.bindValue(":quantity", medicine.quantity());
    query.bindValue(":price", medicine.price());
    query.bindValue(":expiry",
                    QString("%1-%2-%3")
                        .arg(medicine.expiryDate().year(), 4, 10, QChar('0'))
                        .arg(medicine.expiryDate().month(), 2, 10, QChar('0'))
                        .arg(medicine.expiryDate().day(), 2, 10, QChar('0')));

    if (!query.exec()) {
    qDebug() << "UPDATE FAILED!";
    qDebug() << query.lastError();
    qDebug() << "ID:" << medicine.id();
    qDebug() << "TYPE:" << QString::fromStdString(Medicine::typeToString(medicine.type()));
    return false;
    }

    return true;
}

bool SQLiteMedicineRepository::deleteMedicine(const std::string& filePath, int id) const {
    if (!ensureOpen(filePath)) {
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    QSqlQuery query(database);
    query.prepare("DELETE FROM medicines WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        logQueryError("delete medicine", query);
        return false;
    }

    return true;
}

bool SQLiteMedicineRepository::ensureOpen(const std::string& filePath) const {
    QSqlDatabase database;
    if (QSqlDatabase::contains(*connectionName_)) {
        database = QSqlDatabase::database(*connectionName_);
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE", *connectionName_);
    }

    const QString databasePath = toQString(filePath);
    if (database.databaseName() != databasePath) {
        if (database.isOpen()) {
            database.close();
        }
        database.setDatabaseName(databasePath);
    }

    if (!database.isOpen() && !database.open()) {
        qWarning() << "Failed to open SQLite database:" << database.lastError().text();
        return false;
    }

    return ensureSchema();
}

bool SQLiteMedicineRepository::ensureSchema() const {
    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    QSqlQuery query(database);
    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS medicines ("
            "id INTEGER PRIMARY KEY, "
            "name TEXT NOT NULL, "
            "company TEXT NOT NULL, "
            "type TEXT NOT NULL, "
            "quantity INTEGER NOT NULL, "
            "price REAL NOT NULL, "
            "expiry TEXT NOT NULL)")) {
        logQueryError("create medicines table", query);
        return false;
    }

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS orders ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "datetime TEXT NOT NULL, "
            "total REAL NOT NULL)")) {
        logQueryError("create orders table", query);
        return false;
    }

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS order_items ("
            "order_id INTEGER NOT NULL, "
            "medicine_name TEXT NOT NULL, "
            "quantity INTEGER NOT NULL, "
            "price REAL NOT NULL)")) {
        logQueryError("create order_items table", query);
        return false;
    }

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "username TEXT UNIQUE NOT NULL, "
            "password TEXT NOT NULL, "
            "role TEXT NOT NULL)")) {
        logQueryError("create users table", query);
        return false;
    }

    if (!ensureDefaultAdmin()) {
        return false;
    }

    return true;
}

void SQLiteMedicineRepository::logQueryError(const char* operation, const QSqlQuery& query) const {
    qWarning() << "SQLite operation failed:" << operation << query.lastError().text();
}

bool SQLiteMedicineRepository::saveOrder(const std::string& filePath,
                                         const std::string& dateTime,
                                         double total,
                                         const std::vector<OrderItem>& items) const {
    if (!ensureOpen(filePath)) {
        return false;
    }

    int orderId = 0;
    return insertOrderRecord(dateTime, total, items, orderId);
}

bool SQLiteMedicineRepository::saveCheckout(const std::string& filePath,
                                            const std::vector<Medicine>& medicines,
                                            const std::string& dateTime,
                                            double total,
                                            const std::vector<OrderItem>& items) const {
    if (!ensureOpen(filePath)) {
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    qDebug() << "Checkout started with" << items.size() << "items, total=" << total;

    if (!database.transaction()) {
        qWarning() << "Failed to start checkout transaction:" << database.lastError().text();
        return false;
    }

    for (const auto& item : items) {
        qDebug() << "Updating stock for medicine id:" << item.medicineId
                 << "quantity:" << item.quantity;

        QSqlQuery updateQuery(database);
        updateQuery.prepare(
            "UPDATE medicines SET quantity = quantity - :qty "
            "WHERE id = :id AND quantity >= :qty");
        updateQuery.bindValue(":qty", item.quantity);
        updateQuery.bindValue(":id", item.medicineId);

        if (!updateQuery.exec()) {
            logQueryError("update quantity", updateQuery);
            database.rollback();
            return false;
        }

        if (updateQuery.numRowsAffected() != 1) {
            qWarning() << "Insufficient stock or invalid medicine id for checkout:" 
                       << item.medicineId << "requested:" << item.quantity;
            database.rollback();
            return false;
        }
    }

    int orderId = 0;
    if (!insertOrderRecord(dateTime, total, items, orderId)) {
        qWarning() << "Failed to save order record during checkout";
        database.rollback();
        return false;
    }

    if (!database.commit()) {
        qWarning() << "Failed to commit checkout transaction:" << database.lastError().text();
        database.rollback();
        return false;
    }

    qDebug() << "Checkout transaction committed successfully. Order ID:" << orderId;
    return true;
}

bool SQLiteMedicineRepository::insertOrderRecord(const std::string& dateTime,
                                                 double total,
                                                 const std::vector<OrderItem>& items,
                                                 int& orderId) const {
    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    QSqlQuery orderQuery(database);
    orderQuery.prepare("INSERT INTO orders (datetime, total) VALUES (:datetime, :total)");
    orderQuery.bindValue(":datetime", toQString(dateTime));
    orderQuery.bindValue(":total", total);

    if (!orderQuery.exec()) {
        logQueryError("insert order", orderQuery);
        return false;
    }

    orderId = orderQuery.lastInsertId().toInt();

    for (std::size_t i = 0; i < items.size(); ++i) {
        const OrderItem& item = items[i];
        QSqlQuery itemQuery(database);
        itemQuery.prepare(
            "INSERT INTO order_items (order_id, medicine_name, quantity, price) "
            "VALUES (:order_id, :medicine_name, :quantity, :price)");
        itemQuery.bindValue(":order_id", orderId);
        itemQuery.bindValue(":medicine_name", toQString(item.medicineName));
        itemQuery.bindValue(":quantity", item.quantity);
        itemQuery.bindValue(":price", item.totalPrice());

        if (!itemQuery.exec()) {
            logQueryError("insert order item", itemQuery);
            return false;
        }
    }

    return true;
}

int SQLiteMedicineRepository::orderCount(const std::string& filePath) const {
    if (!ensureOpen(filePath)) {
        return 0;
    }

    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    QSqlQuery query(database);
    if (!query.exec("SELECT COUNT(*) FROM orders")) {
        logQueryError("count orders", query);
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

double SQLiteMedicineRepository::totalRevenue(const std::string& filePath) const {
    if (!ensureOpen(filePath)) {
        return 0.0;
    }

    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    QSqlQuery query(database);
    if (!query.exec("SELECT COALESCE(SUM(total), 0) FROM orders")) {
        logQueryError("sum order revenue", query);
        return 0.0;
    }

    if (query.next()) {
        return query.value(0).toDouble();
    }

    return 0.0;
}

bool SQLiteMedicineRepository::authenticateUser(const std::string& filePath,
                                                const std::string& username,
                                                const std::string& password,
                                                std::string& role) const {
    if (!ensureOpen(filePath)) {
        return false;
    }

    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    QSqlQuery query(database);
    query.prepare("SELECT role FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", toQString(username));
    query.bindValue(":password", toQString(hashPassword(password)));

    if (!query.exec()) {
        logQueryError("authenticate user", query);
        return false;
    }

    if (!query.next()) {
        return false;
    }

    role = query.value(0).toString().toStdString();
    return true;
}

std::string SQLiteMedicineRepository::hashPassword(const std::string& password) const {
    const QByteArray hash =
        QCryptographicHash::hash(QByteArray::fromStdString(password), QCryptographicHash::Sha256);
    return hash.toHex().toStdString();
}

bool SQLiteMedicineRepository::ensureDefaultAdmin() const {
    QSqlDatabase database = QSqlDatabase::database(*connectionName_);

    // 🔹 Check admin exists
    QSqlQuery checkAdmin(database);
    checkAdmin.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    checkAdmin.bindValue(":username", "admin");

    if (!checkAdmin.exec() || !checkAdmin.next()) {
        return false;
    }

    if (checkAdmin.value(0).toInt() == 0) {
        QSqlQuery insertAdmin(database);
        insertAdmin.prepare(
            "INSERT INTO users (username, password, role) VALUES (:username, :password, :role)");
        insertAdmin.bindValue(":username", "admin");
        insertAdmin.bindValue(":password", toQString(hashPassword("admin123")));
        insertAdmin.bindValue(":role", "admin");

        if (!insertAdmin.exec()) {
            logQueryError("insert admin", insertAdmin);
            return false;
        }
    }

    // 🔹 Check staff exists
    QSqlQuery checkStaff(database);
    checkStaff.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    checkStaff.bindValue(":username", "staff");

    if (!checkStaff.exec() || !checkStaff.next()) {
        return false;
    }

    if (checkStaff.value(0).toInt() == 0) {
        QSqlQuery insertStaff(database);
        insertStaff.prepare(
            "INSERT INTO users (username, password, role) VALUES (:username, :password, :role)");
        insertStaff.bindValue(":username", "staff");
        insertStaff.bindValue(":password", toQString(hashPassword("1234")));
        insertStaff.bindValue(":role", "staff");

        if (!insertStaff.exec()) {
            logQueryError("insert staff", insertStaff);
            return false;
        }
    }

    return true;
}

bool SQLiteMedicineRepository::seedMedicinesIfEmpty() const {
    QSqlDatabase database = QSqlDatabase::database(*connectionName_);
    QSqlQuery countQuery(database);
    if (!countQuery.exec("SELECT COUNT(*) FROM medicines")) {
        logQueryError("count medicines", countQuery);
        return false;
    }

    if (countQuery.next() && countQuery.value(0).toInt() > 0) {
        return true;
    }

    qDebug() << "Seeding medicines...";

    struct SeedMedicine {
        const char* name;
        const char* company;
        const char* type;
        int quantity;
        double price;
        const char* expiry;
    };

    const SeedMedicine medicines[] = {
        {"Paracetamol", "Pharma", "TABLET", 100, 20, "2026-12-01"},
        {"Crocin", "GSK", "TABLET", 80, 25, "2026-10-15"},
        {"Dolo 650", "Micro Labs", "TABLET", 120, 30, "2027-01-01"},
        {"Ibuprofen", "Abbott", "TABLET", 90, 40, "2026-06-15"},
        {"Vitamin C", "Himalaya", "TABLET", 130, 25, "2026-07-22"},

        {"Insulin", "Novo Nordisk", "LIQUID", 5, 500, "2026-12-31"},
        {"Azithromycin", "Cipla", "TABLET", 7, 120, "2026-08-10"},
        {"ORS", "Dabur", "LIQUID", 3, 50, "2026-09-01"},
        {"Amoxicillin", "Sun Pharma", "TABLET", 9, 80, "2027-03-20"},

        {"OldParacetamol", "TestPharma", "TABLET", 20, 10, "2022-01-01"},
        {"ExpiredCough", "TestPharma", "LIQUID", 15, 60, "2023-05-10"},
        {"ExpiredAntibiotic", "TestPharma", "TABLET", 10, 90, "2023-12-01"},

        {"SoonExpire1", "Cipla", "TABLET", 25, 35, "2026-04-25"},
        {"SoonExpire2", "Sun Pharma", "TABLET", 40, 45, "2026-05-01"},
        {"SoonExpire3", "Abbott", "TABLET", 30, 50, "2026-05-10"},

        {"Metformin", "Sun Pharma", "TABLET", 95, 35, "2026-05-20"},
        {"Aspirin", "Bayer", "TABLET", 100, 22, "2026-04-10"},
        {"Pantoprazole", "Cipla", "TABLET", 65, 90, "2026-10-05"},
        {"Omeprazole", "Dr Reddy", "TABLET", 70, 85, "2027-01-18"},
        {"Zincovit", "Apex", "TABLET", 60, 95, "2026-08-30"},
        {"Benadryl", "Johnson", "LIQUID", 55, 110, "2026-12-15"},
        {"Digene", "Abbott", "LIQUID", 45, 75, "2027-03-01"},
        {"Loperamide", "Cipla", "TABLET", 50, 60, "2026-11-12"},

        {"Cetirizine", "Dr Morepen", "TABLET", 85, 18, "2026-09-17"},
        {"Calpol Syrup", "GSK", "LIQUID", 26, 65, "2026-05-08"},
        {"ORS Apple", "Electral", "LIQUID", 8, 45, "2026-05-14"},
        {"Liv 52", "Himalaya", "TABLET", 75, 130, "2026-11-05"},
        {"Cough Relief", "Dabur", "LIQUID", 32, 95, "2026-05-05"}
    };

    if (!database.transaction()) {
        qWarning() << "Failed to start medicine seed transaction:" << database.lastError().text();
        return false;
    }

    QSqlQuery insertQuery(database);
    insertQuery.prepare(
        "INSERT INTO medicines (name, company, type, quantity, price, expiry) "
        "VALUES (:name, :company, :type, :quantity, :price, :expiry)");

    for (std::size_t i = 0; i < sizeof(medicines) / sizeof(medicines[0]); ++i) {
        insertQuery.bindValue(":name", medicines[i].name);
        insertQuery.bindValue(":company", medicines[i].company);
        insertQuery.bindValue(":type", medicines[i].type);
        insertQuery.bindValue(":quantity", medicines[i].quantity);
        insertQuery.bindValue(":price", medicines[i].price);
        insertQuery.bindValue(":expiry", medicines[i].expiry);

        if (!insertQuery.exec()) {
            logQueryError("seed medicine", insertQuery);
            database.rollback();
            return false;
        }
    }

    if (!database.commit()) {
        qWarning() << "Failed to commit medicine seed transaction:" << database.lastError().text();
        database.rollback();
        return false;
    }

    qDebug() << "Seeding completed";
    return true;
}
