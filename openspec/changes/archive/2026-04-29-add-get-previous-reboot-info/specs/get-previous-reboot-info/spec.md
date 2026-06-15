## ADDED Requirements

### Requirement: GetPreviousRebootInfo returns structured reboot details
The `DeviceDiagnostics` plugin SHALL expose a `getPreviousRebootInfo` JSON-RPC method that returns structured information about the most recent device reboot.

The method SHALL read reboot details from `/opt/secure/reboot/previousreboot.info`. This file must exist, be non-empty, and contain valid JSON — if not, the API returns `Core::ERROR_GENERAL`. The last hard power reset timestamp is read from `/opt/secure/reboot/hardpower.info`; if this file is missing, unreadable, contains invalid JSON, or lacks the `lastHardPowerReset` key, `lastHardPowerReset` SHALL be set to `"Unknown"` and the API SHALL still return success.

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
    string timestamp          /* @text timestamp */;
    string source             /* @text source */;
    string reason             /* @text reason */;
    string customReason       /* @text customReason */;
    string otherReason        /* @text otherReason */;
    string lastHardPowerReset /* @text lastHardPowerReset */;
};
```

The method SHALL return `Core::ERROR_NONE` on success and `Core::ERROR_GENERAL` only if `previousreboot.info` is missing, empty, or contains invalid JSON.

#### Scenario: Device has rebooted and both files exist with valid JSON
- **WHEN** both `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info` exist, are non-empty, and contain valid JSON
- **THEN** the API SHALL return `success: true` and populate all available fields in `rebootInfo`

#### Scenario: previousreboot.info file is missing
- **WHEN** `/opt/secure/reboot/previousreboot.info` does not exist
- **THEN** the API SHALL return `Core::ERROR_GENERAL` and `success: false`

#### Scenario: hardpower.info file is missing
- **WHEN** `/opt/secure/reboot/hardpower.info` does not exist
- **THEN** the API SHALL return `Core::ERROR_NONE` and `success: true` with `lastHardPowerReset` set to `"Unknown"`

#### Scenario: Both info files exist with valid JSON
- **WHEN** both `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info` exist and contain valid JSON
- **THEN** `lastHardPowerReset` SHALL be read from the `lastHardPowerReset` key of `/opt/secure/reboot/hardpower.info`

### Requirement: GetPreviousRebootInfo parses reboot info files as JSON
Both `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info` SHALL be in JSON format. The implementation SHALL use `JsonObject::FromString()` to parse the files.

The fields in `/opt/secure/reboot/previousreboot.info` SHALL map as follows:

| JSON key in file        | Maps to `RebootInfo` field |
|-------------------------|---------------------------|
| `timestamp`             | `timestamp`               |
| `source`                | `source`                  |
| `reason`                | `reason`                  |
| `customReason`          | `customReason`            |
| `otherReason`           | `otherReason`             |

The field in `/opt/secure/reboot/hardpower.info`:

| JSON key in file        | Maps to `RebootInfo` field |
|-------------------------|---------------------------|
| `lastHardPowerReset`    | `lastHardPowerReset`      |

Fields not present in the JSON object SHALL default to the empty string `""`.

#### Scenario: All fields present in both JSON files
- **WHEN** both files contain all expected JSON keys with non-empty values
- **THEN** all fields in `rebootInfo` SHALL be populated with their corresponding values and `success` SHALL be `true`

#### Scenario: Some optional fields missing from JSON
- **WHEN** the JSON files are valid but missing one or more optional fields (e.g., `reason`, `customReason`)
- **THEN** missing fields SHALL default to `""`, the API SHALL still return `Core::ERROR_NONE` and `success: true`

#### Scenario: Invalid JSON in primaryreboot.info
- **WHEN** `/opt/secure/reboot/previousreboot.info` contains content that is not valid JSON
- **THEN** the API SHALL return `Core::ERROR_GENERAL` and `success: false`

#### Scenario: Invalid JSON in hardpower.info
- **WHEN** `/opt/secure/reboot/hardpower.info` contains content that is not valid JSON
- **THEN** the API SHALL return `Core::ERROR_NONE` and `success: true` with `lastHardPowerReset` set to `"Unknown"`

#### Scenario: Empty previousreboot.info
- **WHEN** `/opt/secure/reboot/previousreboot.info` exists but is empty
- **THEN** the API SHALL return `Core::ERROR_GENERAL` and `success: false`

### Requirement: File existence checked with Core::File and content read via free function overload
The implementation SHALL use `Core::File::Exists()` to check whether `previousreboot.info` exists before attempting to read it. File content SHALL be read using an overloaded free function `getFileContent(std::string, std::string&)` that reads the entire file into a string via `std::stringstream`.

#### Scenario: File existence check before read
- **WHEN** `GetPreviousRebootInfo` is called
- **THEN** the implementation SHALL use `Core::File` to verify the existence of `/opt/secure/reboot/previousreboot.info` before reading it
- **NOTE** No pre-existence check is performed on `/opt/secure/reboot/hardpower.info`; its absence is handled gracefully at the read/parse step

#### Scenario: getFileContent reads file successfully
- **WHEN** a valid, readable file path is provided to `getFileContent(string, string&)`
- **THEN** the function SHALL return the full file contents as a string and return `true`

#### Scenario: getFileContent fails on missing file
- **WHEN** a non-existent file path is provided to `getFileContent(string, string&)`
- **THEN** the function SHALL return `false`
