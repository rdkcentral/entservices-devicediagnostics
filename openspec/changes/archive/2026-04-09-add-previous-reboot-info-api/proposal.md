## Why

DeviceDiagnostics plugin needs to provide diagnostic information about the device's last reboot event. Currently, the system stores reboot information in `/opt/secure/reboot/previousreboot.info` and hard power reset information in `/opt/secure/reboot/hardpower.info`, but lacks a Thunder plugin API to expose this data to applications. This API will enable applications to programmatically retrieve reboot diagnostics including timestamp, source, reason, custom reasons, and last hard power reset information.

## What Changes

- Add `RebootInfo` structure to `IDeviceDiagnostic` interface containing six string fields: timestamp, source, reason, customReason, otherReason, and lastHardPowerReset
- Add `getPreviousRebootInfo` method to `IDeviceDiagnostic` interface with signature: `Core::hresult GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success)`
- Implement the method in `DeviceDiagnosticsImplementation` to read and parse JSON from `/opt/secure/reboot/previousreboot.info` for the first 5 fields
- Read `lastHardPowerReset` field from `/opt/secure/reboot/hardpower.info`
- Populate RebootInfo structure with all six fields
- Return `Core::ERROR_GENERAL` for any error scenario (file not found, invalid JSON, etc.)
- Return result with nested `rebootInfo` object and top-level `success` boolean

## Capabilities

### New Capabilities
- `previous-reboot-info`: Provides access to the device's last reboot diagnostic information including when it occurred, what triggered it, the reboot reason code, and any custom or descriptive reasons

### Modified Capabilities
<!-- No existing capabilities are being modified -->

## Impact

- **Interface Change**: `IDeviceDiagnostic.h` will be modified to add the `RebootInfo` structure and new method signature
- **Implementation**: `DeviceDiagnosticsImplementation.cpp` will implement file reading and JSON parsing logic
- **Plugin Registration**: `DeviceDiagnostics.cpp` will register the new JSON-RPC method
- **Dependencies**: Uses JSON parsing utilities already available via Thunder Core
- **Backward Compatibility**: Non-breaking change - adds new functionality without modifying existing APIs
