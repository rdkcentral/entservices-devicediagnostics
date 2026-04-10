# Previous Reboot Info Specification

## Overview

This specification defines the requirements for retrieving detailed information about the device's previous reboot. This capability enables applications to access diagnostic data including reboot timestamp, triggering source, reason codes, and hardware power reset information for monitoring and troubleshooting purposes.

## ADDED Requirements

### Requirement: Retrieve previous reboot information

The DeviceDiagnostics plugin SHALL provide a `getPreviousRebootInfo` API that returns comprehensive reboot diagnostic data including timestamp, source, reason, custom reason, other reason, and last hardware power reset time.

#### Scenario: Successful retrieval of reboot information
- **WHEN** a client invokes `org.rdk.DeviceDiagnostics.getPreviousRebootInfo` JSON-RPC method
- **THEN** the system returns a JSON object containing all reboot information fields with `success: true`

#### Scenario: Reboot info available after device restart
- **WHEN** the device reboots for any reason (scheduled, unscheduled, user-initiated, system-initiated)
- **AND** the device completes boot sequence
- **AND** a client calls `getPreviousRebootInfo`
- **THEN** the API returns the exact reason and metadata from the previous boot cycle

#### Scenario: COM-RPC interface access
- **WHEN** a Thunder plugin queries the `IDeviceDiagnostics` COM-RPC interface
- **AND** invokes `GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success)`
- **THEN** the method populates the `RebootInfo` structure with all fields and sets `success` appropriately

### Requirement: Read reboot data from persistent storage

The implementation SHALL read reboot information from `/opt/secure/reboot/previousreboot.info` file and last hardware power reset timestamp from `/opt/secure/reboot/hardpower.info` file.

#### Scenario: Parse previousreboot.info file
- **WHEN** the API is invoked
- **AND** `/opt/secure/reboot/previousreboot.info` file exists
- **THEN** the system SHALL parse the file to extract timestamp, source, reason, customReason, and otherReason fields

#### Scenario: Parse hardpower.info file
- **WHEN** the API is invoked
- **AND** `/opt/secure/reboot/hardpower.info` file exists
- **THEN** the system SHALL read the last hardware power reset timestamp

#### Scenario: Missing reboot info files
- **WHEN** the API is invoked
- **AND** one or both reboot info files do not exist
- **THEN** the system SHALL return `success: false` and appropriate error indication

#### Scenario: Malformed file content
- **WHEN** the API is invoked
- **AND** the reboot info files contain malformed or unparseable content
- **THEN** the system SHALL return `success: false` and log the parse error

### Requirement: Return structured reboot information

The API response SHALL include a `RebootInfo` structure containing six fields: timestamp, source, reason, customReason, otherReason, and lastHardPowerReset.

#### Scenario: All fields populated
- **WHEN** complete reboot information is available
- **THEN** all six fields in the `RebootInfo` structure SHALL be populated with non-empty values

#### Scenario: Field format validation
- **WHEN** the API returns reboot information
- **THEN** `timestamp` SHALL be in format "YYYYMMDDHHmmss" (e.g., "20200128083540")
- **AND** `lastHardPowerReset` SHALL be a human-readable timestamp (e.g., "Tue Jan 28 08:22:22 UTC 2020")
- **AND** `source` SHALL identify the component that triggered the reboot (e.g., "SystemPlugin")
- **AND** `reason` SHALL be a standardized reason code (e.g., "FIRMWARE_FAILURE", "USER_REBOOT")

#### Scenario: JSON-RPC response format
- **WHEN** a JSON-RPC client calls the API
- **THEN** the response SHALL conform to this structure:
  ```json
  {
      "jsonrpc": "2.0",
      "id": <request_id>,
      "result": {
          "rebootInfo": {
              "timestamp": "<YYYYMMDDHHmmss>",
              "source": "<component_name>",
              "reason": "<REASON_CODE>",
              "customReason": "<custom_text>",
              "otherReason": "<additional_text>",
              "lastHardPowerReset": "<readable_timestamp>"
          },
          "success": true
      }
  }
  ```

### Requirement: Error handling

The API SHALL return `success: false` when reboot information cannot be retrieved, with appropriate logging of the failure reason.

#### Scenario: File read permission denied
- **WHEN** the API attempts to read reboot info files
- **AND** file permissions deny read access
- **THEN** the system SHALL return `success: false` and log "Permission denied" error

#### Scenario: File system error
- **WHEN** a file system error occurs during file read
- **THEN** the system SHALL return `success: false` and log the specific error

#### Scenario: Partial data available
- **WHEN** only one of the two reboot info files is readable
- **THEN** the system SHALL return available data with `success: true` and leave unavailable fields empty

### Requirement: Integration with monitoring systems

The reboot information SHALL be suitable for transmission to the SIFT monitoring endpoint for diagnostic analysis.

#### Scenario: SIFT endpoint data format
- **WHEN** an application retrieves reboot information via this API
- **THEN** the data format SHALL be compatible with SIFT diagnostic data schema requirements

#### Scenario: Comprehensive diagnostic data
- **WHEN** reboot information is sent to monitoring endpoints
- **THEN** it SHALL provide sufficient detail to diagnose scheduled vs. unscheduled reboots, user-initiated vs. system-initiated reboots, and failure scenarios

## API Contract

### JSON-RPC Method

**Endpoint:** `org.rdk.DeviceDiagnostics.getPreviousRebootInfo`

**Request:**
```json
{
    "jsonrpc": "2.0",
    "id": 42,
    "method": "org.rdk.DeviceDiagnostics.getPreviousRebootInfo"
}
```

**Response (Success):**
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

**Response (Failure):**
```json
{
    "jsonrpc": "2.0",
    "id": 42,
    "result": {
        "rebootInfo": {},
        "success": false
    }
}
```

### COM-RPC Interface

**Method Signature:**
```cpp
Core::hresult GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success);
```

**RebootInfo Structure:**
```cpp
struct EXTERNAL RebootInfo {
    string timestamp;           // Format: YYYYMMDDHHmmss
    string source;              // Component that triggered reboot
    string reason;              // Standardized reason code
    string customReason;        // Custom reason text
    string otherReason;         // Additional reason details
    string lastHardPowerReset;  // Human-readable timestamp
};
```

**Return Values:**
- `Core::ERROR_NONE` - Success
- `Core::ERROR_GENERAL` - Error occurred (check logs)

## File Format Specifications

### /opt/secure/reboot/previousreboot.info

Expected format (key-value pairs):
```
timestamp=20200128083540
source=SystemPlugin
reason=FIRMWARE_FAILURE
customReason=API Validation
otherReason=API Validation
```

### /opt/secure/reboot/hardpower.info

Expected format (single line with timestamp):
```
Tue Jan 28 08:22:22 UTC 2020
```

## Non-Functional Requirements

### Performance
- API response time SHALL be < 50ms under normal conditions
- File parsing SHALL handle files up to 4KB efficiently

### Reliability
- Implementation SHALL handle missing, empty, or malformed files gracefully
- File read operations SHALL use appropriate error handling

### Observability
- All file read errors SHALL be logged with ERROR level
- Parse failures SHALL be logged with specific error details
- Successful API calls MAY be logged at INFO level for audit purposes

## Dependencies

- **File System:** `/opt/secure/reboot/` directory must exist with appropriate permissions
- **Thunder Framework:** JSON-RPC dispatcher and error handling
- **IDeviceDiagnostics Interface:** Interface definition must include `GetPreviousRebootInfo()` method signature
