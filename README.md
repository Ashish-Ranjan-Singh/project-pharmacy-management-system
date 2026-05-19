# Pharmacy Management System

A modern desktop Pharmacy Management System built with `C++`, `Qt Widgets`, `SQLite`, and `CMake`.

This project started as a terminal-based C++ OOP application and was refactored into a layered Qt GUI application with authentication, dashboard analytics, inventory management, order processing, checkout, receipt generation, and SQLite persistence.

## Overview

The application is designed to help manage pharmacy operations through a clean desktop interface. It provides:

- secure login with role-based access
- dashboard summary metrics
- medicine inventory management
- stock and expiry alerts
- cart and checkout workflow
- receipt preview and export
- SQLite-based persistent storage

## Features

### 1. Authentication System

- Login dialog with username and password
- Password hashing using `SHA-256`
- SQLite-backed user authentication
- Role-based access control
- Default roles:
  - `admin`: full access
  - `staff`: limited access

### 2. Dashboard

- Total medicines
- Low stock item count
- Total orders
- Total revenue

### 3. Inventory Management

- Add medicines
- Edit medicines
- Delete medicines
- Search by medicine/company/ID
- Status indicators:
  - `In Stock`
  - `Low Stock`
  - `Expiring Soon`
  - `Expired`
- Color-coded rows for stock/expiry visibility

### 4. Orders System

- Browse medicines
- Add items to cart
- Remove items from cart
- Real-time total price calculation
- Checkout workflow

### 5. Receipt System

- Receipt preview dialog before checkout
- Shows medicine, quantity, price, total, and date/time
- Export receipt to `.txt`

### 6. Database

SQLite database file: `pharmacy.db`

Tables:

- `medicines`
- `orders`
- `order_items`
- `users`

### 7. UI/UX

- Qt Widgets desktop application
- Dark theme using `Fusion` + QSS
- Sidebar navigation
- Dashboard, Inventory, and Orders pages
- Reusable dialogs:
  - Login dialog
  - Medicine form dialog
  - Checkout/receipt dialog

## Tech Stack

- `C++11`
- `Qt Widgets`
- `Qt SQL`
- `SQLite`
- `CMake`
- `MinGW`
- `OOP / layered architecture`

## Architecture

The application follows a layered architecture with separation of concerns:

```text
+---------------------------+
|         UI Layer          |
|---------------------------|
| MainWindow                |
| DashboardPage             |
| InventoryPage             |
| OrdersPage                |
| LoginDialog               |
| MedicineDialog            |
| CheckoutDialog            |
+-------------+-------------+
              |
              v
+---------------------------+
|      Service Layer        |
|---------------------------|
| InventoryService          |
| OrderService              |
+-------------+-------------+
              |
              v
+---------------------------+
|     Repository Layer      |
|---------------------------|
| MedicineRepository        |
| SQLiteMedicineRepository  |
+-------------+-------------+
              |
              v
+---------------------------+
|      Persistence Layer    |
|---------------------------|
| SQLite Database           |
| pharmacy.db               |
+---------------------------+
```

## Project Structure

```text
Pharmacy-Management-System-master/
├── include/
│   ├── checkout_dialog.h
│   ├── dashboard_page.h
│   ├── date.h
│   ├── inventory_page.h
│   ├── inventory_service.h
│   ├── login_dialog.h
│   ├── main_window.h
│   ├── medicine.h
│   ├── medicine_dialog.h
│   ├── medicine_repository.h
│   ├── medicine_status.h
│   ├── order_service.h
│   ├── orders_page.h
│   ├── sqlite_medicine_repository.h
│   └── text_medicine_repository.h
├── src/
│   ├── checkout_dialog.cpp
│   ├── dashboard_page.cpp
│   ├── date.cpp
│   ├── inventory_page.cpp
│   ├── inventory_service.cpp
│   ├── login_dialog.cpp
│   ├── main.cpp
│   ├── main_window.cpp
│   ├── medicine.cpp
│   ├── medicine_dialog.cpp
│   ├── medicine_status.cpp
│   ├── order_service.cpp
│   ├── orders_page.cpp
│   ├── sqlite_medicine_repository.cpp
│   └── text_medicine_repository.cpp
├── resources/
│   ├── icons/
│   └── styles/
├── resources.qrc
├── CMakeLists.txt
└── pharmacy.db
```

