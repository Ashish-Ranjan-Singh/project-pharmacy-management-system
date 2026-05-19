# Pharmacy Management System

## Complete Explanation Document for Lab Viva and Presentation

## 1. Project Overview

### Short Explanation

The **Pharmacy Management System** is a desktop application built using **C++**, **Qt Widgets**, and **SQLite** to manage pharmacy operations digitally. It supports secure login, role-based access, medicine inventory management, stock and expiry monitoring, order processing, checkout, receipt generation, and persistent storage using a local database.

### Long Explanation

This project is designed to replace manual pharmacy record handling with a structured software solution. In a real pharmacy, staff need to maintain medicine details, stock quantity, expiry dates, prices, customer orders, and sales records. Doing all this manually is slow and error-prone.

To solve this problem, this project uses:

- **C++ with OOP** for core business logic
- **Qt Widgets** for a desktop graphical user interface
- **SQLite** for lightweight local database storage
- **Layered architecture** for clean separation of UI, business logic, and data access

The application starts with a login screen. After authentication, the user enters the main window. Depending on the role, the user can access the dashboard, inventory, and orders pages. The inventory page allows medicine management, the dashboard shows analytics, and the orders page supports cart-based checkout with receipt generation. All data is saved in `pharmacy.db`.

### Real-World Use Case

In a pharmacy shop or small medical store, this system can be used to:

- store all medicine records in one place
- track available stock
- identify low-stock and expired medicines
- manage billing and customer purchases
- save order history
- control access for admin and staff users

This makes pharmacy operations faster, more accurate, and more secure.

---

## 2. System Flow

### Overall Step-by-Step Flow

1. **Application starts**
   The program begins from `main.cpp`.

2. **Qt application is initialized**
   The app style, font, and stylesheet are loaded.

3. **Login dialog opens**
   `LoginDialog` asks the user for username and password.

4. **Authentication happens**
   Entered password is hashed using SHA-256 and checked against the `users` table in SQLite.

5. **Role is identified**
   If login is successful, the system stores the user role such as `admin` or `staff`.

6. **Main window opens**
   `MainWindow` creates the main interface and loads the inventory from the database.

7. **Dashboard is shown for admin**
   Admin can view summary cards like total medicines, low stock count, total orders, and total revenue.

8. **Inventory page is used**
   User can add, edit, delete, and search medicines.

9. **Medicine status is calculated**
   The system marks medicines as:
   - In Stock
   - Low Stock
   - Expiring Soon
   - Expired

10. **Orders page is used**
    User selects medicines, enters quantity, and adds items to cart.

11. **Checkout is performed**
    A receipt preview dialog opens before confirming the order.

12. **Database is updated**
    On checkout:
    - medicine stock is reduced
    - order is stored in `orders`
    - order items are stored in `order_items`

13. **Receipt can be exported**
    Receipt can be saved to a text file.

### Simple User Flow

`Login -> Main Window -> Dashboard/Inventory/Orders -> Cart -> Checkout -> Database Save -> Receipt`

---

## 3. Architecture

### Layered Architecture Explanation

This project follows a **layered architecture**, which means each part of the system has a separate responsibility.

### Layers Used

1. **UI Layer**
   Handles screens, dialogs, tables, buttons, and user interaction.

2. **Service Layer**
   Handles business logic like medicine management, cart operations, stock updates, and sorting.

3. **Repository Layer**
   Acts as the bridge between business logic and data storage.

4. **Database Layer**
   Stores data permanently in SQLite.

### Why This Architecture Is Good

- easy to understand
- easier to maintain
- supports future extension
- separates logic from UI
- makes code more modular and reusable

### Simple Text Diagram

```text
+-----------------------------+
|          UI Layer           |
|-----------------------------|
| LoginDialog                 |
| MainWindow                  |
| DashboardPage               |
| InventoryPage               |
| OrdersPage                  |
| MedicineDialog              |
| CheckoutDialog              |
+-------------+---------------+
              |
              v
+-----------------------------+
|       Service Layer         |
|-----------------------------|
| InventoryService            |
| OrderService                |
+-------------+---------------+
              |
              v
+-----------------------------+
|      Repository Layer       |
|-----------------------------|
| MedicineRepository          |
| SQLiteMedicineRepository    |
| TextMedicineRepository      |
+-------------+---------------+
              |
              v
+-----------------------------+
|       Database Layer        |
|-----------------------------|
| SQLite                      |
| pharmacy.db                 |
+-----------------------------+
```

