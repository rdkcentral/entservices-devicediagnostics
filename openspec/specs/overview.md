# DeviceDiagnostics Plugin - Complete System Specification

## Executive Summary

The DeviceDiagnostics plugin is a Thunder framework service providing diagnostic and monitoring capabilities for RDK-based TV and Set-Top Box devices. It offers four core capabilities: configuration retrieval, milestone logging, AV decoder monitoring, and event notifications.

**Version:** 1.0.0  
**Platform:** RDK TV and STB devices  
**Framework:** Thunder/WPEFramework  
**License:** Apache 2.0

## System Overview

### Component Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      Thunder Framework                           │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              DeviceDiagnostics Plugin                      │ │
│  │                                                            │ │
│  │  ┌──────────────────┐         ┌───────────────────────┐  │ │
│  │  │  JSON-RPC Layer  │         │   COM-RPC Interface   │  │ │
│  │  │  (Port 9998)     │────────▶│   (IDeviceDiagnostics)│  │ │
│  │  │                  │         │                       │  │ │
│  │  │  - GetConfig     │         │   Implementation      │  │ │
│  │  │  - LogMilestone  │         │   Layer               │  │ │
│  │  │  - GetMilestones │         │                       │  │ │
│  │  │  - GetAVStatus   │         │   ┌───────────────┐  │  │ │
│  │  └──────────────────┘         │   │ Background    │  │  │ │
│  │                                │   │ AV Poll       │  │  │ │
│  │  ┌──────────────────┐         │   │ Thread (30s)  │  │ │ │
│  │  │ Event Broadcast  │         │   └───────────────┘  │  │ │
│  │  │ - COM-RPC        │◀────────│                       │  │ │
│  │  │ - JSON-RPC       │         └───────────────────────┘  │ │
│  │  └──────────────────┘                                    │ │
│  └────────────────────────────────────────────────────────────┘ │
└──────────────┬─────────────────┬─────────────────┬─────────────┘
               │                 │                 │
               ▼                 ▼                 ▼
     ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
     │ Config       │  │ File System  │  │ ERM Library  │
     │ Service      │  │ (/opt/logs)  │  │ (Hardware)   │
     │ (Port 10999) │  │              │  │              │
     └──────────────┘  └──────────────┘  └──────────────┘
```

## Core Capabilities

### 1. Configuration Retrieval
**Purpose:** Query device configuration parameters via HTTP backend proxy  
**API:** `getConfiguration(names[])`  
**Spec:** [configuration-retrieval/spec.md](configuration-retrieval/spec.md)

**Key Features:**
- Proxies requests to HTTP service at `localhost:10999`
- Retrieves multiple parameters in single request
- Returns name-value pairs via iterator pattern
- 30-second timeout for backend communication

**Primary Use Cases:**
- Device inventory and discovery
- Feature capability queries
- Platform-specific settings retrieval
- Integration with external systems

### 2. Milestone Logging
**Purpose:** Record and retrieve diagnostic timeline markers  
**API:** `logMilestone(marker)`, `getMilestones()`  
**Spec:** [milestone-logging/spec.md](milestone-logging/spec.md)

**Key Features:**
- Integrates with RDK logger milestone facility
- Persists to `/opt/logs/rdk_milestones.log`
- Timestamped entries for timeline analysis
- Supports boot time and performance tracking

**Primary Use Cases:**
- Boot time measurement and optimization
- Performance regression detection
- Field diagnostics and support
- System lifecycle tracking

### 3. AV Decoder Monitoring
**Purpose:** Track audio/video decoder operational status  
**API:** `getAVDecoderStatus()`, event `onAVDecoderStatusChanged`  
**Spec:** [av-decoder-monitoring/spec.md](av-decoder-monitoring/spec.md)

**Key Features:**
- Real-time decoder state tracking (IDLE/PAUSED/ACTIVE)
- Background polling via ERM library (30s interval)
- Automatic status change notifications
- Multi-decoder aggregation (most active state)

**Primary Use Cases:**
- Resource management and optimization
- Power management decisions
- Playback analytics and telemetry
- Diagnostic monitoring

### 4. Event Notification System
**Purpose:** Publish-subscribe event delivery to clients  
**API:** `INotification` interface, JSON-RPC events  
**Spec:** [event-notification/spec.md](event-notification/spec.md)

**Key Features:**
- Dual-protocol support (COM-RPC + JSON-RPC)
- Asynchronous event dispatch via worker pool
- Multiple concurrent subscribers
- Reference-counted lifecycle management

**Primary Use Cases:**
- System telemetry and analytics
- UI status updates
- Inter-service communication
- Real-time monitoring

## Plugin Lifecycle

### Initialization Sequence

```
1. Constructor
   └─ Create DeviceDiagnostics instance
   └─ Initialize members (_service, _connectionId, etc.)

