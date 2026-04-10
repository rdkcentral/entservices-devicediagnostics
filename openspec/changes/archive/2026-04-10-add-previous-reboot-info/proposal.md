## Why

Applications need accurate reboot reason information to send diagnostic data to the SIFT monitoring endpoint. Currently, the DeviceDiagnostics plugin lacks an API to retrieve detailed previous reboot information including timestamp, source, reason, and hardware power reset details. This prevents apps from properly diagnosing device behavior and reporting issues to the monitoring infrastructure.

## What Changes

- Add new `getPreviousRebootInfo` JSON-RPC method to DeviceDiagnostics plugin
- Implement `GetPreviousRebootInfo()` in DeviceDiagnosticsImplementation.cpp to read reboot data from persistent storage
- Add `RebootInfo` structure to interface definition with fields: timestamp, source, reason, customReason, otherReason, lastHardPowerReset
- Support reading reboot information from `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info` files
- Return structured JSON response with all reboot details for client consumption

## Capabilities

### New Capabilities
- `previous-reboot-info`: Retrieves detailed information about the last device reboot, including timestamp, triggering source, reason codes (standard and custom), and last hardware power reset time. Enables applications to send accurate diagnostic data to monitoring endpoints.

### Modified Capabilities
<!-- No existing capabilities are being modified -->

## Impact

**Affected Components:**
- `plugin/DeviceDiagnosticsImplementation.h` - Add method declaration
- `plugin/DeviceDiagnosticsImplementation.cpp` - Implement reboot info retrieval logic
- `plugin/DeviceDiagnostics.h` - Register JSON-RPC method (if needed)
- Interface definition (external repo) - Add IDeviceDiagnostics::GetPreviousRebootInfo() signature

**File Dependencies:**
- `/opt/secure/reboot/previousreboot.info` - Source of reboot metadata
- `/opt/secure/reboot/hardpower.info` - Source of hardware power reset timestamp

**API Surface:**
- New JSON-RPC endpoint: `org.rdk.DeviceDiagnostics.getPreviousRebootInfo`
- COM-RPC interface method: `IDeviceDiagnostics::GetPreviousRebootInfo()`

**Client Impact:**
- Applications gain ability to retrieve detailed reboot diagnostics
- No breaking changes to existing APIs
