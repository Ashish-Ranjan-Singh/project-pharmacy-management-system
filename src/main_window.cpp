#include "main_window.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QWidget>

#include "dashboard_page.h"
#include "inventory_service.h"
#include "inventory_page.h"
#include "orders_page.h"
#include "sqlite_medicine_repository.h"

MainWindow::MainWindow(const std::string& role, QWidget* parent)
    : QMainWindow(parent),
      dataFilePath_("pharmacy.db"),
      role_(role),
      inventoryService_(new InventoryService()),
      repository_(new SQLiteMedicineRepository()),
      navigationList_(new QListWidget(this)),
      pageStack_(new QStackedWidget(this)),
      dashboardPage_(new DashboardPage(*inventoryService_, *repository_, dataFilePath_, this)),
      inventoryPage_(new InventoryPage(*inventoryService_,
                                       *repository_,
                                       dataFilePath_,
                                       role_ == "admin",
                                       this)),
      ordersPage_(new OrdersPage(*inventoryService_, *repository_, dataFilePath_, this)) {
    QWidget* central = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(central);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(18);

    inventoryService_->replaceAll(repository_->load(dataFilePath_));

    navigationList_->setIconSize(QSize(18, 18));
    if (role_ == "admin") {
        navigationList_->addItem(new QListWidgetItem(QIcon(":/resources/icons/dashboard.svg"), "Dashboard"));
    }
    navigationList_->addItem(new QListWidgetItem(QIcon(":/resources/icons/inventory.svg"), "Inventory"));
    navigationList_->addItem(new QListWidgetItem(QIcon(":/resources/icons/orders.svg"), "Orders"));
    navigationList_->setFixedWidth(200);
    navigationList_->setCurrentRow(0);

    pageStack_->setObjectName("pageStack");

    if (role_ == "admin") {
        pageStack_->addWidget(dashboardPage_);
    }
    pageStack_->addWidget(inventoryPage_);
    pageStack_->addWidget(ordersPage_);

    layout->addWidget(navigationList_);
    layout->addWidget(pageStack_, 1);

    connect(navigationList_, &QListWidget::currentRowChanged, this, &MainWindow::changePage);

    setWindowTitle("Pharmacy Inventory");
    resize(1120, 680);
    setCentralWidget(central);
    setStyleSheet(styleSheet() +
                  "#pageStack { background-color: #0f1722; border: 1px solid #223243; border-radius: 18px; }");
}

void MainWindow::changePage(int row) {
    if (row < 0 || row >= pageStack_->count()) {
        return;
    }

    if (role_ == "admin") {
        dashboardPage_->refreshView();
    }
    inventoryPage_->refreshView();
    ordersPage_->refreshView();
    pageStack_->setCurrentIndex(row);
}
