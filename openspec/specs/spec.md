# DeviceDiagnostics Plugin Specification

## Overview

Specification for the `DeviceDiagnostics` Thunder (WPEFramework) plugin, which provides device diagnostic capabilities on RDK-based devices. Covers AV decoder status monitoring, device configuration retrieval, and milestone logging — all exposed via JSON-RPC.

---

## Description

The `DeviceDiagnostics` plugin runs within the Thunder framework under the callsign `org.rdk.DeviceDiagnostics`. It is composed of two layers:

- **DeviceDiagnostics** — the JSON-RPC/COM-RPC shell (plugin lifecycle, event forwarding)
- **DeviceDiagnosticsImplementation** — the business logic layer (hardware interaction, HTTP calls, file I/O)

The implementation optionally integrates with:
- **ERM (Essos Resource Manager)** — when `ENABLE_ERM` is defined, a background poll thread monitors AV decoder state changes and fires events.
- **RDK Logger** — when `RDK_LOG_MILESTONE` is defined, `logMilestone` writes to the platform milestone log.

When neither compile-time flag is set, the plugin degrades gracefully (AV status always reports `IDLE`; milestone writes are no-ops returning `success=true`).

### Capability Areas

| Capability | JSON-RPC Method / Event | Notes |
|---|---|---|
| AV Decoder Status Query | `getAVDecoderStatus` | Returns most-active decoder state |
| AV Decoder Status Event | `onAVDecoderStatusChanged` | Fired on state transitions |
| Device Configuration Retrieval | `getConfiguration` | HTTP POST to internal config service |
| Milestone Log Read | `getMilestones` | Reads `/opt/logs/rdk_milestones.log` |
| Milestone Log Write | `logMilestone` | Writes via RDK logger |

---

## Requirements

### General
- REQ-01: The plugin MUST register under callsign `org.rdk.DeviceDiagnostics`.
- REQ-02: The plugin MUST support out-of-process (OOP) execution via Thunder's COM-RPC mechanism.
- REQ-03: The plugin MUST NOT autostart; it requires the `Platform` precondition.
- REQ-04: All JSON-RPC method responses MUST include a `success` boolean field where applicable.

### AV Decoder Status
- REQ-10: `getAVDecoderStatus` MUST return the most active AV decoder status from among all active decoders.
- REQ-11: The `avDecoderStatus` field MUST be one of: `"IDLE"`, `"PAUSED"`, `"ACTIVE"`.
- REQ-12: When `ENABLE_ERM` is not defined, `getAVDecoderStatus` MUST return `"IDLE"`.
- REQ-13: When `ENABLE_ERM` is defined, the implementation MUST poll the ERM library at a minimum interval of 30 seconds for status changes.
- REQ-14: The plugin MUST emit the `onAVDecoderStatusChanged` event when the most-active decoder status transitions between states.
- REQ-15: The `onAVDecoderStatusChanged` event payload MUST include a `avDecoderStatusChange` string field carrying the new status value.

### Device Configuration
- REQ-20: `getConfiguration` MUST accept an array of TR-181-style parameter name strings.
- REQ-21: The implementation MUST send an HTTP POST request to the local configuration service to resolve parameter values.
- REQ-22: The response MUST include a `paramList` array of `{ name, value }` objects.
- REQ-23: On HTTP or parsing failure, `success` MUST be `false` and `paramList` MUST be empty.
- REQ-24: The HTTP request MUST time out after 30 seconds.

### Milestone Logging
- REQ-30: `getMilestones` MUST read milestone entries from `/opt/logs/rdk_milestones.log`.
- REQ-31: `getMilestones` MUST return `success=false` if the milestones log file does not exist or cannot be read.
- REQ-32: `logMilestone` MUST reject an empty `marker` string and return `success=false`.
- REQ-33: When `RDK_LOG_MILESTONE` is defined, `logMilestone` MUST write the marker to the platform RDK milestone log and return `success=true`.
- REQ-34: When `RDK_LOG_MILESTONE` is not defined, `logMilestone` behavior is platform-defined (see Open Queries).

---

## Architecture / Design