---

## 4. File-by-File Explanation

## Core Domain Files

### `include/medicine.h` and `src/medicine.cpp`

These files define the **Medicine class**, which is the main data model of the project.

#### Purpose

- stores medicine details
- represents one medicine record
- converts medicine type to string and back

#### Main Data Stored

- company name
- medicine name
- quantity
- id
- price
- type
- expiry date

#### Important Points

- Uses getters and setters for safe access
- Supports multiple medicine types through `MedicineType`
- Uses helper functions like `typeToString()` and `tryParseType()`

This class is the backbone of inventory and order management.

### `include/date.h` and `src/date.cpp`

These files define a custom **Date** class.

#### Purpose

- stores day, month, and year
- validates dates
- converts date to display string

#### Why It Is Used

Instead of depending only on Qt date types in the full business layer, the project keeps a simple C++ date object for the medicine model.

---

## Service Layer Files

### `include/inventory_service.h` and `src/inventory_service.cpp`

These files implement the **InventoryService** class.

#### Purpose

- manages medicines in memory
- adds, updates, removes, and searches medicines
- sorts medicines by name

#### Key Functions

- `addMedicine()`
- `removeMedicineById()`
- `updateMedicine()`
- `findById()`
- `findByName()`
- `sortedByName()`

#### Role in Project

The inventory page does not directly manipulate vectors or database logic. Instead, it uses `InventoryService`, which keeps business logic cleaner.

### `include/order_service.h` and `src/order_service.cpp`

These files implement the **OrderService** and `OrderItem`.

#### Purpose

- manages cart items
- adds and removes items from cart
- calculates total cost
- performs checkout by reducing stock

#### Key Functions

- `addItem()`
- `removeItemByMedicineId()`
- `total()`
- `checkout()`

#### Important Logic

- does not allow invalid quantity
- does not allow ordering more than available stock
- decreases inventory quantity only after validation

This class is responsible for the order workflow.

### `include/medicine_status.h` and `src/medicine_status.cpp`

These files calculate medicine health/status.

#### Status Values

- `InStock`
- `LowStock`
- `ExpiringSoon`
- `Expired`

#### Rules Used

- if expiry date is before today -> `Expired`
- if expiry date is within 30 days -> `Expiring Soon`
- if quantity is less than 10 -> `Low Stock`
- otherwise -> `In Stock`

This logic is reused in inventory and orders tables.

---

## Repository Layer Files

### `include/medicine_repository.h`

This is the **abstract base repository interface**.

#### Purpose

- defines common functions for loading and saving data
- hides database details from the UI and service layers

#### Why It Matters

This is a strong example of abstraction and polymorphism. The application can work with different repository implementations as long as they follow the same interface.

### `include/sqlite_medicine_repository.h` and `src/sqlite_medicine_repository.cpp`

These files implement the concrete SQLite repository.

#### Purpose

- connects the application to SQLite
- loads medicines from `pharmacy.db`
- saves inventory, orders, and checkout data
- handles schema creation
- handles login authentication
- hashes passwords
- seeds default records

#### Important Responsibilities

- create tables if they do not exist
- authenticate users from database
- seed default admin account
- seed sample medicines if medicine table is empty
- store order and order item records
- calculate total orders and revenue

This is the most important persistence file in the project.

### `include/text_medicine_repository.h` and `src/text_medicine_repository.cpp`

These files implement a text-based repository.

#### Purpose

- legacy or alternate file-based storage
- reads and writes medicines from text files

#### Why It Is Useful

It shows that the system design supports changing persistence methods without changing the whole application.

---

## Entry Point and Main Window

### `src/main.cpp`

This is the entry point of the application.

#### What It Does

- creates `QApplication`
- applies style and font
- loads QSS styling
- creates `SQLiteMedicineRepository`
- opens `LoginDialog`
- opens `MainWindow` after successful login

This file controls application startup.

### `include/main_window.h` and `src/main_window.cpp`

These files define the main application window.

#### Purpose

- creates page navigation
- loads inventory data
- switches between pages
- controls role-based page visibility

#### Important Behavior

- admin sees Dashboard, Inventory, and Orders
- staff sees Inventory and Orders
- inventory is loaded once at startup from repository

This file acts as the UI shell of the system.

---

## UI Pages and Dialogs

### `include/login_dialog.h` and `src/login_dialog.cpp`

Provides the login screen.

#### Purpose

- collect username and password
- call repository authentication
- return authenticated role

#### Important Points

