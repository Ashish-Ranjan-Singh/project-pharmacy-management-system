#include "login_dialog.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "sqlite_medicine_repository.h"

LoginDialog::LoginDialog(SQLiteMedicineRepository& repository,
                         const std::string& dataFilePath,
                         QWidget* parent)
    : QDialog(parent),
      repository_(repository),
      dataFilePath_(dataFilePath),
      usernameEdit_(new QLineEdit(this)),
      passwordEdit_(new QLineEdit(this)),
      loginButton_(new QPushButton("Login", this)),
      exitButton_(new QPushButton("Exit", this)) {
    setWindowTitle("Pharmacy Login");
    setModal(true);
    resize(420, 300);

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    QFrame* card = new QFrame(this);
    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    QLabel* titleLabel = new QLabel("Pharmacy Management", this);
    QLabel* subtitleLabel = new QLabel("Sign in to continue", this);
    QLabel* usernameLabel = new QLabel("Username", this);
    QLabel* passwordLabel = new QLabel("Password", this);
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    titleLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #f6fbff;");
    subtitleLabel->setStyleSheet("color: #8fa2b7;");
    card->setStyleSheet("background-color: #121c27; border: 1px solid #233243; border-radius: 18px;");

    usernameEdit_->setPlaceholderText("Enter username");
    passwordEdit_->setPlaceholderText("Enter password");
    passwordEdit_->setEchoMode(QLineEdit::Password);
    loginButton_->setIcon(QIcon(":/resources/icons/dashboard.svg"));
    exitButton_->setProperty("class", "secondary");

    rootLayout->setContentsMargins(24, 24, 24, 24);
    rootLayout->addStretch();
    rootLayout->addWidget(card);
    rootLayout->addStretch();

    cardLayout->setContentsMargins(22, 22, 22, 22);
    cardLayout->setSpacing(10);
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(subtitleLabel);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(usernameLabel);
    cardLayout->addWidget(usernameEdit_);
    cardLayout->addWidget(passwordLabel);
    cardLayout->addWidget(passwordEdit_);
    cardLayout->addSpacing(8);

    buttonLayout->addWidget(exitButton_);
    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton_);
    cardLayout->addLayout(buttonLayout);

    connect(loginButton_, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
    connect(exitButton_, &QPushButton::clicked, this, &QDialog::reject);
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
}

std::string LoginDialog::authenticatedRole() const {
    return authenticatedRole_;
}

void LoginDialog::attemptLogin() {
    const std::string username = usernameEdit_->text().trimmed().toStdString();
    const std::string password = passwordEdit_->text().toStdString();
    std::string role;

    if (username.empty() || password.empty()) {
        QMessageBox::warning(this, "Missing Credentials",
                             "Enter both username and password.");
        return;
    }

    if (!repository_.authenticateUser(dataFilePath_, username, password, role)) {
        QMessageBox::warning(this, "Login Failed",
                             "Invalid username or password.");
        passwordEdit_->clear();
        passwordEdit_->setFocus();
        return;
    }

    authenticatedRole_ = role;
    accept();
}
