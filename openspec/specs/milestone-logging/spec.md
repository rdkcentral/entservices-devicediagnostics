# Milestone Logging Specification

## Overview

The Milestone Logging capability provides a standardized interface for recording and retrieving diagnostic milestones on RDK devices. Milestones are timestamped markers used to track system lifecycle events, performance metrics, and diagnostic checkpoints.

**Target Devices:** RDK TV and STB platforms  
**Version:** 1.0.0  
**Status:** Active

## Purpose

Enables observability of device behavior by providing a centralized mechanism to log and query diagnostic milestones. Used for boot time analysis, performance tracking, debugging, and field diagnostics.

## API Contract

### Method: `logMilestone`

Records a milestone marker to the RDK milestone log.

**Request:**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "org.rdk.DeviceDiagnostics.logMilestone",
  "params": {
    "marker": "VIDEO_FIRST_FRAME"
  }
}
```

**Response (Success):**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "success": true
  }
}
```

**Response (Failure):**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "success": false
  }
}
```

### Method: `getMilestones`

Retrieves all recorded milestones from the log file.

**Request:**
```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "method": "org.rdk.DeviceDiagnostics.getMilestones"
}
```

**Response (Success):**
```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "result": {
    "milestones": [
      "180101-00:01:45.123456 BOOT_START",
      "180101-00:01:48.456789 NETWORK_UP",
      "180101-00:01:52.789012 VIDEO_FIRST_FRAME"
    ],
    "success": true
  }
}
```

**Response (Failure):**
```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "result": {
    "milestones": [],
    "success": false
  }
}
```

## Functional Requirements

### FR-1: Milestone Recording (`logMilestone`)

#### FR-1.1: Parameter Validation
- **Requirement:** The plugin MUST reject empty marker strings
- **Behavior:** Returns `success: false` and logs error for empty markers
- **Rationale:** Empty markers provide no diagnostic value

#### FR-1.2: RDK Logger Integration
- **Requirement:** When `RDK_LOG_MILESTONE` is defined, MUST call `logMilestone()` from RDK logger
- **Behavior:** Delegates to platform-specific milestone logging facility
- **Format:** RDK logger adds timestamp and formats entry

#### FR-1.3: Build Configuration
- **Conditional Compilation:** Milestone logging only active when `RDK_LOG_MILESTONE` is defined
- **Disabled Behavior:** Returns `success: true` without logging (no-op) when build flag absent
- **Rationale:** Allows disabling milestone logging for production builds where overhead is unacceptable

#### FR-1.4: Storage
- **File Location:** `/opt/logs/rdk_milestones.log`
- **Ownership:** Written by RDK logger (not directly by plugin)
- **Persistence:** Survives process restarts (file-based storage)

### FR-2: Milestone Retrieval (`getMilestones`)

#### FR-2.1: File Validation
- **Requirement:** MUST check if `/opt/logs/rdk_milestones.log` exists before reading
- **Missing File Behavior:** Returns `success: false` with empty list
- **Error Logging:** Logs "Expected file not found" when file absent

#### FR-2.2: File Reading
- **Implementation:** Reads entire file line-by-line into memory
- **Line Filtering:** Skips empty lines (lines with size == 0)
- **Encoding:** Assumes UTF-8 text encoding

#### FR-2.3: Iterator Pattern
- **Requirement:** Result MUST be returned as `IStringIterator`
- **Rationale:** Supports efficient marshalling and large log files
- **Implementation:** Uses Thunder's `RPC::StringIterator`

#### FR-2.4: Error Handling
- **File Access Failure:** Returns `success: false` with empty list
- **Partial Read:** If file read fails mid-stream, returns partial results read so far
- **Permissions:** No explicit permission handling (relies on process privileges)

## Non-Functional Requirements

### NFR-1: Performance
- **Logging Overhead:** < 1ms per `logMilestone` call (delegated to RDK logger)
- **Retrieval Speed:** < 100ms for typical log files (< 1000 entries)
- **Memory Usage:** Entire log file loaded into memory (consider for large logs)

### NFR-2: Reliability
- **Thread Safety:** File I/O operations are not synchronized (assumes single writer)
- **Graceful Degradation:** Missing file or read errors return empty list, not crash
- **No Corruption:** RDK logger handles concurrent writes (plugin is read-only)

### NFR-3: Observability
- **Logging:** All errors logged (empty marker, file not found, read failure)
- **Diagnostics:** Plugin logs validation failures for troubleshooting

## Dependencies

### Runtime Dependencies
- **RDK Logger:** When `RDK_LOG_MILESTONE` is defined, requires `logMilestone()` function
- **File System:** `/opt/logs/` directory must exist with appropriate permissions
- **Thunder Framework:** Iterator types for result marshalling

### Compile-Time Dependencies
- **RDK_LOG_MILESTONE:** Build flag controlling milestone logging compilation
- **rdk_logger_milestone.h:** Header for RDK milestone logging API

## Integration Points

### RDK Logger Contract
- **Function:** `void logMilestone(const char* marker)`
- **Timestamp:** RDK logger adds timestamp automatically
- **Format:** Platform-specific (typically `YYMMDD-HH:MM:SS.microseconds MARKER`)
- **Location:** Writes to `/opt/logs/rdk_milestones.log`

### Common Milestone Markers

| Marker | Description | Logged By |
|--------|-------------|-----------|
| `BOOT_START` | System boot initiated | Init scripts |
| `NETWORK_UP` | Network connectivity established | Network manager |
| `VIDEO_FIRST_FRAME` | First video frame rendered | Video player |
| `APP_LAUNCHED` | Application startup complete | Application code |
| `CHANNEL_CHANGE_START` | Channel change initiated | Tuner service |
| `CHANNEL_CHANGE_COMPLETE` | Channel change finished | Tuner service |
| `DECODER_ACTIVE` | Video decoder activated | Device diagnostics |
| `SYSTEM_READY` | All services initialized | System controller |

### Client Usage Patterns

```cpp
// C++ COM-RPC client - Logging
Exchange::IDeviceDiagnostics* diagnostics = /* obtain interface */;
bool success;
diagnostics->LogMilestone("VIDEO_PLAYBACK_START", success);
```

```cpp
// C++ COM-RPC client - Retrieval
Exchange::IDeviceDiagnostics::IStringIterator* milestones;
bool success;
diagnostics->GetMilestones(milestones, success);

