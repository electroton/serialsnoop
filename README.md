# serialsnoop

Lightweight serial port monitor with protocol-aware decoding for embedded debugging.

## Installation

```bash
make && sudo make install
```

## Usage

Monitor a serial port with default settings (9600 baud, 8N1):

```bash
serialsnoop /dev/ttyUSB0
```

Specify baud rate and enable protocol decoding:

```bash
serialsnoop -b 115200 -p modbus /dev/ttyUSB0
```

Dump raw hex output to a file:

```bash
serialsnoop -b 9600 --hex -o capture.log /dev/ttyUSB0
```

### Options

| Flag | Description |
|------|-------------|
| `-b` | Baud rate (default: 9600) |
| `-p` | Protocol decoder (`modbus`, `nmea`, `raw`) |
| `--hex` | Display output as hexadecimal |
| `-o` | Write output to file |
| `-t` | Prepend timestamps to each line |

## Building from Source

```bash
git clone https://github.com/yourname/serialsnoop.git
cd serialsnoop
make
```

Requires a C99-compatible compiler and POSIX-compliant system.

## License

MIT © 2024 — see [LICENSE](LICENSE) for details.