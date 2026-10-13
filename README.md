# Advantest PA Series (R3263) LabWindows/CVI Instrument Driver

LabWindows/CVI (VXIplug&play) instrument driver for the Advantest PA series spectrum analyzers.

| | |
|---|---|
| **Driver type** | LabWindows/CVI (VXIplug&play) |
| **Source folder** | [`avpa/`](avpa/) |
| **Function prefix** | `AVPA_` |
| **Manufacturer** | Advantest |
| **Instrument type** | Spectrum analyzer |
| **Driver version** | Rev 2.0 (05/1999) |
| **Certification** | NI Certified |

## Supported Instruments

- Advantest R3263
- Advantest R3272
- Advantest R3463
- Advantest R3465

## Supported Interfaces

- GPIB
- Serial (RS-232). All functions except trace data readback are supported over RS-232.

## Features

- Frequency, span, frequency offset, step size, and frequency counter
- Reference level, units, scale, reference offset, and attenuation
- RBW/VBW and auto-coupling, sweep time, sweep mode, trigger mode, and gated sweep
- Trace A/B modes, detector, trace points, trace math, and averaging
- Limit lines, display line, measurement window, and level correction
- Normal, delta, and multi-markers, peak search, and marker-to functions
- Trace data readback, calibration, screen copy, and memory card file operations

## Requirements

- LabWindows/CVI 5.0.1 or later (the project file is saved in LabWindows/CVI 2020)
- [NI-VISA](https://www.ni.com/en/support/downloads/drivers/download.ni-visa.html)
- Windows

## Repository Layout

```
avpa/
├── avpa.c / avpa.h     Driver source and header
├── avpa.fp             Function panels
├── avpa.prj            LabWindows/CVI project (builds a DLL)
├── avpa.doc            Function reference
└── avpau.c / .h / .uir Example user-interface application
```

## Getting Started

1. Install LabWindows/CVI and NI-VISA.
2. Clone this repository, or download it as a ZIP.
3. Open [`avpa/avpa.prj`](avpa/avpa.prj) in LabWindows/CVI and build it, or add
   `avpa.c`, `avpa.h`, and `avpa.fp` to your own project.
4. Find the analyzer's VISA resource name in NI MAX (e.g. `GPIB0::8::INSTR` or
   `ASRL1::INSTR`).
5. Call `AVPA_init` with that resource name, configure the analyzer with the
   `AVPA_Set_*` functions, read results with the `AVPA_Read_*` functions, and finish with
   `AVPA_close`. The `avpau` application shows a complete front-panel example.

## Known Issues

- `AVPA_Read_TraceData` is not supported over RS-232; use GPIB to read traces.

## Support

Report driver bugs and feature requests through [GitHub Issues](https://github.com/ASingh-NI/instr-driver-advantest-pa-r3263-cvi/issues).

## Contributing

Contributions are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md).

## Security

See [SECURITY.md](SECURITY.md). Do not report security vulnerabilities through public GitHub issues.

## License

See [LICENSE.md](LICENSE.md).
