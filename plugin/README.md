-----------------
Build:

bitbake wpeframework-service-plugins

-----------------
Test:

curl --header "Content-Type: application/json" --request POST --data '{"jsonrpc":"2.0","id":"3","method": "DeviceDiagnostics.1."}' http://127.0.0.1:9998/jsonrpc

-----------------

## API Documentation

### GetPreviousRebootInfo

Retrieves diagnostic information about the device's last reboot event.

**Method:** `org.rdk.DeviceDiagnostics.getPreviousRebootInfo`

**Description:**
Returns comprehensive reboot information from persistent storage, including timestamp, source, reason, and custom descriptions. Data is read from two files:
- `/opt/secure/reboot/previousreboot.info` (primary - required)
- `/opt/secure/reboot/hardpower.info` (secondary - optional)

**Parameters:** None

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

**Fields:**
- `timestamp` (string): Timestamp of the reboot event (format: YYYYMMDDHHmmss)
- `source` (string): Source that initiated the reboot (e.g., "SystemPlugin")
- `reason` (string): Reboot reason code (e.g., "POWER_ON_RESET", "FIRMWARE_FAILURE")
- `customReason` (string): Custom description of the reboot reason
- `otherReason` (string): Additional descriptive information about the reboot
- `lastHardPowerReset` (string): Timestamp of the last hard power reset event
- `success` (boolean): Indicates whether the operation succeeded

**Error Handling:**
- Returns `success: false` if the primary reboot info file is missing or invalid
- Continues with empty `lastHardPowerReset` if the secondary file is unavailable
- Empty strings returned for any missing fields

**Example Request:**
```bash
curl --header "Content-Type: application/json" --request POST \
  --data '{"jsonrpc":"2.0","id":42,"method":"org.rdk.DeviceDiagnostics.getPreviousRebootInfo","params":{}}' \
  http://127.0.0.1:9998/jsonrpc
```

**Example Response (Success):**
```json
{
    "jsonrpc": "2.0",
    "id": 42,
    "result": {
        "rebootInfo": {
            "timestamp": "20240408120000",
            "source": "UserRequest",
            "reason": "POWER_ON_RESET",
            "customReason": "",
            "otherReason": "Reboot due to unplug of power cable from the STB",
            "lastHardPowerReset": "Tue Apr 08 12:00:00 UTC 2024"
        },
        "success": true
    }
}
```

**Example Response (Error - Primary File Not Found):**
```json
{
    "jsonrpc": "2.0",
    "id": 42,
    "result": {
        "rebootInfo": {
            "timestamp": "",
            "source": "",
            "reason": "",
            "customReason": "",
            "otherReason": "",
            "lastHardPowerReset": ""
        },
        "success": false
    }
}
```
