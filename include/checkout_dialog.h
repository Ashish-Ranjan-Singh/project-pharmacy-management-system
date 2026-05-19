#ifndef CHECKOUT_DIALOG_H
#define CHECKOUT_DIALOG_H

#include <QDialog>

#include <string>
#include <vector>

#include "order_service.h"

class QLabel;
class QPushButton;
class QTableWidget;

class CheckoutDialog : public QDialog {
public:
    CheckoutDialog(const std::vector<OrderItem>& items,
                   int total,
                   const std::string& dateTimeText,
                   QWidget* parent = 0);

private:
    std::string buildReceiptText() const;
    void exportReceipt();

    std::vector<OrderItem> items_;
    int total_;
    std::string dateTimeText_;
    QTableWidget* table_;
    QLabel* totalLabel_;
    QLabel* dateTimeLabel_;
    QPushButton* exportButton_;
};

#endif