```
┌─────────────────────────────────────────────────────────────────┐
│                      Thunder Framework                           │
│                                                                  │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │             DeviceDiagnostics  (JSON-RPC Shell)           │  │
│  │  - Initialize / Deinitialize                              │  │
│  │  - Aggregates IDeviceDiagnostics via COM-RPC              │  │
│  │  - Forwards Notification callbacks → JSON-RPC events      │  │
│  └────────────────────────┬──────────────────────────────────┘  │
│                           │ COM-RPC (in-proc or OOP)             │
│  ┌────────────────────────▼──────────────────────────────────┐  │
│  │         DeviceDiagnosticsImplementation (Business Logic)  │  │
│  │                                                           │  │
│  │   getAVDecoderStatus ──────────► ERM query (or IDLE)      │  │
│  │   getConfiguration   ──────────► HTTP POST :10999 (curl)  │  │
│  │   getMilestones      ──────────► file read (rdk_milestones│  │
│  │   logMilestone       ──────────► RDK logger (or no-op)    │  │
│  │                                                           │  │
│  │   ┌─────────────────────────────────────────────────┐    │  │
│  │   │  AVPollThread  [only when ENABLE_ERM defined]   │    │  │
│  │   │  - polls ERM every 30s via condition_variable   │    │  │
│  │   │  - fires onDecoderStatusChange on transition    │    │  │
│  │   └─────────────────────────────────────────────────┘    │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘

External dependencies:
  ┌──────────────┐   ┌────────────────────────┐   ┌─────────────────────────┐
  │ ERM Library  │   │  Config HTTP Service   │   │  /opt/logs/             │
  │ (essosrmgr)  │   │  localhost:10999       │   │  rdk_milestones.log     │
  └──────────────┘   └────────────────────────┘   └─────────────────────────┘
```

### AV Decoder Status State Machine

```
              ┌──────────────────────────────────┐
              │         ERM Poll Thread          │
              │  (every 30s via wait_for + cv)   │
              └──────────────┬───────────────────┘
                             │ status changed?
               No ◄──────────┴──────────► Yes
                                          │
                          ┌───────────────▼────────────────┐
                          │   onDecoderStatusChange(status) │
                          └───────────────┬────────────────┘
                                          │
                          ┌───────────────▼────────────────┐
                          │  dispatchEvent via WorkerPool  │
                          └───────────────┬────────────────┘
                                          │
                          ┌───────────────▼────────────────┐
                          │ OnAVDecoderStatusChanged →      │
                          │ JDeviceDiagnostics::Event       │
                          └────────────────────────────────┘
```

---

## External Interfaces

### JSON-RPC Interface

**Callsign:** `org.rdk.DeviceDiagnostics`
**Transport:** HTTP on `127.0.0.1:9998/jsonrpc`

#### Method: `getAVDecoderStatus`
- **Parameters:** none
- **Response:**
  ```json
  { "avDecoderStatus": "IDLE" | "PAUSED" | "ACTIVE" }
  ```

#### Method: `getConfiguration`
- **Parameters:**
  ```json
  { "names": ["<TR-181-param-name>", ...] }
  ```
- **Response:**
  ```json
  {
    "paramList": [ { "name": "<string>", "value": "<string>" } ],
    "success": true | false
  }
  ```
- **Internal transport:** HTTP POST to `http://127.0.0.1:10999`
  - Request body: `{"paramList":[{"name":"<param>"},...]}` 
  - Timeout: 30 seconds

#### Method: `getMilestones`
- **Parameters:** none
- **Response:**
  ```json
  { "milestones": ["<string>", ...], "success": true | false }
  ```
- **Source file:** `/opt/logs/rdk_milestones.log`

#### Method: `logMilestone`
- **Parameters:**
  ```json
  { "marker": "<non-empty string>" }
  ```
- **Response:**
  ```json
  { "success": true | false }
  ```

#### Event: `onAVDecoderStatusChanged`
- **Trigger:** Most-active AV decoder status transitions (ERM poll thread only)
- **Payload:**
  ```json
  { "avDecoderStatusChange": "IDLE" | "PAUSED" | "ACTIVE" }
  ```

### COM-RPC Interface

Defined in `IDeviceDiagnostics.h` (`Exchange::IDeviceDiagnostics`):