- checks empty fields
- uses message boxes for errors
- password field is masked

### `include/dashboard_page.h` and `src/dashboard_page.cpp`

Shows summary analytics.

#### Displays

- total medicines
- low stock items
- total orders
- total revenue

#### Data Source

- inventory count comes from `InventoryService`
- orders and revenue come from repository/database

### `include/inventory_page.h` and `src/inventory_page.cpp`

This is the medicine management page.

#### Purpose

- display all medicines
- search medicines
- add new medicines
- edit medicine details
- delete medicines
- show status and type

#### Important Behavior

- search works on medicine name, company, ID, type, and status text
- delete button is disabled for staff users
- updates are saved through repository

### `include/medicine_dialog.h` and `src/medicine_dialog.cpp`

This dialog is used when adding or editing a medicine.

#### Purpose

- collect medicine details from user
- return a `Medicine` object

#### Important Fields

- company
- medicine name
- ID
- quantity
- price
- type
- expiry date

#### Medicine Types

The dialog supports multiple medicine types such as:

- Tablet
- Capsule
- Liquid
- Syrup
- Injection

### `include/orders_page.h` and `src/orders_page.cpp`

This page manages medicine sales and cart operations.

#### Purpose

- show available medicines
- search medicines
- add medicines to cart
- remove items from cart
- show total price
- start checkout

#### Important Behavior

- out-of-stock items are not shown for sale
- quantity is validated before adding to cart
- receipt preview is shown before final confirmation

### `include/checkout_dialog.h` and `src/checkout_dialog.cpp`

This dialog shows receipt preview before final checkout.

#### Purpose

- display purchase summary
- confirm checkout
- export receipt to text file

#### Output Includes

- medicine names
- quantity
- total item price
- total bill amount
- date and time

---

## 5. OOP Concepts Used

This project is a strong example of **Object-Oriented Programming**.

### 1. Encapsulation

Encapsulation means binding data and methods inside classes and controlling access through functions.

#### Example

`Medicine` keeps data members like `name_`, `price_`, and `quantity_` private, and provides public getters/setters.

#### Benefit

- protects data
- avoids direct modification from outside classes
- improves reliability

### 2. Abstraction

Abstraction means hiding internal complexity and showing only required functionality.

#### Example

`MedicineRepository` provides abstract functions like `load()` and `save()`. UI classes use these functions without needing to know SQL details.

#### Benefit

- simpler code usage
- separates interface from implementation

### 3. Inheritance

Inheritance means one class derives from another.

#### Example

`SQLiteMedicineRepository` inherits from `MedicineRepository`.

#### Benefit

- code reuse
- common structure for repositories

### 4. Polymorphism

Polymorphism means one interface can support multiple implementations.

#### Example

The project can use `MedicineRepository` as a base type and work with `SQLiteMedicineRepository` or `TextMedicineRepository`.

#### Benefit

- flexible design
- easier future extension

### 5. Modularity

Modularity means dividing the system into independent parts.

#### Example

- medicine logic in `Medicine`
- inventory logic in `InventoryService`
- order logic in `OrderService`
- database logic in repository
- screen logic in UI pages

#### Benefit

- easier debugging
- easier maintenance
- easier presentation and understanding

### 6. Separation of Concerns

This is also visible throughout the project.

#### Example

- UI handles display and user input
- services handle business logic
- repository handles database access

This makes the system clean and professional.

---

## 6. Database Explanation

The project uses **SQLite** as a local database. The database file is `pharmacy.db`.

### Why SQLite Is Used

- lightweight
- serverless
- easy to integrate
- good for desktop applications
- no external database setup required

### Tables and Their Roles

### `medicines`

Stores all medicine records.

#### Main Columns

- `id`
- `name`
- `company`
- `type`
- `quantity`
- `price`
- `expiry`

#### Purpose

Used by inventory and orders modules.

### `orders`

Stores each completed order.

#### Main Columns

- `id`
- `datetime`
- `total`

#### Purpose

Keeps summary of each sale transaction.

### `order_items`

Stores individual items for each order.

#### Main Columns

- `order_id`
- `medicine_name`
- `quantity`
- `price`

#### Purpose

Stores details of medicines sold in a particular order.

### `users`

Stores login credentials and roles.

#### Main Columns

- `id`
- `username`
- `password`
- `role`

#### Purpose

Supports authentication and role-based access control.

### Auto Seeding

When database tables are empty, the application automatically inserts:

- default admin user
- sample medicine data

