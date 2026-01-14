# 🚀 C++ TCP Chat Server (WinSock)

## 📌 Description
This project is a simple TCP chat server written in **C++** using **WinSock2** on Windows.  
The server supports multiple clients using non-blocking sockets and `select()` for I/O multiplexing.

Each message received from a client is broadcast to all other connected clients.

This project demonstrates core networking concepts and is suitable as a **junior C++ portfolio project**.

---

## 🛠 Technologies
- C++ (C++11+)
- WinSock2
- TCP/IP
- Non-blocking sockets
- `select()`
- STL (`vector`)
- Windows API

---

## ⚙️ Features
- TCP server
- Multiple client support
- Non-blocking sockets (`ioctlsocket`)
- I/O multiplexing with `select`
- Message broadcasting
- Graceful client disconnect handling

---

## 📂 Project Structure