| Method | Direction | Description |
|---|---|---|
| `GetConfiguration(names, paramList, success)` | in/out | Retrieve config values |
| `GetMilestones(milestones, success)` | out | Read milestone log |
| `LogMilestone(marker, success)` | in/out | Write milestone log |
| `GetAVDecoderStatus(AVDecoderStatus)` | out | Get decoder state |
| `Register(INotification*)` | in | Register event listener |
| `Unregister(INotification*)` | in | Unregister event listener |

### Plugin Configuration

| Field | Value |
|---|---|
| callsign | `org.rdk.DeviceDiagnostics` |
| classname | `DeviceDiagnostics` |
| locator | `libWPEFrameworkDeviceDiagnostics.so` |
| autostart | `false` |
| precondition | `Platform` |
| mode | configurable (in-process / out-of-process) |

---

## Performance

- AV decoder poll interval: 30 seconds (defined by `AVDECODERSTATUS_RETRY_INTERVAL`)
- `getConfiguration` HTTP request timeout: 30 seconds (`curlTimeoutInSeconds`)
- Event dispatch is non-blocking — dispatched via Thunder WorkerPool
- Single-instance design; no per-request heap allocation for the poll thread

---

## Security

- The configuration retrieval endpoint (`localhost:10999`) is accessed without authentication. This is an internal loopback-only HTTP service; no external exposure is assumed.
- The `marker` parameter for `logMilestone` is validated to be non-empty before use.
- No user-supplied data is used in file path construction.

---

## Versioning & Compatibility

- **Plugin API version:** 1.1.2 (as registered in `DeviceDiagnostics.cpp`)
- **Interface version:** 1.0.0 (as declared in `IDeviceDiagnostics.h` JSON annotation)
- The plugin is designed for Thunder R4+ (with compile-time `USE_THUNDER_R4` conditional handling in `Job::Create`)
- Supports both in-process and out-of-process modes via COM-RPC

---

## Conformance Testing & Validation

### Existing Tests

| Test | File | Coverage |
|---|---|---|
| `RegisterMethod` | [Tests/L1Tests/tests/test_DeviceDiagnostics.cpp](../../../Tests/L1Tests/tests/test_DeviceDiagnostics.cpp) | Verifies `getConfiguration` and `getAVDecoderStatus` are registered |
| `getConfiguration` | [Tests/L1Tests/tests/test_DeviceDiagnostics.cpp](../../../Tests/L1Tests/tests/test_DeviceDiagnostics.cpp) | Full socket-based HTTP mock; validates request format and response parsing |
| `getAVDecoderStatus` | [Tests/L1Tests/tests/test_DeviceDiagnostics.cpp](../../../Tests/L1Tests/tests/test_DeviceDiagnostics.cpp) | Validates `IDLE` response without ERM |
| L2 integration test | [Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp](../../../Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp) | End-to-end plugin lifecycle |

### Gaps
- L1 tests: no coverage for `getMilestones` or `logMilestone` (covered at L2 only)
- L1 tests: no coverage for `onAVDecoderStatusChanged` event emission (covered at L2 only)
- No test for `ENABLE_ERM` compile path — all existing tests exercise the non-ERM (always-`IDLE`) path at L1; L2 tests cover `ACTIVE` and `PAUSED` states via COM-RPC mock, but no real ERM integration test exists
- No negative-path test for `getConfiguration` HTTP failure (REQ-23)

---

## Covered Code

- plugin/DeviceDiagnostics.cpp:
    - `DeviceDiagnostics::Initialize`
    - `DeviceDiagnostics::Deinitialize`
    - `DeviceDiagnostics::Notification::OnAVDecoderStatusChanged`
    - `DeviceDiagnostics::Deactivated`
- plugin/DeviceDiagnostics.h:
    - `DeviceDiagnostics`
    - `DeviceDiagnostics::Notification`
- plugin/DeviceDiagnosticsImplementation.cpp:
    - `DeviceDiagnosticsImplementation::GetConfiguration`
    - `DeviceDiagnosticsImplementation::GetMilestones`
    - `DeviceDiagnosticsImplementation::LogMilestone`
    - `DeviceDiagnosticsImplementation::GetAVDecoderStatus`
    - `DeviceDiagnosticsImplementation::Register`
    - `DeviceDiagnosticsImplementation::Unregister`
    - `DeviceDiagnosticsImplementation::getConfig`
    - `DeviceDiagnosticsImplementation::getMostActiveDecoderStatus`
    - `DeviceDiagnosticsImplementation::onDecoderStatusChange`
    - `DeviceDiagnosticsImplementation::dispatchEvent`
    - `DeviceDiagnosticsImplementation::Dispatch`
    - `DeviceDiagnosticsImplementation::AVPollThread` _(ENABLE_ERM only)_
