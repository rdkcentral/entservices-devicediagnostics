## Why

After a device reboot (scheduled, unscheduled, user-initiated, or system-initiated), apps need to know the exact reboot reason so they can include that information in diagnostics data sent to the SIFT endpoint. The `DeviceDiagnostics` plugin currently lacks a dedicated API for retrieving previous reboot information, forcing apps to either rely on another plugin or parse raw files directly.

## What Changes

- **New API** `getPreviousRebootInfo` added to the `DeviceDiagnostics` Thunder plugin.
- The API reads reboot details from `/opt/secure/reboot/previousreboot.info` and the last hard power reset timestamp from `/opt/secure/reboot/hardpower.info`.
- The implementation is added to `DeviceDiagnosticsImplementation.cpp` as `GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success)`.
- The `RebootInfo` structure (with fields: `timestamp`, `source`, `reason`, `customReason`, `otherReason`, `lastHardPowerReset`) is defined in the Exchange interface (outside this repo); this change implements the method body.
- All methods return `Core::ERROR_NONE` on success and `Core::ERROR_GENERAL` on error.

## Capabilities

### New Capabilities
- `get-previous-reboot-info`: Retrieves the previous reboot information (timestamp, source, reason, custom reason, other reason, last hard power reset) from device-side reboot log files and returns it via a structured JSON-RPC response.

### Modified Capabilities
- `device-diagnostics`: The existing `DeviceDiagnostics` plugin spec is extended to include the new `getPreviousRebootInfo` method and its response contract.

## Impact

- **`plugin/DeviceDiagnosticsImplementation.cpp`**: New method `GetPreviousRebootInfo` implemented.
- **`plugin/DeviceDiagnosticsImplementation.h`**: Method declaration added (if not already in the interface).
- **Exchange interface** (outside this repo): `IDeviceDiagnostics` interface and `RebootInfo` struct already assumed to be defined there.
- **JSON-RPC stub** (`Exchange::JDeviceDiagnostics`): Auto-generated from the interface; no manual registration needed.
- **No breaking changes** to existing APIs.
