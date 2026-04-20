# DeviceDiagnostics Plugin Specification

## Overview
The DeviceDiagnostics plugin provides an interface for device diagnostics within the Thunder framework. It enables clients to query AV decoder status, retrieve configuration properties, list milestones, and log milestone markers. The plugin is designed to be extensible and integrates with system APIs and hardware drivers as needed.

---

## Table of Contents
- Purpose
- API Methods
- Notifications
- Data Models
- Configuration
- Security & Permissions
- Extensibility
- References

---

## Purpose
The DeviceDiagnostics plugin exposes device health and diagnostic information, focusing on AV decoder status and milestone tracking, to clients via JSON-RPC.

---

## API Methods

| Method                | Description                                                    |
|-----------------------|----------------------------------------------------------------|
| getAVDecoderStatus    | Gets the most active status of audio/video decoder/pipeline     |
| getConfiguration      | Gets values for specified property names                        |
| getMilestones         | Returns a list of milestones                                   |
| logMilestone          | Logs a marker string to RDK milestones log                     |

### Method Details

#### getAVDecoderStatus
- **Parameters:** None
- **Returns:** `{ avDecoderStatus: string }`
- **Description:** Returns the current status of the AV decoder or pipeline.

#### getConfiguration
- **Parameters:** `{ names: string[] }` (array of property names)
- **Returns:** `{ paramList: [{ name: string, value: string }], success: boolean }`
- **Description:** Returns the values for the specified property names.

#### getMilestones
- **Parameters:** None
- **Returns:** `{ milestones: string[], success: boolean }`
- **Description:** Returns a list of milestone strings.

#### logMilestone
- **Parameters:** `{ marker: string }`
- **Returns:** `{ success: boolean }`
- **Description:** Logs a marker string to the RDK milestones log.

---

## Notifications

| Event                     | Description                                                    |
|---------------------------|----------------------------------------------------------------|
| onAVDecoderStatusChanged  | Triggered when AV decoder/pipeline status changes              |

### Event Details

#### onAVDecoderStatusChanged
- **Parameters:** `{ avDecoderStatusChange: string }`
- **Description:** Notifies clients when the AV decoder status changes.

---

## Data Models
- **AV Decoder Status:** String representing the current state of the AV decoder/pipeline.
- **Configuration Properties:** List of name-value pairs.
- **Milestones:** List of strings.
- **Success Flag:** Boolean indicating operation success.

---

## Configuration
- **callsign:** Plugin instance name (default: org.rdk.DeviceDiagnostics)
- **classname:** Class name: DeviceDiagnostics
- **locator:** Library name: libWPEFrameworkDeviceDiagnostics.so
- **autostart:** Boolean to autostart plugin with framework

---

## Security & Permissions
- [To be defined based on deployment requirements. Consider restricting access to authorized clients.]

---

## Extensibility
- The plugin can be extended with new diagnostic methods or properties as needed.
- Data models should be updated to reflect new diagnostics.

---

## References
- [DeviceDiagnostics.md](https://github.com/rdkcentral/entservices-apis/blob/develop/docs/apis/DeviceDiagnostics.md)
- [Thunder Framework](https://rdkcentral.github.io/Thunder/)
- [IDeviceDiagnostics.h](https://github.com/rdkcentral/entservices-apis/blob/develop/apis/DeviceDiagnostics/IDeviceDiagnostics.h)