## Database Schema

### `medicines`

```sql
id INTEGER PRIMARY KEY
name TEXT
company TEXT
type TEXT
quantity INTEGER
price REAL
expiry TEXT
```

### `orders`

```sql
id INTEGER PRIMARY KEY AUTOINCREMENT
datetime TEXT
total REAL
```

### `order_items`

```sql
order_id INTEGER
medicine_name TEXT
quantity INTEGER
price REAL
```

### `users`

```sql
id INTEGER PRIMARY KEY AUTOINCREMENT
username TEXT UNIQUE
password TEXT
role TEXT
```

## Default Login Credentials

The application seeds a default admin user automatically if the `users` table is empty.

```text
Username: admin
Password: admin123
Role: admin
```

## Role-Based Access

### Admin

- Full access to the application
- Access to Dashboard
- Can manage inventory
- Can delete medicines
- Can process orders and checkout

### Staff

- Can access Inventory and Orders
- Can view and work with stock/orders
- Cannot delete medicines
- Dashboard access is restricted

## Installation

### Prerequisites

Install the following:

- `Qt 5` or `Qt 6` with:
  - `Qt Widgets`
  - `Qt SQL`
- `CMake` 3.16+
- `MinGW` compiler
- Optional: Qt Creator

### Recommended Environment

- OS: Windows
- Compiler: `MinGW`
- Build system: `CMake`

## Build Instructions

### 1. Clone or download the project

```bash
git clone <your-repo-url>
cd Pharmacy-Management-System-master
```

### 2. Configure the project

```bash
cmake -S . -B build -G "MinGW Makefiles"
```

If required, specify your Qt and compiler paths depending on your local setup.

### 3. Build

```bash
cmake --build build
```

### 4. Run

```bash
./build/pharmacy_gui
```

On Windows, the executable may be:

```bash
build\pharmacy_gui.exe
```

## Notes on Qt + MinGW Setup

If `cmake` cannot detect Qt automatically, configure your environment so CMake can find the Qt installation.

Typical options:

- Use `Qt Creator` with the correct kit
- Add Qt and MinGW paths to `PATH`
- Pass `CMAKE_PREFIX_PATH` manually

Example:

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_xx_xx"
```

## How the Application Works

### Login Flow

1. Application starts
2. `LoginDialog` opens first
3. Credentials are checked against SQLite
4. Password is hashed with `SHA-256`
5. If valid, `MainWindow` opens with role-aware access

### Inventory Flow

- Data is loaded from `SQLiteMedicineRepository`
- `InventoryService` manages in-memory medicine records
- `InventoryPage` provides add/edit/delete/search UI
- Stock and expiry status are calculated in helper logic

### Orders & Checkout Flow

- User adds medicines to cart
- `OrderService` manages cart operations
- Checkout opens `CheckoutDialog`
- Receipt is previewed
- On confirmation:
  - stock is reduced
  - order is saved to SQLite
  - receipt can be exported

## UI Highlights

- Professional dark theme
- Sidebar navigation with icons
- Summary dashboard cards
- Styled tables and buttons
- Role-aware page access
- Consistent dialogs and cards

## Future Improvements

- User management panel for admin
- Create staff users from UI
- Password reset/change password
- Sales reports and analytics charts
- Better charting with `Qt Charts`
- Printable invoice/PDF export
- Barcode support
- Expiry reminder notifications
- Low stock alerts panel
- Search filters and sorting enhancements
- Database backup/restore
- Unit and integration tests
- Packaging and installer support

## Resume / Portfolio Description

**Pharmacy Management System | C++, Qt Widgets, SQLite, CMake**

Developed a desktop pharmacy management application using `C++` and `Qt Widgets`, following a layered architecture with separate UI, service, and repository layers. Implemented secure login with `SHA-256` password hashing and role-based access control, dashboard analytics, inventory management with stock/expiry alerts, order processing, checkout and receipt generation, and SQLite-based persistence. Built and configured the project using `CMake` and `MinGW`.

## Author

**Your Name**

- GitHub: `your-github-profile`
- LinkedIn: `your-linkedin-profile`
- Email: `your-email@example.com`

## License

This project is available for educational and portfolio use. Add a proper open-source license if you plan to publish it publicly.
