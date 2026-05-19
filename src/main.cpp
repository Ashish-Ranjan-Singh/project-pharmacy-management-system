#include <QApplication>
#include <QFile>
#include <QFont>
#include <QTextStream>

#include "login_dialog.h"
#include "main_window.h"
#include "sqlite_medicine_repository.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setFont(QFont("Segoe UI", 10));
    QFile styleFile(":/resources/styles/app.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&styleFile);
        app.setStyleSheet(stream.readAll());
        styleFile.close();
    }
    SQLiteMedicineRepository repository;
    const std::string dataFilePath = "pharmacy.db";
    LoginDialog loginDialog(repository, dataFilePath);
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow window(loginDialog.authenticatedRole());
    window.show();
    return app.exec();
}
