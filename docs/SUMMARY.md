# Summary

[Introduction](./intro.md)
# Documentation
- [Getting Starting](./doc/getting_started.md)
- [Porting](./doc/porting.md)
    - [Critical Sections](./doc/porting_critical_sections.md)
- [Configuration](./doc/config.md)
- [Instrumenting Your Code](./doc/instrumentation.md)
    - [Event Markers](./doc/evtmarkers.md)
    - [Value Markers](./doc/valmarkers.md)
    - [Interrupts](./doc/interrupts.md)
- [FreeRTOS Tracing](./doc/freertos.md)
- [Trace Handling](./doc/handling.md)
    - [The Metadata Buffer](./doc/metadata_buf.md)
    - [Streaming Backend](./doc/streaming.md)
    - [Snapshot Backend](./doc/snapshot.md)
    - [Post-Mortem Backend](./doc/postmortem.md)
    - [Dropped Events](./doc/dropped_evts.md)
- [Multi-core Support](./doc/multicore_support.md)
- [Viewing your traces](./doc/viewing.md)
    - [CLI Conversion Tool](./doc/tband_cli.md)
    - [Web Conversion Tool](./doc/web.md)
# Technical Details
- [Trace Format](./tech_details/trace_format.md)
    - [COBS Framing](./tech_details/cobs.md)
    - [Trace Event Fields](./tech_details/bin_event_fields.md)
    - [Trace Event Index](./tech_details/bin_events.md)
    - [Code Generation](./tech_details/codegen.md)
- [Technologies Overview](./tech_details/techstack.md)
    - [Rust](./tech_details/rust.md)
    - [WASM](./tech_details/wasm.md)
    - [Website](./tech_details/web.md)
    - [Perfetto & Synthetto](./tech_details/synthetto.md)
