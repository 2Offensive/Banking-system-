# Banking Management System (C++)

A console-based banking application built for Project Phase 1 of my internship at Sqrock IT Solutions. The goal was to simulate the core operations of a real bank — opening an account, moving money around safely, and keeping a proper transaction trail — using nothing but core C++.

## What it does

- Anyone can open a new account by providing their name, account type (Savings/Current), a PIN, and an initial deposit. The system auto-generates an account number.
- Login is done using account number + PIN, and blocked accounts can't log in at all.
- Once logged in, you can deposit, withdraw, or transfer money to another account. The app validates every transaction so you can never withdraw more than your balance, transfer to a non-existent account, or end up with a negative balance.
- Every deposit, withdrawal, and transfer gets logged with a timestamp, so you can check your full transaction history whenever you want.
- There's also a separate Admin Panel (password protected) where an "admin" can view every account in the system, monitor all transactions across all users, block/unblock accounts, or delete an account entirely.

## How it's built

The system is split into an `Account` class and a `Transaction` class, both of which know how to serialize themselves to and from a single line of text. Account data lives in `accounts.txt` and every transaction gets appended to `transactions.txt`, so nothing is ever lost between runs — you can close the terminal and come back later and everything's exactly how you left it.

## Technologies used

- Core C++
- OOP (Account & Transaction classes)
- STL (`vector`, `string`, `sstream`, `algorithm`)
- File Handling (`ifstream` / `ofstream`, append mode for transaction logs)
- Basic role-based access (customer vs admin)

## How to run it

```bash
g++ -std=c++17 -o banking_system banking_system.cpp
./banking_system
```

On Windows (VS Code terminal):

```powershell
g++ -std=c++17 -o banking_system banking_system.cpp
.\banking_system.exe
```

> Admin panel password: `admin123` (hardcoded for demo purposes — obviously a real system would never do this!)

## Things I'd add if I had more time

- Actual PIN hashing instead of storing it as plain text
- Interest calculation for savings accounts
- A basic loan management simulation
- ATM-style withdrawal limits per day

## What I learned

This was a good exercise in thinking about validation — a banking app can't just "trust" the user's input, so a lot of the logic here is really about catching edge cases (insufficient balance, invalid account numbers, self-transfers, etc.) before anything touches the actual balance. I also got more comfortable separating "customer" logic from "admin" logic within the same program instead of mixing them together.

---
Built as part of Project Phase 1, Sqrock IT Solutions Internship.

<img width="448" height="632" alt="0" src="https://github.com/user-attachments/assets/af7f1d35-97cf-4906-ba3b-9554fad38847" />
<img width="453" height="871" alt="1" src="https://github.com/user-attachments/assets/784db62b-828f-426c-b87c-d585b987bc66" />
<img width="466" height="860" alt="2" src="https://github.com/user-attachments/assets/2fdf23eb-3dd7-4b54-9cd0-653daceab7f8" />
<img width="436" height="607" alt="3" src="https://github.com/user-attachments/assets/5f2626c3-19aa-4e16-910d-80f57ad80177" />
<img width="425" height="764" alt="4" src="https://github.com/user-attachments/assets/5fa11bd9-8357-41d9-bf7f-ccc24639d0e2" />
<img width="556" height="628" alt="5" src="https://github.com/user-attachments/assets/78e703a3-3138-432c-b64c-ceb2fe783764" />
<img width="450" height="343" alt="6" src="https://github.com/user-attachments/assets/9244da5c-7a81-4bf9-a810-43a807e3f507" />