This helps in testing and demonstration.

---

## 7. Key Features

### 1. Secure Login System

- username/password authentication
- SHA-256 hashed password storage
- error handling for invalid login

### 2. Role-Based Access

- admin has full access
- staff has restricted access
- dashboard access is admin-only
- delete operation is admin-only

### 3. Inventory Management

- add medicine
- edit medicine
- delete medicine
- search medicine
- save all changes to database

### 4. Multiple Medicine Types

Supports multiple types through a centralized type model, including:

- Tablet
- Capsule
- Liquid
- Syrup
- Injection

### 5. Status Detection

Automatically detects:

- low stock
- expiry soon
- already expired

### 6. Orders and Cart

- add medicines to cart
- remove items from cart
- calculate total amount
- prevent over-ordering

### 7. Checkout and Receipt

- preview order before final confirmation
- export receipt as text file
- save order history in database

### 8. Dashboard Analytics

- total medicines
- low stock count
- total orders
- total revenue

### 9. Persistent Storage

- inventory is not lost after closing the app
- orders remain stored in SQLite
- users remain stored in SQLite

---

## 8. Security

### Password Hashing

The system does not compare plain text passwords directly from the database.

#### How It Works

1. User enters password
2. Password is hashed using **SHA-256**
3. Hashed value is compared with stored hash in database

#### Why It Is Secure

- actual password is not stored directly
- reduces risk if database is exposed
- follows a better security practice than plain text storage

### Role-Based Security

The application distinguishes between user roles.

#### Admin

- can view dashboard
- can manage inventory fully
- can delete medicines
- can process orders

#### Staff

- can use inventory and orders
- cannot delete medicines
- dashboard is hidden

This ensures not every user gets full control.

### Input Validation

The project also validates:

- empty login fields
- duplicate medicine IDs
- invalid quantity in cart
- stock limits during checkout

---

## 9. Viva Questions and Answers

### 1. What is the main objective of this project?

The objective is to digitally manage pharmacy operations such as medicine inventory, stock monitoring, order processing, login security, and database storage using C++ and Qt.

### 2. Why did you use Qt Widgets?

Qt Widgets is suitable for desktop GUI applications. It provides ready-made components like dialogs, tables, buttons, labels, and layouts, which help build a professional interface quickly.

### 3. Why did you use SQLite?

SQLite is lightweight, serverless, easy to use, and ideal for desktop applications. It stores data in a single local file and does not need separate server configuration.

### 4. What architecture does your project use?

The project uses layered architecture: UI layer, service layer, repository layer, and database layer.

### 5. What is the role of `InventoryService`?

`InventoryService` handles in-memory medicine operations such as adding, updating, deleting, searching, and sorting medicines.

### 6. What is the role of `OrderService`?

`OrderService` manages the cart, calculates bill totals, validates quantity, and performs checkout by reducing stock.

### 7. How is OOP used in this project?

OOP is used through classes like `Medicine`, `InventoryService`, `OrderService`, and repository classes. Concepts used include encapsulation, abstraction, inheritance, and polymorphism.

### 8. How is login made secure?

The system hashes passwords using SHA-256 before comparing them with the stored password hash in the database.

### 9. How does the project detect medicine status?

It checks expiry date and quantity. Expired medicines are marked expired, medicines expiring within 30 days are marked expiring soon, and medicines with quantity less than 10 are marked low stock.

### 10. How is role-based access implemented?

After successful login, the authenticated role is passed to `MainWindow`. Based on the role, certain pages and actions are enabled or hidden.

### 11. What happens during checkout?

The system validates cart items, reduces stock from inventory, saves updated medicines to the database, inserts order details into `orders` and `order_items`, and then clears the cart.

### 12. Why did you create an abstract repository class?

It separates database logic from business logic and allows multiple implementations such as SQLite repository or text file repository.

---

## 10. Presentation Script

## 2-Minute Explanation

Good morning. My project is **Pharmacy Management System**, developed using **C++**, **Qt Widgets**, and **SQLite**. The purpose of this project is to computerize pharmacy operations such as medicine inventory management, secure login, stock monitoring, order handling, and billing.

The system starts with a login screen. User authentication is handled using SQLite, and passwords are protected using **SHA-256 hashing**. After login, users are given access based on their role. Admin users can view the dashboard, manage medicines, delete records, and process orders, while staff users have limited access.

The project uses a **layered architecture**, where the UI layer handles user interaction, the service layer handles business logic, the repository layer handles database communication, and SQLite stores the data permanently.