2. Initialize(IShell* service)
   └─ Store service pointer and AddRef
   └─ Register for RPC connection notifications
   └─ Create out-of-process implementation via Root<>()
      └─ Implementation Constructor
         └─ Create ERM manager (if ENABLE_ERM)
         └─ Spawn AV polling thread (if ENABLE_ERM)
   └─ Register COM-RPC notification callbacks
   └─ Register JSON-RPC auto-generated stubs
   └─ Return empty string (success) or error message

3. Ready State
   └─ Accept JSON-RPC method calls
   └─ Accept COM-RPC interface queries
   └─ Background thread polls decoder status
   └─ Dispatch events to registered clients
```

### Deinitialization Sequence

```
1. Deinitialize(IShell* service)
   └─ Unregister RPC connection notifications
   └─ Unregister COM-RPC notification callbacks
   └─ Unregister JSON-RPC stubs
   └─ Release implementation interface
      └─ Implementation Destructor
         └─ Stop polling thread (m_pollThreadRun = 0)
         └─ Join polling thread
         └─ Destroy ERM manager
   └─ Terminate out-of-process connection
   └─ Release service pointer

2. Destructor
   └─ Final cleanup
   └─ Logging
```

## Data Flow Examples

### Example 1: Configuration Query

```
Client                    Plugin                Backend
  │                         │                      │
  │  getConfiguration()     │                      │
  ├────────────────────────▶│                      │
  │                         │  HTTP POST           │
  │                         ├─────────────────────▶│
  │                         │  (30s timeout)       │
  │                         │                      │
  │                         │  JSON Response       │
  │                         │◀─────────────────────┤
  │                         │  Parse name-values   │
  │  ParamList Iterator     │                      │
  │◀────────────────────────┤                      │
  │  success: true          │                      │
  │                         │                      │
```

### Example 2: Decoder Status Change Event

```
Hardware          Poll Thread       Worker Pool       Clients
  │                   │                   │               │
  │  Decoder starts   │                   │               │
  ├──────────────────▶│                   │               │
  │                   │  Query ERM        │               │
  │                   │  Status changed!  │               │
  │                   │  Submit Job       │               │
  │                   ├──────────────────▶│               │
  │                   │                   │  Dispatch     │
  │                   │                   │  Notify all   │
  │                   │                   ├──────────────▶│
  │                   │                   │  ACTIVE event │
  │                   │  (wait 30s)       │               │
  │                   ▼                   │               │
```

### Example 3: Milestone Logging

```
Client              Plugin             RDK Logger      File System
  │                   │                    │               │
  │  logMilestone()   │                    │               │
  ├──────────────────▶│                    │               │
  │  "VIDEO_START"    │  Validate marker   │               │
  │                   │  logMilestone()    │               │
  │                   ├───────────────────▶│               │
  │                   │                    │  Write log    │
  │                   │                    ├──────────────▶│
  │                   │                    │  /opt/logs/   │
  │  success: true    │                    │  milestones   │
  │◀──────────────────┤                    │               │
  │                   │                    │               │
