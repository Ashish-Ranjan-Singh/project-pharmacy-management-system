#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>

#include <string>

class QLineEdit;
class QPushButton;
class SQLiteMedicineRepository;

class LoginDialog : public QDialog {
public:
    LoginDialog(SQLiteMedicineRepository& repository,
                const std::string& dataFilePath,
                QWidget* parent = 0);

    std::string authenticatedRole() const;

private:
    void attemptLogin();

    SQLiteMedicineRepository& repository_;
    std::string dataFilePath_;
    std::string authenticatedRole_;
    QLineEdit* usernameEdit_;
    QLineEdit* passwordEdit_;
    QPushButton* loginButton_;
    QPushButton* exitButton_;
};

#endif
