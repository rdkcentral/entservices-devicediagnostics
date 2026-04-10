# DeviceDiagnostics Plugin

## Build

```bash
bitbake wpeframework-service-plugins
```

## Usage Examples

### JSON-RPC API

#### Get Previous Reboot Info

```bash
curl --header "Content-Type: application/json" \
     --request POST \
     --data '{"jsonrpc":"2.0","id":"42","method":"org.rdk.DeviceDiagnostics.getPreviousRebootInfo"}' \
     http://127.0.0.1:9998/jsonrpc
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

#### Other Methods

```bash
# Get AV Decoder Status
curl -H 'content-type:text/plain;' \
  --data-binary '{"jsonrpc":"2.0","id":"1","method":"org.rdk.DeviceDiagnostics.getAVDecoderStatus"}' \
  http://127.0.0.1:9998/jsonrpc

# Get Milestones
curl -H 'content-type:text/plain;' \
  --data-binary '{"jsonrpc":"2.0","id":"2","method":"org.rdk.DeviceDiagnostics.getMilestones"}' \
  http://127.0.0.1:9998/jsonrpc

# Log Milestone
curl -H 'content-type:text/plain;' \
  --data-binary '{"jsonrpc":"2.0","id":"3","method":"org.rdk.DeviceDiagnostics.logMilestone","params":{"marker":"VIDEO_START"}}' \
  http://127.0.0.1:9998/jsonrpc
```

### COM-RPC Interface (C++)

```cpp
#include <interfaces/IDeviceDiagnostics.h>

// Obtain the interface
auto deviceDiagnostics = /* obtain IDeviceDiagnostics interface via Thunder */;

// Call GetPreviousRebootInfo
Exchange::IDeviceDiagnostics::RebootInfo rebootInfo;
bool success = false;

Core::hresult result = deviceDiagnostics->GetPreviousRebootInfo(rebootInfo, success);

if (result == Core::ERROR_NONE && success) {
    printf("Reboot timestamp: %s\n", rebootInfo.timestamp.c_str());
    printf("Reboot source: %s\n", rebootInfo.source.c_str());
    printf("Reboot reason: %s\n", rebootInfo.reason.c_str());
    printf("Custom reason: %s\n", rebootInfo.customReason.c_str());
    printf("Other reason: %s\n", rebootInfo.otherReason.c_str());
    printf("Last hard power reset: %s\n", rebootInfo.lastHardPowerReset.c_str());
} else {
    printf("Failed to retrieve reboot info\n");
}
```

## Required System Files

For **getPreviousRebootInfo** API:
- `/opt/secure/reboot/previousreboot.info` - Reboot metadata (key=value format)
- `/opt/secure/reboot/hardpower.info` - Hardware power reset timestamp

Expected format for `previousreboot.info`:
```
timestamp=20200128083540
source=SystemPlugin
reason=FIRMWARE_FAILURE
customReason=API Validation
otherReason=API Validation
```

Expected format for `hardpower.info`:
```
Tue Jan 28 08:22:22 UTC 2020
```
