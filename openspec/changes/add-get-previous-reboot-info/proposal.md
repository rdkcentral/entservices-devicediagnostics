## Why

After a device reboot (scheduled, unscheduled, user-initiated, or system-initiated), applications need to know the exact reason the device rebooted so they can include accurate diagnostic context when reporting to the SIFT analytics endpoint. There is currently no JSON-RPC API in the `DeviceDiagnostics` Thunder plugin that surfaces this information.

## What Changes

- **New JSON-RPC method** `getPreviousRebootInfo` added to the `org.rdk.DeviceDiagnostics` plugin.
- The method reads previous reboot metadata from `/opt/secure/reboot/previousreboot.info` (fields: `timestamp`, `source`, `reason`, `customReason`, `otherReason`) and the last hard-power-reset timestamp from `/opt/secure/reboot/hardpower.info`.
- A new COM-RPC method `GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success)` is added to the `Exchange::IDeviceDiagnostics` interface (`IDeviceDiagnostics.h`) — the interface lives in the external `entservices-apis` repo.
- The implementation is added to `DeviceDiagnosticsImplementation.cpp`, using the same file-read pattern as `getPreviousRebootInfo2()` in `SystemServices.cpp`.
- The `RebootInfo` struct (already defined in `IDeviceDiagnostics.h` but not yet exposed via JSON-RPC) is leveraged directly; no new types are required.
- All methods return `Core::ERROR_NONE` on success and `Core::ERROR_GENERAL` on error (e.g., file missing or unreadable).

## Capabilities

### New Capabilities

- `get-previous-reboot-info`: JSON-RPC API `getPreviousRebootInfo` on the `DeviceDiagnostics` plugin that returns a `rebootInfo` object containing `timestamp`, `source`, `reason`, `customReason`, `otherReason`, and `lastHardPowerReset`.

### Modified Capabilities

- `device-diagnostics`: The existing `DeviceDiagnostics` plugin spec (`specs/DeviceDiagnostics_Spec.md`) must be updated to document the new method, its request/response format, the new COM-RPC signature, and the file dependencies.

## Impact

- **Code**: `plugin/DeviceDiagnosticsImplementation.cpp`, `plugin/DeviceDiagnosticsImplementation.h`
- **Interface (external repo)**: `Exchange::IDeviceDiagnostics` in `entservices-apis` — `GetPreviousRebootInfo` method and the pre-existing `RebootInfo` struct must be annotated for JSON auto-generation (`@json`).
- **Spec**: `specs/DeviceDiagnostics_Spec.md` — new method documented; `RebootInfo` struct usage resolved (closes OQ-01).
- **Files read at runtime**: `/opt/secure/reboot/previousreboot.info`, `/opt/secure/reboot/hardpower.info`
- **No breaking changes** to existing APIs; purely additive.
- **No new build dependencies** — file I/O only, no new libraries required.
