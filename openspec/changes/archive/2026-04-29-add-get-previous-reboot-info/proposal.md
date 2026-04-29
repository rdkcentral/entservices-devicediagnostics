## Why

After a device reboot (scheduled, unscheduled, user-initiated, or system-initiated), apps need to know the exact reboot reason so they can include that information in diagnostics data sent to the SIFT endpoint. The `DeviceDiagnostics` plugin currently lacks a dedicated API for retrieving previous reboot information, forcing apps to either rely on another plugin or parse raw files directly.

## What Changes

- **New API** `getPreviousRebootInfo` added to the `DeviceDiagnostics` Thunder plugin.
- Both `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info` are read and parsed as **JSON**. Both files must exist, be non-empty, and contain valid JSON for the API to succeed.
- The implementation is added to `DeviceDiagnosticsImplementation.cpp` as `GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success)`.
- A free function overload `getFileContent(std::string, std::string&)` is added alongside the existing list-based overload to read file contents into a string using `std::stringstream`.
- `Core::File::Exists()` is used to check existence of both files before reading.
- The `RebootInfo` structure (with fields: `timestamp`, `source`, `reason`, `customReason`, `otherReason`, `lastHardPowerReset`) is defined in the Exchange interface (outside this repo); this change implements the method body.
- All methods return `Core::ERROR_NONE` on success and `Core::ERROR_GENERAL` on any error (missing file, empty file, or invalid JSON).

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