- plugin/DeviceDiagnosticsImplementation.h:
    - `DeviceDiagnosticsImplementation`
    - `DeviceDiagnosticsImplementation::Job`
- IDeviceDiagnostics.h:
    - `Exchange::IDeviceDiagnostics`
    - `Exchange::IDeviceDiagnostics::INotification`
    - `Exchange::IDeviceDiagnostics::ParamList`
    - `Exchange::IDeviceDiagnostics::AvDecoderStatusResult`
- Tests/L1Tests/tests/test_DeviceDiagnostics.cpp:
    - `DeviceDiagnosticsTest`
    - `DeviceDiagnosticsTest::RegisterMethod`
    - `DeviceDiagnosticsTest::getConfiguration`
    - `DeviceDiagnosticsTest::getAVDecoderStatus`
- Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp:
    - `DeviceDiagnostics_L2test`
    - `DeviceDiagnostics_L2test::OnAVDecoderStatusChanged`
    - `DeviceDiagnostics_L2test::WaitForRequestStatus`
    - `DeviceDiagnostics_L2test::onAVDecoderStatusChanged`
    - `LogMilestone_JSONRPC`
    - `IDLE_GetAVDecoderStatus_JSONRPC`
    - `ACTIVE_GetAVDecoderStatus_JSONRPC`
    - `GetMilestones_JSONRPC`
    - `GetConfiguration_JSONRPC`
    - `LogMilestone_COMRPC`
    - `IDLE_GetAVDecoderStatus_COMRPC`
    - `ACTIVE_GetAVDecoderStatus_COMRPC`
    - `PAUSED_GetAVDecoderStatus_COMRPC`
    - `GetConfiguration_COMRPC`
    - `GetMilestones_COMRPC`

---

## Open Queries

- **OQ-01** — `logMilestone` with `RDK_LOG_MILESTONE` undefined currently returns `success=true` without writing anything. Should this return `success=false` or log a warning instead? The current silent success could mislead callers.
- **OQ-02** — `getConfiguration` talks to `http://127.0.0.1:10999`. What is this service? Is it a TR-181 data model provider? What is the complete list of valid parameter names? The spec currently only documents the protocol, not the parameter space.
- **OQ-03** — Plugin version mismatch: code registers as `1.1.2` but the interface annotation declares `1.0.0`. Which is authoritative? Does the `success` field on `getAVDecoderStatus` need aligning with the newer version?
- **OQ-04** — The `AVDECODERSTATUS_RETRY_INTERVAL` (30s) is a compile-time constant. Should it be runtime-configurable via the plugin config? What is the acceptable latency between a decoder state change and the Thunder event being fired?
- **OQ-05** — No L1 tests exist for `getMilestones` or `logMilestone`. Should these be added as part of this spec's acceptance criteria?
- **OQ-06** — Event field naming inconsistency: the `INotification` interface uses `avDecoderStatusChange` (in the event payload) while `getAVDecoderStatus` returns `avDecoderStatus`. Should these align?

---

## References

- [IDeviceDiagnostics.h](../../../IDeviceDiagnostics.h) — COM-RPC interface definition
- [DeviceDiagnostics.md](../../../DeviceDiagnostics.md) — Auto-generated JSON-RPC API documentation
- [ARCHITECTURE.md](../../../ARCHITECTURE.md) — Plugin architecture overview
- [Thunder framework](https://rdkcentral.github.io/Thunder/)
- [entservices-apis IDeviceDiagnostics](https://github.com/rdkcentral/entservices-apis/tree/main/apis/DeviceDiagnostics/IDeviceDiagnostics.h)

---

## Change History

- [2026-04-27] - openspec-explore - Initial spec generated from codebase exploration (IDeviceDiagnostics.h, DeviceDiagnosticsImplementation.cpp, ARCHITECTURE.md, DeviceDiagnostics.md).
- [2026-04-27] - openspec-templater - Restructured to match spec template; expanded Covered Code with L2 test methods; corrected Conformance gaps based on full L2 test scan.
