## ADDED Requirements

### Requirement: Retrieve previous reboot information
The DeviceDiagnostics plugin SHALL provide a Thunder API named `getPreviousRebootInfo` that returns the previous reboot's timestamp, source, reason, customReason, otherReason, and lastHardPowerReset. The API SHALL read these values from `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info`. The API SHALL return `Core::ERROR_NONE` on success and `Core::ERROR_GENERAL` on error. The response format SHALL match the following structure:

```
{
    "jsonrpc": "2.0",
    "id": <int>,
    "result": {
        "rebootInfo": {
            "timestamp": "<string>",
            "source": "<string>",
            "reason": "<string>",
            "customReason": "<string>",
            "otherReason": "<string>",
            "lastHardPowerReset": "<string>"
        },
        "success": <bool>
    }
}
```

#### Scenario: Successful retrieval
- **WHEN** the API is called and both files exist and are readable
- **THEN** the API returns all fields populated and `success: true` with `Core::ERROR_NONE`

#### Scenario: Missing or unreadable files
- **WHEN** the API is called and either file is missing or unreadable
- **THEN** the API returns `success: false` and `Core::ERROR_GENERAL`

#### Scenario: App integration
- **WHEN** an app calls `getPreviousRebootInfo` after a device reboot
- **THEN** the app can use the returned data to send the reboot reason to the SIFT endpoint as part of diagnostics