```

## API Surface

### JSON-RPC Methods (Port 9998)

| Method | Parameters | Returns | Description |
|--------|-----------|---------|-------------|
| `getConfiguration` | `names: string[]` | `ParamList[], success: bool` | Retrieve configuration values |
| `getMilestones` | None | `milestones: string[], success: bool` | Get all milestone entries |
| `logMilestone` | `marker: string` | `success: bool` | Record milestone marker |
| `getAVDecoderStatus` | None | `avDecoderStatus: string` | Get current decoder state |

### JSON-RPC Events

| Event | Parameters | Description |
|-------|-----------|-------------|
| `onAVDecoderStatusChanged` | `avDecoderStatusChange: string` | Decoder status changed |

### COM-RPC Interface (C++)

```cpp
namespace Exchange {
    struct IDeviceDiagnostics : public Core::IUnknown {
        struct INotification : public Core::IUnknown {
            virtual void OnAVDecoderStatusChanged(const string& status) = 0;
        };
        
        virtual Core::hresult Register(INotification* notification) = 0;
        virtual Core::hresult Unregister(INotification* notification) = 0;
        
        virtual Core::hresult GetConfiguration(
            IStringIterator* const& names,
            IDeviceDiagnosticsParamListIterator*& paramList,
            bool& success) = 0;
            
        virtual Core::hresult GetMilestones(
            IStringIterator*& milestones,
            bool& success) = 0;
            
        virtual Core::hresult LogMilestone(
            const string& marker,
            bool& success) = 0;
            
        virtual Core::hresult GetAVDecoderStatus(
            AvDecoderStatusResult& status) = 0;
    };
}
```

## Build Configuration

### Required CMake Options

```cmake
# Enable DeviceDiagnostics plugin
set(PLUGIN_DEVICEDIAGNOSTICS ON)

# Optional: Enable ERM integration for AV decoder monitoring
set(BUILD_ENABLE_ERM ON)  # Default: OFF

# Optional: Custom startup order
set(PLUGIN_DEVICEDIAGNOSTICS_STARTUPORDER 50)  # Default: empty
```

### Compile-Time Flags

| Flag | Effect | Default |
|------|--------|---------|
| `ENABLE_ERM` | Enables AV decoder monitoring via ERM | OFF |
| `RDK_LOG_MILESTONE` | Enables milestone logging via RDK logger | Platform-specific |

### Dependencies

**Build Dependencies:**
- Thunder Plugins package
- Thunder Definitions package
- libcurl (with development headers)
- essosrmgr library (if ENABLE_ERM)
- RDK logger (if RDK_LOG_MILESTONE)

**Runtime Dependencies:**
- Thunder framework (WPEFramework)
- Configuration service at `localhost:10999`
- `/opt/logs/` directory (write permissions)
- ERM library (if ENABLE_ERM enabled)

## Plugin Configuration

### Configuration File: `DeviceDiagnostics.config`

```
set (autostart false)
set (preconditions Platform)
set (callsign "org.rdk.DeviceDiagnostics")
set (startuporder ${PLUGIN_DEVICEDIAGNOSTICS_STARTUPORDER})

map()
    key(root)
    map()
        kv(mode ${PLUGIN_DEVICEDIAGNOSTICS_MODE})
        kv(locator lib${PLUGIN_IMPLEMENTATION}.so)
    end()
end()
ans(configuration)
```

**Key Settings:**
- `autostart`: false (manual activation required)
- `preconditions`: Platform subsystem must be active
- `callsign`: `org.rdk.DeviceDiagnostics` (JSON-RPC namespace)
- `mode`: `Off` (in-process) or `Local`/`Container` (out-of-process)

### Runtime Configuration

```json
{
  "callsign": "org.rdk.DeviceDiagnostics",
  "autostart": false,
  "configuration": {
    "root": {
      "mode": "Off"
    }
  }
}
```

## System Integration

### Thunder Controller Integration

```bash
# Activate plugin
curl -d '{"jsonrpc":"2.0","id":1,"method":"Controller.1.activate","params":{"callsign":"org.rdk.DeviceDiagnostics"}}' \
  http://127.0.0.1:9998/jsonrpc

