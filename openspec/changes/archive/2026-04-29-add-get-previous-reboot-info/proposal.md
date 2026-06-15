## Why

After a device reboot (scheduled, unscheduled, user-initiated, or system-initiated), apps need to know the exact reboot reason so they can include that information in diagnostics data sent to the SIFT endpoint. The `DeviceDiagnostics` plugin currently lacks a dedicated API for retrieving previous reboot information, forcing apps to either rely on another plugin or parse raw files directly.

## What Changes

- **New API** `getPreviousRebootInfo` added to the `DeviceDiagnostics` Thunder plugin.
- `/opt/secure/reboot/previousreboot.info` is read and parsed as **JSON**; it must exist, be non-empty, and contain valid JSON for the API to succeed — otherwise `Core::ERROR_GENERAL` is returned.
- `/opt/secure/reboot/hardpower.info` is also read and parsed as JSON to extract `lastHardPowerReset`. If this file is missing, unreadable, contains invalid JSON, or lacks the `lastHardPowerReset` key, `lastHardPowerReset` is set to `"Unknown"` and the API still returns `Core::ERROR_NONE` (non-fatal).
- If `lastHardPowerReset` value is empty or `"null"`, it is reported as `"Unknown"`.
- The implementation is added to `DeviceDiagnosticsImplementation.cpp` as `GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success)`.
- A free function overload `getFileContent(std::string, std::string&)` is added alongside the existing list-based overload to read file contents into a string using `std::stringstream`.
- `Core::File::Exists()` is used to check existence of `previousreboot.info` before reading.
- The `RebootInfo` structure (with fields: `timestamp`, `source`, `reason`, `customReason`, `otherReason`, `lastHardPowerReset`) is defined in the Exchange interface (outside this repo); this change implements the method body.

## Capabilities

### New Capabilities
- `get-previous-reboot-info`: Retrieves the previous reboot information (timestamp, source, reason, custom reason, other reason, last hard power reset) from device-side reboot log files and returns it via a structured JSON-RPC response.

### Modified Capabilities
- `device-diagnostics`: The existing `DeviceDiagnostics` plugin spec is extended to include the new `getPreviousRebootInfo` method and its response contract.

- All methods return `Core::ERROR_NONE` on success and `Core::ERROR_GENERAL` on error. `GetPreviousRebootInfo` returns `Core::ERROR_GENERAL` only if `previousreboot.info` is missing, empty, or invalid JSON; `hardpower.info` failure degrades gracefully to `lastHardPowerReset = "Unknown"`.

## Impact

- **`plugin/DeviceDiagnosticsImplementation.cpp`**: New method `GetPreviousRebootInfo` implemented.
- **`plugin/DeviceDiagnosticsImplementation.h`**: Method declaration added (if not already in the interface).
- **Exchange interface** (outside this repo): `IDeviceDiagnostics` interface and `RebootInfo` struct already assumed to be defined there.
- **JSON-RPC stub** (`Exchange::JDeviceDiagnostics`): Auto-generated from the interface; no manual registration needed.
- **No breaking changes** to existing APIs.
