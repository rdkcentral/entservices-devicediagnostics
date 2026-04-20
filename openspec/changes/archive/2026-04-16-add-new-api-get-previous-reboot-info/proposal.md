## Why

Applications and diagnostic systems require accurate information about the previous device reboot to provide better troubleshooting, analytics, and reporting. Currently, there is no standardized Thunder API in the DeviceDiagnostics plugin to retrieve the previous reboot reason and related metadata, which limits the ability of apps to send this information to external systems like SIFT for diagnosis.

## What Changes

- Add a new Thunder API named `getPreviousRebootInfo` to the DeviceDiagnostics plugin.
- The API will return structured information about the previous reboot, including timestamp, source, reason, customReason, otherReason, and lastHardPowerReset.
- The API will read its data from `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info`.
- The API will follow the specified request/response format and return `Core::ERROR_NONE` on success, `Core::ERROR_GENERAL` on error.
- The implementation will be added to `DeviceDiagnosticsImplementation.cpp`.

## Capabilities

### New Capabilities
- `previous-reboot-info`: Adds the ability for apps to query the previous reboot reason and related metadata from DeviceDiagnostics via Thunder API.

### Modified Capabilities
- (none)

## Impact

- Affects the DeviceDiagnostics plugin (DeviceDiagnosticsImplementation.cpp).
- Adds a new public API endpoint: `org.rdk.DeviceDiagnostics.getPreviousRebootInfo`.
- No breaking changes; only new functionality is added.
- Enables apps to send accurate reboot reason data to SIFT and other diagnostic endpoints.