# Check plugin status  
curl -d '{"jsonrpc":"2.0","id":2,"method":"Controller.1.status"}' \
  http://127.0.0.1:9998/jsonrpc

# Deactivate plugin
curl -d '{"jsonrpc":"2.0","id":3,"method":"Controller.1.deactivate","params":{"callsign":"org.rdk.DeviceDiagnostics"}}' \
  http://127.0.0.1:9998/jsonrpc
```

### Integration with Other Plugins

**Telemetry Plugin:**
- Subscribes to `onAVDecoderStatusChanged` events
- Tracks playback statistics and usage patterns
- Forwards metrics to cloud analytics

**System Services Plugin:**
- Uses `getConfiguration` to query device capabilities
- Calls `logMilestone` during system state transitions
- Monitors decoder status for health checks

**UI Applications:**
- Subscribe to decoder events for UI updates
- Query milestones for boot time display
- Use configuration data for feature enablement

## Performance Characteristics

### Throughput Metrics

| Operation | Typical Latency | Max Throughput |
|-----------|----------------|----------------|
| `getConfiguration` | 50-500ms (backend-dependent) | Limited by backend |
| `getMilestones` | 10-100ms (file size-dependent) | 10-100 req/s |
| `logMilestone` | < 1ms (async) | 1000+ req/s |
| `getAVDecoderStatus` | < 1ms | 1000+ req/s |
| Event dispatch | < 10ms | 100+ events/s |

### Resource Usage

| Resource | Typical | Peak | Notes |
|----------|---------|------|-------|
| Memory | 2-5 MB | 10 MB | With 100 event subscribers |
| CPU | < 1% | 5% | Mostly idle, spikes on events |
| Threads | 2 | 2 | Main + AV poll thread |
| File Handles | 1-2 | 5 | Log file + curl sockets |
| Network | Minimal | 10 KB/s | Configuration requests |

### Scalability Limits

- **Event Subscribers:** Tested up to 100 concurrent subscribers
- **Configuration Parameters:** Tested up to 100 parameters per request
- **Milestone Log Size:** Performance degrades beyond 1000 entries
- **Event Rate:** Can handle 100+ events/second sustained

## Error Handling

### Plugin-Level Errors

| Error Condition | Behavior | Recovery |
|----------------|----------|----------|
| Implementation creation fails | Initialize() returns error message | Plugin remains deactivated |
| Out-of-process crashes | Deactivate notification triggered | Thunder restarts plugin |
| Worker pool exhaustion | Events queued/delayed | Self-recovers when load decreases |

### API-Level Errors

| Error Condition | API Behavior | Client Impact |
|----------------|--------------|---------------|
| Backend unavailable | `success: false`, empty results | Graceful degradation |
| File not found | `success: false`, empty results | Retry or skip |
| Invalid parameters | `success: false` | Correct parameters and retry |
| ERM not available | Returns `IDLE` always | Limited functionality |

## Security Considerations

### Threat Model

**Trust Boundaries:**
- Thunder framework (trusted)
- Plugin implementation (trusted)
- Backend services (trusted - localhost only)
- Client plugins/apps (semi-trusted - same device)

**Attack Surface:**
- JSON-RPC API (localhost port 9998)
- Configuration backend (localhost port 10999)
- File system access (`/opt/logs/`)
- ERM library integration

### Security Measures

1. **Localhost-Only Communications:** All network services bound to `127.0.0.1`
2. **No External Access:** No internet-facing endpoints
3. **Process Isolation:** Can run out-of-process for containment
4. **File Permissions:** Relies on OS-level permissions for log access
5. **No Authentication Required:** All clients on device trusted equally

### Known Security Limitations

1. **No Input Validation:** Configuration parameter names not sanitized
2. **No Rate Limiting:** Malicious client can flood requests
3. **Log Injection:** Milestone markers written verbatim to logs
4. **No Access Control:** Any client can call any API
5. **Memory Exhaustion:** Unbounded event subscriber list

## Testing Strategy

### Test Levels

**L1 - Unit Tests:**
- Individual method functionality
- Error handling paths
- Edge cases and boundary conditions
- Mock external dependencies (ERM, files, HTTP)

**L2 - Integration Tests:**
- End-to-end API workflows
- Inter-component interactions
- Real backend/file system integration
- Performance and stress testing

**Hardware Tests:**
- Platform-specific ERM integration
- Real video playback scenarios
- Multi-decoder configurations

### Test Coverage Goals

- Code Coverage: > 80%
- Branch Coverage: > 70%
- API Coverage: 100% of public methods
- Error Path Coverage: > 60%

## Operational Considerations

### Monitoring & Observability

**Key Metrics:**
- Plugin activation/deactivation events
- API call rates and latencies
- Event dispatch rates
- Error rates by API method
- Backend timeout frequency
- File access failures

**Logging:**
- All errors logged with context
- Configuration changes logged
- State transitions logged (ACTIVATED/DEACTIVATED)
- Performance warnings (slow operations)

### Maintenance

**Log Rotation:**
```bash
# Recommended cron job for milestone logs
0 0 * * * /usr/sbin/logrotate /etc/logrotate.d/rdk-milestones
```

**Health Checks:**
```bash
# Verify plugin is active
curl -d '{"jsonrpc":"2.0","id":1,"method":"Controller.1.status"}' \
  http://127.0.0.1:9998/jsonrpc | grep -q "org.rdk.DeviceDiagnostics.*activated"

