A Real-Time Embedded Emergency Management System

## Overview

This project implements a shopping solution that consists of a server‐side console application and a client‐side Windows Forms application. The server handles product information, user accounts, and orders, while the client provides a user-friendly interface for product selection and purchases.

## Features

### Server-Side

- **Console Application**  
  Outputs server responses to the console before communicating with clients.

- **Pre-defined Products**  
  Initializes five products with random quantities (1–3) each time the server starts.

- **User Accounts**  
  Supports at least three pre-defined user accounts, each with an account number and username.

- **In-Memory Storage**  
  Stores ordering information in variables (e.g., dictionaries or lists); data is disposed of when the server shuts down.

- **Multi-client Support**  
  Can handle multiple client connections simultaneously.

- **Protocol Handling**  
  Processes client commands and sends appropriate responses.

### Client-Side

- **Windows Forms Application**  
  Provides a graphical UI for users to select products and make purchases.

- **Login Form**  
  Prompts for hostname/IP and account number (defaults to `localhost`).

- **Error Handling**  
  Displays errors if the server can’t be found or if login fails.

- **Product Information**  
  Retrieves and displays product names and quantities upon successful connection.

- **Graceful Disconnection**  
  Allows users to disconnect from the server and closes the application appropriately.

- **Purchase Management**  
  Displays current purchase orders and handles purchase attempts, including error messaging for unavailable products.

- **Asynchronous Communication**  
  Ensures that server interactions do not block the GUI thread (using multithreading or async).

## Protocol Standard

- **DISCONNECT**  
  - Server Response: No response.

- **CONNECT:account_no**  
  - Server Response:  
    - `CONNECTED:user_name`  
    - `CONNECT_ERROR`

- **GET_PRODUCTS**  
  - Server Response:  
    - `PRODUCTS: product_name1,quantity1|...`  
    - `NOT_CONNECTED`

- **GET_ORDERS**  
  - Server Response:  
    - `ORDERS: product_name1,quantity1,user_name|...`  
    - `NOT_CONNECTED`

- **PURCHASE:product_name**  
  - Server Response:  
    - `NOT_AVAILABLE`  
    - `NOT_VALID`  
    - `NOT_CONNECTED`

## Setup Instructions

### Server-Side Setup

1. Clone the repository:  
   ```bash
   git clone https://github.com/yourusername/shopping-solution.git
   cd shopping-solution/Server
   ```
2. Restore and build (assuming .NET Core):  
   ```bash
   dotnet restore
   dotnet build
   ```
3. Run the server:  
   ```bash
   dotnet run
   ```

### Client-Side Setup

1. Open a new terminal and navigate to the client folder:  
   ```bash
   cd ../Client
   ```
2. Build and run the Windows Forms app:  
   ```bash
   dotnet run
   ```
