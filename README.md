# 🔧 Portable Stress Test (CPU + Disk) in C

This is a simple **cross-platform** (Linux + macOS) command-line stress test tool written in pure C. It loads the CPU with prime number calculations and stresses the disk by writing to a temporary file.

## 📦 Features

- ✅ CPU stress via prime number calculations
- ✅ Disk stress via file write operations
- ✅ Multithreading using POSIX threads (`pthread`)
- ✅ Clean shutdown on `Ctrl+C` or after specified duration
- ✅ Portable: works on both **Linux** and **macOS**
- 🧱 Simple and dependency-free — only uses the standard C library

---

## 🛠️ Compilation

Use `gcc` (or `clang`) with `-pthread`:

```bash
gcc stress_test_portable.c -o stress_test -pthread
```

---

## 🚀 Usage

```bash
./stress_test [-t threads] [-d duration]
```

### Options:

| Option      | Description                          | Default |
|-------------|--------------------------------------|---------|
| `-t`        | Number of CPU threads to spawn       | `1`     |
| `-d`        | Duration of the test in seconds      | `10`    |

### Example:

```bash
./stress_test -t 4 -d 5
```

Runs a 5-second stress test using 4 CPU threads.

---

## 📊 Output Example

```
=== Stress Test Report ===
Test Duration: 5.00 seconds
CPU Stress: 12848929 iterations
Disk Stress: 348160 bytes written
===========================
```

---

## 🧼 Cleanup

Temporary files used for disk stress are **automatically deleted** after the test ends.

---

## 📎 Notes

- On **macOS**, `/proc/stat` does not exist — this version does not attempt to read system-wide CPU usage.
- To stop the test early, press `Ctrl+C`.

---

## 🧩 Possible Extensions

- Add **memory stress** (malloc/free loop)
- Measure **actual CPU usage** (via `sysctl` on macOS)
- Support output to a **log file**
- JSON-formatted reports

---

## 📄 License

MIT License. Use freely and modify for your needs.