# Test basic functionality
curl -d '{"jsonrpc":"2.0","id":2,"method":"org.rdk.DeviceDiagnostics.getAVDecoderStatus"}' \
  http://127.0.0.1:9998/jsonrpc
```

### Troubleshooting

**Common Issues:**

| Symptom | Cause | Solution |
|---------|-------|----------|
| Plugin won't activate | Precondition not met | Check Platform subsystem active |
| getConfiguration fails | Backend service down | Verify service on port 10999 |
| getMilestones returns empty | Log file missing | Check `/opt/logs/rdk_milestones.log` |
| Status always IDLE | ERM not enabled | Rebuild with BUILD_ENABLE_ERM=ON |
| Events not received | Not subscribed | Verify registration/subscription |

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | Current | Initial release with four core capabilities |

## Future Roadmap

### Planned Enhancements

**Short Term (Next Release):**
1. Configurable polling interval for AV monitoring
2. Structured JSON parsing for configuration responses
3. Event filtering and subscription management
4. Milestone log rotation via API

**Medium Term:**
5. Caching layer for frequently-accessed configuration
6. Asynchronous backend communication
7. Multi-decoder status details (not just aggregate)
8. Historical data tracking and trending

**Long Term:**
9. Plugin resource usage diagnostics
10. Advanced telemetry integration
11. Cloud-based diagnostic reporting
12. ML-based anomaly detection

## References

### Specifications
- [Configuration Retrieval Spec](configuration-retrieval/spec.md)
- [Milestone Logging Spec](milestone-logging/spec.md)
- [AV Decoder Monitoring Spec](av-decoder-monitoring/spec.md)
- [Event Notification Spec](event-notification/spec.md)

### External Documentation
- [Thunder Framework](https://rdkcentral.github.io/Thunder/)
- [RDK Central](https://rdkcentral.com)
- [Thunder Interfaces](https://github.com/rdkcentral/ThunderInterfaces)
- [API Definition](../../DeviceDiagnostics.md)

### Source Code
- Plugin Implementation: `plugin/`
- Helper Utilities: `helpers/`
- Build Configuration: `CMakeLists.txt`, `plugin/CMakeLists.txt`
- Tests: `Tests/L1Tests/`, `Tests/L2Tests/`

### Community
- GitHub Issues: https://github.com/rdkcentral/entservices-devicediagnostics/issues
- RDK Wiki: https://wiki.rdkcentral.com
- Developer Support: RDK Slack/Forums

---

**Document Status:** Active  
**Last Updated:** 2026-04-10  
**Maintainer:** RDK DeviceDiagnostics Team
