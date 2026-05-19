#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <string>

class InventoryService;
class DashboardPage;
class InventoryPage;
class QListWidget;
class OrdersPage;
class QStackedWidget;
class SQLiteMedicineRepository;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(const std::string& role, QWidget* parent = 0);

private:
    void changePage(int row);

    std::string dataFilePath_;
    std::string role_;
    InventoryService* inventoryService_;
    SQLiteMedicineRepository* repository_;
    QListWidget* navigationList_;
    QStackedWidget* pageStack_;
    DashboardPage* dashboardPage_;
    InventoryPage* inventoryPage_;
    OrdersPage* ordersPage_;
};

#endif
