# DeviceDiagnostics Plugin Specification

## Overview
The DeviceDiagnostics plugin provides an interface for device diagnostics within the Thunder framework. It enables clients to query AV decoder status, retrieve configuration properties, list milestones, log milestone markers, and retrieve previous reboot information. The plugin integrates with system APIs and hardware drivers as needed.

## Description
The DeviceDiagnostics plugin exposes device health and diagnostic information to clients via JSON-RPC, operating within the WPEFramework/Thunder plugin architecture. It implements the `Exchange::IDeviceDiagnostics` interface through an out-of-process `DeviceDiagnosticsImplementation` and bridges notifications via an inner `Notification` sink. Key capabilities include:
- Querying the most active AV decoder/pipeline status (optionally using the Essos Resource Manager).
- Retrieving named configuration properties from the device.
- Listing and logging RDK milestone markers.
- Retrieving previous reboot information.
- Emitting `onAVDecoderStatusChanged` events to connected clients when decoder state changes.

## Requirements
- The plugin must expose `getAVDecoderStatus`, `getConfiguration`, `getMilestones`, `logMilestone`, and `getPreviousRebootInfo` methods over JSON-RPC.
- The plugin must emit `onAVDecoderStatusChanged` notifications when AV decoder/pipeline status changes.
- `getConfiguration` must accept an array of property name strings and return their corresponding values as name-value pairs.
- `getMilestones` must return the current list of RDK milestone strings.
- `logMilestone` must write the given marker string to the RDK milestones log (when `RDK_LOG_MILESTONE` is enabled).
- `getPreviousRebootInfo` must return structured reboot information including reason, timestamp, and related metadata.
- The plugin must support optional AV decoder status polling via the Essos Resource Manager (`ENABLE_ERM` build flag).
- The plugin must register and unregister notification listeners through `IDeviceDiagnostics::INotification`.
- The plugin must handle remote connection deactivation gracefully.

## Architecture / Design
- **Out-of-process architecture:** `DeviceDiagnostics` (proxy plugin) runs in the framework process; `DeviceDiagnosticsImplementation` runs in a separate process and communicates via COM-RPC.
- **Interface:** Both classes implement `Exchange::IDeviceDiagnostics` as defined in `IDeviceDiagnostics.h`.
- **Notification flow:** `DeviceDiagnosticsImplementation` calls `dispatchEvent` → dispatches a `Job` on the Thunder worker pool → invokes `OnAVDecoderStatusChanged` on all registered `INotification` listeners → the proxy's `Notification` sink forwards the event to JSON-RPC clients via `JDeviceDiagnostics::Event::OnAVDecoderStatusChanged`.
- **AV decoder polling (optional):** When built with `ENABLE_ERM`, a background thread (`AVPollThread`) polls the Essos Resource Manager every `AVDECODERSTATUS_RETRY_INTERVAL` (30 s) for decoder status changes.
- **Plugin lifecycle:** `Initialize` / `Deinitialize` manage COM-RPC connection, notification registration, and JSONRPC handler setup.

## External Interfaces

### JSON-RPC Methods

| Method                  | Parameters                          | Returns                                                             | Description                                              |
|-------------------------|-------------------------------------|---------------------------------------------------------------------|----------------------------------------------------------|
| `getAVDecoderStatus`    | None                                | `{ avDecoderStatus: string }`                                       | Returns the most active AV decoder/pipeline status.      |
| `getConfiguration`      | `{ names: string[] }`               | `{ paramList: [{ name: string, value: string }], success: boolean }` | Returns values for specified property names.             |
| `getMilestones`         | None                                | `{ milestones: string[], success: boolean }`                        | Returns the list of RDK milestone strings.               |
| `logMilestone`          | `{ marker: string }`                | `{ success: boolean }`                                              | Logs a marker string to the RDK milestones log.          |
| `getPreviousRebootInfo` | None                                | `{ rebootInfo: object, success: boolean }`                          | Returns information about the previous device reboot.    |

### JSON-RPC Events

| Event                      | Parameters                         | Description                                                |
|----------------------------|------------------------------------|------------------------------------------------------------|
| `onAVDecoderStatusChanged` | `{ avDecoderStatusChange: string }` | Emitted when AV decoder/pipeline status changes.           |

### Plugin Configuration

| Field       | Value                                      |
|-------------|--------------------------------------------|
| callsign    | `org.rdk.DeviceDiagnostics`                |
| classname   | `DeviceDiagnostics`                        |
| locator     | `libWPEFrameworkDeviceDiagnostics.so`      |
| autostart   | configurable (boolean)                     |

