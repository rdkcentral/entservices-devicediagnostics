## ADDED Requirements

### Requirement: GetPreviousRebootInfo returns structured reboot details
The `DeviceDiagnostics` plugin SHALL expose a `getPreviousRebootInfo` JSON-RPC method that returns structured information about the most recent device reboot.

The method SHALL read reboot details from `/opt/secure/reboot/previousreboot.info` and the last hard power reset timestamp from `/opt/secure/reboot/hardpower.info`.

The JSON-RPC response SHALL follow this structure:

**Request:**
```json
{
    "jsonrpc": "2.0",
    "id": 42,
    "method": "org.rdk.DeviceDiagnostics.getPreviousRebootInfo"
}
```

**Response:**
```json
{
    "jsonrpc": "2.0",
    "id": 42,
    "result": {
        "rebootInfo": {
            "timestamp": "20200128083540",
            "source": "SystemPlugin",
            "reason": "FIRMWARE_FAILURE",
            "customReason": "API Validation",
            "otherReason": "API Validation",
            "lastHardPowerReset": "Tue Jan 28 08:22:22 UTC 2020"
        },
        "success": true
    }
}
```

The C++ implementation method signature SHALL be:
```cpp
Core::hresult GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success);
```

Where `RebootInfo` is:
```cpp
struct EXTERNAL RebootInfo {
    string timestamp       /* @text timestamp */;
    string source          /* @text source */;
    string reason          /* @text reason */;
    string customReason    /* @text customReason */;
    string otherReason     /* @text otherReason */;
    string lastHardPowerReset /* @text lastHardPowerReset */;
};
```

The method SHALL return `Core::ERROR_NONE` on success and `Core::ERROR_GENERAL` on error.

#### Scenario: Device has rebooted and info file exists
- **WHEN** the device has rebooted and `/opt/secure/reboot/previousreboot.info` exists and is readable
- **THEN** the API SHALL return `success: true` and populate all available fields in `rebootInfo`

#### Scenario: Previousreboot.info file is missing
- **WHEN** `/opt/secure/reboot/previousreboot.info` does not exist
- **THEN** the API SHALL return `success: false`

#### Scenario: Both info files exist
- **WHEN** both `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info` exist
- **THEN** the `lastHardPowerReset` field SHALL be populated from `/opt/secure/reboot/hardpower.info`

#### Scenario: Hardpower.info file is missing
- **WHEN** `/opt/secure/reboot/hardpower.info` does not exist
- **THEN** the `lastHardPowerReset` field SHALL be left empty and the API SHALL still return `success: true` if the primary reboot info file was read successfully

### Requirement: GetPreviousRebootInfo parses reboot info file fields
The implementation SHALL parse the following key-value fields from `/opt/secure/reboot/previousreboot.info` using regex or line-based parsing:

| Field in file           | Maps to response field |
|-------------------------|------------------------|
| `PreviousRebootTime:`   | `timestamp`            |
| `PreviousRebootReason:` | `reason`               |
| `PreviousRebootInitiatedBy:` | `source`          |
| `PreviousCustomReason:` | `customReason`         |
| `PreviousOtherReason:`  | `otherReason`          |

Fields not present in the file SHALL default to the empty string `""`.

#### Scenario: All fields present in file
- **WHEN** the previousreboot.info file contains all expected key-value pairs
- **THEN** all fields in `rebootInfo` SHALL be populated with their corresponding values

#### Scenario: Some fields missing from file
- **WHEN** the previousreboot.info file is missing one or more expected fields
- **THEN** the missing fields SHALL default to `""` and the API SHALL still return `success: true`

### Requirement: File existence checked with Core::File
The implementation SHALL use `Core::File` to check whether the reboot info files exist before attempting to read them. A helper function `GetFileContent` SHALL be implemented to read the entire content of a file into a string.

#### Scenario: File existence check before read
- **WHEN** `GetPreviousRebootInfo` is called
- **THEN** the implementation SHALL use `Core::File` to verify the existence of `/opt/secure/reboot/previousreboot.info` before attempting to open or read it

#### Scenario: GetFileContent reads file successfully
- **WHEN** a valid, readable file path is provided to `GetFileContent`
- **THEN** the function SHALL return the full file contents as a string and return `true`

#### Scenario: GetFileContent fails on missing file
- **WHEN** a non-existent file path is provided to `GetFileContent`
- **THEN** the function SHALL return `false` and leave the output string empty