Main features include adding, editing, deleting, and searching medicines, identifying low stock and expired medicines, managing a cart, performing checkout, and generating a receipt. The dashboard also shows total medicines, low stock count, total revenue, and order count.

Overall, this project demonstrates OOP concepts, GUI development, database integration, and secure application design in a real-world pharmacy use case.

## 5-Minute Detailed Explanation

Good morning. My project is **Pharmacy Management System**, a desktop application built using **C++**, **Qt Widgets**, and **SQLite**. The goal of this project is to make pharmacy management faster, more accurate, and more secure by replacing manual record keeping with software.

This system has four main layers. First is the **UI layer**, which includes pages like login, dashboard, inventory, and orders. Second is the **service layer**, which contains business logic classes such as `InventoryService` and `OrderService`. Third is the **repository layer**, where I use `MedicineRepository` as an abstract interface and `SQLiteMedicineRepository` as the implementation. Finally, the **database layer** stores all records in `pharmacy.db`.

When the application starts, it loads the Qt interface and opens the login dialog. The user enters username and password. The password is hashed using **SHA-256** and compared with the stored value in the database. If the login is successful, the system reads the user role and opens the main window.

The application supports **role-based access control**. If the user is an admin, they can access the dashboard, inventory, and orders. If the user is staff, dashboard access is restricted and delete operations are disabled.

In the **inventory module**, the user can add, edit, delete, and search medicines. Each medicine has an ID, name, company, type, quantity, price, and expiry date. The system also supports multiple medicine types like tablet, capsule, liquid, syrup, and injection. Medicine status is calculated automatically. If quantity is less than 10, it becomes low stock. If expiry is within 30 days, it is shown as expiring soon. If expiry date has passed, it is marked expired.

In the **orders module**, the user can browse available medicines and add them to a cart. The `OrderService` validates quantity and ensures the user cannot order more than available stock. It also calculates the total bill amount. During checkout, a receipt preview dialog appears. If the user confirms, stock is reduced, the order is saved in the `orders` table, and item details are saved in the `order_items` table.

The project uses SQLite for persistence. Important tables are `medicines`, `orders`, `order_items`, and `users`. The system also performs **auto seeding**, meaning that if the database is empty, it automatically inserts a default admin user and sample medicine data.

From an OOP perspective, this project uses encapsulation through classes like `Medicine`, abstraction through `MedicineRepository`, inheritance through `SQLiteMedicineRepository`, polymorphism through repository interfaces, and modularity through separate files for each responsibility.

In conclusion, this project demonstrates GUI programming, database connectivity, OOP principles, secure authentication, and real-world application design in a pharmacy domain.

---

## 11. Conclusion

### Final Conclusion

The Pharmacy Management System is a practical desktop application that combines **C++ OOP**, **Qt GUI programming**, and **SQLite database management** into one complete software solution. It solves real pharmacy problems like inventory tracking, medicine expiry monitoring, user authentication, and order processing in a clear and modular way.

This project is not only useful as a software product, but also a strong academic demonstration of:

- object-oriented design
- layered architecture
- desktop interface development
- database integration
- security through password hashing

### Future Improvements

- add user management page for creating staff accounts
- add password change/reset support
- generate PDF invoices
- add barcode scanning
- add sales reports and charts
- add backup and restore
- add medicine supplier management
- add notifications for expiry and low stock
- add audit logs
- add unit testing

### Scalability

This project is suitable for small to medium pharmacy stores. Because of its layered architecture, it can be extended in the future to:

- use a larger database like MySQL or PostgreSQL
- support multiple branches
- move to client-server architecture
- add online billing and reporting
- integrate with hospital or clinic systems

So the current design is simple for academic use, but also structured enough for future growth.

---

## 12. Quick Revision Points

If you want to memorize quickly, remember these points:

- built with **C++ + Qt Widgets + SQLite**
- follows **UI -> Service -> Repository -> Database**
- starts with **secure login**
- uses **SHA-256 password hashing**
- supports **admin and staff roles**
- manages **inventory, stock, expiry, and orders**
- stores data in **pharmacy.db**
- uses **OOP concepts** like encapsulation, abstraction, inheritance, and polymorphism
- supports **receipt generation and export**
- includes **auto seeding** for demo data

---

## 13. One-Line Summary for Viva

This project is a layered C++ Qt desktop application that securely manages pharmacy inventory, medicine sales, and user access using SQLite persistence and object-oriented design.