if (success) {
    string entry;
    while (milestones->Next(entry)) {
        printf("%s\n", entry.c_str());
    }
    milestones->Release();
}
```

```bash
# JSON-RPC client via curl - Logging
curl -H 'content-type:text/plain;' \
  --data-binary '{"jsonrpc":"2.0","id":1,"method":"org.rdk.DeviceDiagnostics.logMilestone","params":{"marker":"VIDEO_FIRST_FRAME"}}' \
  http://127.0.0.1:9998/jsonrpc

# JSON-RPC client via curl - Retrieval
curl -H 'content-type:text/plain;' \
  --data-binary '{"jsonrpc":"2.0","id":2,"method":"org.rdk.DeviceDiagnostics.getMilestones"}' \
  http://127.0.0.1:9998/jsonrpc
```

## Use Cases

### UC-1: Boot Time Analysis
**Actor:** System Engineer  
**Goal:** Measure time from power-on to video playback  
**Flow:**
1. Init scripts log `BOOT_START` milestone
2. Each subsystem logs milestone on ready
3. Video player logs `VIDEO_FIRST_FRAME` milestone
4. Engineer retrieves milestones via `getMilestones`
5. Calculate delta between `BOOT_START` and `VIDEO_FIRST_FRAME`

### UC-2: Field Diagnostics
**Actor:** Support Tool  
**Goal:** Retrieve diagnostic timeline from customer device  
**Flow:**
1. Customer reports issue
2. Support tool connects to device via JSON-RPC
3. Calls `getMilestones` to retrieve event history
4. Analyzes timeline for anomalies
5. Identifies root cause (e.g., network delay, decoder failure)

### UC-3: Performance Regression Detection
**Actor:** Automated Test System  
**Goal:** Detect performance degradation in CI/CD pipeline  
**Flow:**
1. Test harness triggers specific operations
2. Each operation logs START/COMPLETE milestones
3. Test retrieves milestones and calculates durations
4. Compares against baseline metrics
5. Fails build if regressions detected

## Error Scenarios

| Scenario | Method | Behavior | Result |
|----------|--------|----------|--------|
| Empty marker | `logMilestone` | Validation failure | `success: false` |
| RDK_LOG_MILESTONE not defined | `logMilestone` | No-op | `success: true` |
| Log file missing | `getMilestones` | File check fails | `success: false`, empty list |
| Log file unreadable | `getMilestones` | Read operation fails | `success: false`, empty list |
| Partial file read | `getMilestones` | Returns partial data | `success: true`, partial list |
| Log file empty | `getMilestones` | Valid empty log | `success: true`, empty list |

## Known Limitations

1. **No Log Rotation:** Plugin does not manage log rotation; external logrotate required
2. **No Timestamp Parsing:** Returns raw log lines; client must parse timestamps
3. **Memory Load:** Entire log file loaded into memory (unbounded growth risk)
4. **No Filtering:** Cannot filter by time range or marker pattern
5. **No Deletion:** No API to clear or archive old milestones
6. **Single Log File:** Does not support multiple log files or log aggregation
7. **No Compression:** Large logs consume significant memory during retrieval

## Security Considerations

- **File Access:** Reads `/opt/logs/rdk_milestones.log` with plugin process privileges
- **Information Disclosure:** Milestones may reveal system behavior patterns
- **No Sanitization:** Marker strings logged verbatim (potential injection if logged to other systems)
- **Read-Only:** Plugin only reads log file; RDK logger writes (separation of concerns)

## Future Enhancements

1. **Streaming Iterator:** Return milestones incrementally without loading entire file
2. **Filtering API:** Filter by timestamp range, marker pattern, or severity
3. **Log Rotation API:** Control log rotation and archival via API
4. **Structured Milestones:** Support JSON metadata with each milestone
5. **Log Aggregation:** Merge milestones from multiple log files
6. **Compression:** On-the-fly decompression for compressed log files
7. **Timestamp Parsing:** Return structured timestamp objects instead of raw strings
8. **Milestone Categories:** Tag milestones with categories (boot, playback, network, etc.)
9. **Persistence Control:** Configure log retention policies via API

## Testing Requirements

### Unit Tests (L1)
- `logMilestone` with valid marker
- `logMilestone` with empty marker (validation failure)
- `logMilestone` with RDK_LOG_MILESTONE disabled (no-op)
- `getMilestones` with existing log file
- `getMilestones` with missing log file
- `getMilestones` with empty log file
- `getMilestones` with file read errors
- Iterator traversal of returned milestones

### Integration Tests (L2)
- End-to-end milestone logging and retrieval
- Large log file handling (10,000+ entries)
- Concurrent reads during active logging
- Log file rotation scenarios
- Permission denied scenarios

### Performance Tests
- Measure `logMilestone` overhead
- Measure `getMilestones` latency vs file size
- Memory usage for large log files

## Operational Considerations

### Log Rotation
Recommended logrotate configuration:
```
/opt/logs/rdk_milestones.log {
    daily
    rotate 7
    compress
    missingok
    notifempty
    copytruncate
}
```

### Monitoring
- Alert if log file size exceeds 10MB (performance degradation risk)
- Alert if log file missing when expected
- Monitor `getMilestones` latency for performance regressions

## References

- [DeviceDiagnostics Plugin API](../../DeviceDiagnostics.md)
- [RDK Logger Documentation](https://wiki.rdkcentral.com/display/RDK/RDK+Logger)
- [IDeviceDiagnostics Interface](https://github.com/rdkcentral/entservices-apis/tree/main/apis/DeviceDiagnostics/IDeviceDiagnostics.h)
- Implementation: `plugin/DeviceDiagnosticsImplementation.cpp::LogMilestone()`, `GetMilestones()`