## Performance
_Not applicable — The plugin relies on event-driven notification and on-demand RPC calls. AV decoder polling interval (30 s) is fixed at compile time via `AVDECODERSTATUS_RETRY_INTERVAL`. No specific latency or throughput targets are defined._

## Security
- Access to diagnostic APIs should be restricted to authorized clients. Security tokens or Thunder security plugin integration should be applied as per deployment policy.
- The `logMilestone` method writes to the system milestone log; callers should be trusted processes only.
- No sensitive data (e.g., credentials or PII) is exposed through the current API surface.

## Versioning & Compatibility
- The plugin implements `Exchange::IDeviceDiagnostics` as defined in the `entservices-apis` repository.
- Build-time flags (`ENABLE_ERM`, `RDK_LOG_MILESTONE`, `USE_THUNDER_R4`) control optional features and Thunder version compatibility.
- API changes must remain backward-compatible or be versioned appropriately in the interface definition.

## Conformance Testing & Validation
- **L1 Tests:** Unit tests located at `Tests/L1Tests/tests/test_DeviceDiagnostics.cpp`.
- **L2 Tests:** Integration tests located at `Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp`.
- Test build configuration: `Tests/CMakeLists.txt`, `Tests/gcc-with-coverage.cmake`, `Tests/clang.cmake`.
- Coverity static analysis must be run as part of the CI pipeline.

## Covered Code
- `plugin/DeviceDiagnostics.h`:
    - `DeviceDiagnostics` (class)
    - `DeviceDiagnostics::Notification` (inner class)
    - `DeviceDiagnostics::Initialize`
    - `DeviceDiagnostics::Deinitialize`
    - `DeviceDiagnostics::Information`
    - `DeviceDiagnostics::Deactivated`
- `plugin/DeviceDiagnostics.cpp`:
    - `DeviceDiagnostics::Initialize`
    - `DeviceDiagnostics::Deinitialize`
    - `DeviceDiagnostics::Information`
    - `DeviceDiagnostics::Deactivated`
- `plugin/DeviceDiagnosticsImplementation.h`:
    - `DeviceDiagnosticsImplementation` (class)
    - `DeviceDiagnosticsImplementation::Job` (inner class)
    - `DeviceDiagnosticsImplementation::Register`
    - `DeviceDiagnosticsImplementation::Unregister`
    - `DeviceDiagnosticsImplementation::GetConfiguration`
    - `DeviceDiagnosticsImplementation::GetMilestones`
    - `DeviceDiagnosticsImplementation::LogMilestone`
    - `DeviceDiagnosticsImplementation::GetAVDecoderStatus`
    - `DeviceDiagnosticsImplementation::GetPreviousRebootInfo`
- `plugin/DeviceDiagnosticsImplementation.cpp`:
    - `DeviceDiagnosticsImplementation::GetConfiguration`
    - `DeviceDiagnosticsImplementation::GetMilestones`
    - `DeviceDiagnosticsImplementation::LogMilestone`
    - `DeviceDiagnosticsImplementation::GetAVDecoderStatus`
    - `DeviceDiagnosticsImplementation::GetPreviousRebootInfo`
    - `DeviceDiagnosticsImplementation::dispatchEvent`
    - `DeviceDiagnosticsImplementation::Dispatch`
    - `DeviceDiagnosticsImplementation::AVPollThread` (conditional on `ENABLE_ERM`)
- `plugin/Module.h`:
    - Module declaration
- `plugin/Module.cpp`:
    - Module registration

---

## Open Queries
- The return schema for `getPreviousRebootInfo` (fields in `RebootInfo`) is not yet fully documented in this spec — the structure should be confirmed against `IDeviceDiagnostics.h`.
- The exact set of supported property names for `getConfiguration` is not enumerated here; a normative list or reference to a device-specific configuration guide should be added.
- Security/authorization mechanism (e.g., Thunder Security plugin token enforcement) is not yet defined — deployment requirements should clarify which methods require elevated trust.
- The `autostart` default value in the plugin configuration should be explicitly documented.

## References
- [DeviceDiagnostics API Documentation](https://github.com/rdkcentral/entservices-apis/blob/develop/docs/apis/DeviceDiagnostics.md)
- [IDeviceDiagnostics.h Interface](https://github.com/rdkcentral/entservices-apis/blob/develop/apis/DeviceDiagnostics/IDeviceDiagnostics.h)
- [Thunder Framework](https://rdkcentral.github.io/Thunder/)
- [Openspec change: add-new-api-get-previous-reboot-info](../changes/archive/2026-04-16-add-new-api-get-previous-reboot-info/proposal.md)

## Change History
- [2025-01-01] - Initial draft - Original spec authored.
- [2026-04-23] - openspec-templater - Restructured to match spec template.
