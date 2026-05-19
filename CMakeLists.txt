cmake_minimum_required(VERSION 3.16)

project(PharmacyManagementSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_HEADERS
    include/checkout_dialog.h
    include/dashboard_page.h
    include/date.h
    include/inventory_page.h
    include/inventory_service.h
    include/login_dialog.h
    include/main_window.h
    include/medicine.h
    include/medicine_status.h
    include/medicine_dialog.h
    include/medicine_repository.h
    include/order_service.h
    include/orders_page.h
    include/sqlite_medicine_repository.h
    include/text_medicine_repository.h
)

set(PROJECT_SOURCES
    resources.qrc
    src/checkout_dialog.cpp
    src/dashboard_page.cpp
    src/date.cpp
    src/inventory_page.cpp
    src/inventory_service.cpp
    src/login_dialog.cpp
    src/main.cpp
    src/main_window.cpp
    src/medicine.cpp
    src/medicine_status.cpp
    src/medicine_dialog.cpp
    src/orders_page.cpp
    src/order_service.cpp
    src/sqlite_medicine_repository.cpp
    src/text_medicine_repository.cpp
)

add_executable(pharmacy_gui
    ${PROJECT_HEADERS}
    ${PROJECT_SOURCES}
)

target_include_directories(pharmacy_gui PRIVATE include)
target_link_libraries(pharmacy_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
