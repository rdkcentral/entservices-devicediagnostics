# AV Decoder Status Monitoring Specification

## Overview

The AV Decoder Status Monitoring capability tracks the operational state of audio/video decoders on RDK devices. It provides real-time visibility into decoder activity and publishes state change notifications for system monitoring and diagnostics.

**Target Devices:** RDK TV and STB platforms with hardware video decoders  
**Version:** 1.0.0  
**Status:** Active

## Purpose

Enables system components to monitor video playback pipeline health by tracking decoder states. Critical for:
- Performance monitoring and analytics
- Resource management and optimization
- Debugging playback issues
- Power management decisions
- User experience tracking

## API Contract

### Method: `getAVDecoderStatus`

Retrieves the current most active status of all audio/video decoders.

**Request:**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "org.rdk.DeviceDiagnostics.getAVDecoderStatus"
}
```

**Response:**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "avDecoderStatus": "ACTIVE"
  }
}
```

**Valid Status Values:**
- `"IDLE"` - No decoder activity
- `"PAUSED"` - Decoder paused/buffering
- `"ACTIVE"` - Decoder actively processing media

### Event: `onAVDecoderStatusChanged`

Notification sent when the most active decoder status changes.

**Event Payload:**
```json
{
  "jsonrpc": "2.0",
  "method": "org.rdk.DeviceDiagnostics.onAVDecoderStatusChanged",
  "params": {
    "avDecoderStatusChange": "ACTIVE"
  }
}
```

## Functional Requirements

### FR-1: Decoder Status Query

#### FR-1.1: Status Retrieval
- **Requirement:** MUST return the most active status among all decoders
- **Priority Order:** ACTIVE > PAUSED > IDLE
- **Rationale:** If any decoder is active, system is considered active (most significant state)

#### FR-1.2: ERM Integration (ENABLE_ERM defined)
- **Requirement:** When `ENABLE_ERM` is defined, MUST query ERM library via `EssRMgrGetAVState()`
- **Implementation:** Calls `getMostActiveDecoderStatus()` which wraps ERM API
- **Thread Safety:** Protects ERM call with `m_AVDecoderStatusLock` mutex

#### FR-1.3: Fallback Behavior (ENABLE_ERM not defined)
- **Requirement:** When `ENABLE_ERM` is NOT defined, MUST always return `"IDLE"`
- **Rationale:** Graceful degradation on platforms without ERM support
- **Warning:** Logs warning during initialization: "ENABLE_ERM is not defined, decoder status will always be reported as IDLE"

### FR-2: Automatic Status Monitoring

#### FR-2.1: Background Polling Thread
- **Requirement:** When `ENABLE_ERM` is defined, MUST spawn dedicated polling thread on plugin initialization
- **Implementation:** `AVPollThread` static method executed in separate thread
- **Lifecycle:** Thread runs until plugin destruction

#### FR-2.2: Polling Interval
- **Requirement:** MUST poll ERM library every 30 seconds
- **Configuration:** `AVDECODERSTATUS_RETRY_INTERVAL` (hardcoded to 30 seconds)
- **Mechanism:** Condition variable wait with timeout

#### FR-2.3: Change Detection
- **Requirement:** MUST compare current status with last known status
- **Behavior:** Only trigger events when status actually changes
- **Optimization:** Avoids spurious event notifications

#### FR-2.4: Event Dispatch
- **Requirement:** On status change, MUST dispatch `onAVDecoderStatusChanged` event
- **Implementation:** Creates JsonObject with new status and calls `dispatchEvent()`
- **Async Execution:** Uses Thunder's worker pool via `Job::Create()`

#### FR-2.5: Thread Termination
- **Requirement:** Poll thread MUST gracefully terminate on plugin deinitialization
- **Mechanism:** Sets `m_pollThreadRun` to 0 and signals condition variable
- **Cleanup:** Waits for thread join and destroys ERM manager

### FR-3: ERM Library Integration

#### FR-3.1: Initialization
- **Requirement:** MUST create ERM manager via `EssRMgrCreate()` in constructor
- **Error Handling:** Logs error and returns if ERM creation fails
- **Resource Management:** Stores ERM handle in `m_EssRMgr` member

#### FR-3.2: Status Mapping
- **Requirement:** MUST map ERM status codes to string values
- **Mapping:**
  - `EssRMgrRes_idle` (0) → `"IDLE"`
  - `EssRMgrRes_paused` (1) → `"PAUSED"`
  - `EssRMgrRes_active` (2) → `"ACTIVE"`
- **Implementation:** `decoderStatusStr` array indexed by ERM status

#### FR-3.3: Cleanup
- **Requirement:** MUST destroy ERM manager via `EssRMgrDestroy()` in destructor
- **Order:** Destroy ERM after thread terminates

## Non-Functional Requirements

### NFR-1: Performance
- **Polling Overhead:** < 10ms per poll operation
- **Event Latency:** < 100ms from status change to event dispatch
- **Memory Footprint:** < 100KB for polling thread and ERM integration

### NFR-2: Reliability
- **Thread Safety:** All ERM access protected by mutex
- **Graceful Degradation:** Returns IDLE on platforms without ERM
- **Error Recovery:** ERM initialization failure does not crash plugin

### NFR-3: Resource Management
- **Thread Lifecycle:** Polling thread cleanly joined on shutdown
- **Mutex Handling:** Proper lock/unlock with RAII via unique_lock
- **Memory Leaks:** No leaked ERM resources or thread handles

## Dependencies

### Runtime Dependencies
- **ERM Library (essosrmgr):** When `ENABLE_ERM` is defined
  - Provides `EssRMgrCreate()`, `EssRMgrDestroy()`, `EssRMgrGetAVState()`
  - Tracks hardware decoder state across system
- **Thunder Framework:** Worker pool for async event dispatch
- **POSIX Threads:** std::thread, std::mutex, std::condition_variable

### Compile-Time Dependencies
- **ENABLE_ERM:** Build flag enabling ERM integration
- **essos-resmgr.h:** ERM library header
- **CMake:** Links against `essosrmgr` library when `BUILD_ENABLE_ERM` is set

## Architecture

### Threading Model

```
┌─────────────────────────────────────────┐
│         Main Thread                     │
│  (Thunder JSON-RPC Dispatcher)          │
│                                         │
│  getAVDecoderStatus() ──┐               │
│                         │               │
│                         ▼               │
│                  Lock & Query ERM       │
│                         │               │
│                         ▼               │
│                  Return Status          │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│      AV Poll Thread                     │
│                                         │
│  Loop:                                  │
│    1. Wait 30s (or stop signal)         │
│    2. Lock & Query ERM                  │
│    3. Compare with last status          │
│    4. If changed:                       │
│       - Create JsonObject               │
│       - Dispatch to Worker Pool         │
└─────────────────────────────────────────┘
                    │
                    ▼
┌─────────────────────────────────────────┐
│      Thunder Worker Pool                │
│                                         │
│  Job::Dispatch():                       │
│    1. Lock notification list            │
│    2. Notify all COM-RPC subscribers    │
│    3. Send JSON-RPC event               │
└─────────────────────────────────────────┘
```

### State Diagram

```
     ┌──────┐      Video Playback       ┌────────┐
     │ IDLE │ ─────────Start──────────→ │ ACTIVE │
     └──────┘                            └────────┘
        ▲                                    │
        │                                    │
        │                               Pause/Buffer
        │                                    │
        │                                    ▼
        │         Resume              ┌────────┐
        └────── Playback ─────────────│ PAUSED │
                                      └────────┘
                Stop Playback               │
        ┌───────────────────────────────────┘
        │
        ▼
     ┌──────┐
     │ IDLE │
     └──────┘
```

## Integration Points

### ERM Library Contract
- **Header:** `essos-resmgr.h`
- **Library:** `libessosrmgr.so`
- **Functions:**
  - `EssRMgr* EssRMgrCreate()` - Initialize resource manager
  - `void EssRMgrDestroy(EssRMgr*)` - Cleanup resource manager
  - `int EssRMgrGetAVState(EssRMgr*, int* state)` - Query decoder state

### Client Usage Patterns

```cpp
// C++ COM-RPC client - Query status
Exchange::IDeviceDiagnostics* diagnostics = /* obtain interface */;
Exchange::IDeviceDiagnostics::AvDecoderStatusResult result;
diagnostics->GetAVDecoderStatus(result);
printf("Status: %s\n", result.avDecoderStatus.c_str());
```

```cpp
// C++ COM-RPC client - Subscribe to events
class MyNotification : public Exchange::IDeviceDiagnostics::INotification {
    void OnAVDecoderStatusChanged(const string& status) override {
        printf("Decoder status changed to: %s\n", status.c_str());
    }
};

auto notification = Core::Service<MyNotification>::Create<INotification>();
diagnostics->Register(notification);
```

```bash
# JSON-RPC client - Query status
curl -H 'content-type:text/plain;' \
  --data-binary '{"jsonrpc":"2.0","id":1,"method":"org.rdk.DeviceDiagnostics.getAVDecoderStatus"}' \
  http://127.0.0.1:9998/jsonrpc
```

```javascript
// JavaScript client - Subscribe to events (Thunder JS)
thunderJS.DeviceDiagnostics.on('onAVDecoderStatusChanged', (status) => {
    console.log('Decoder status:', status.avDecoderStatusChange);
});
```

## Use Cases

### UC-1: Resource Management
**Actor:** System Resource Manager  
**Goal:** Release resources when video playback inactive  
**Flow:**
1. Subscribe to `onAVDecoderStatusChanged` events
2. Receive `ACTIVE` when user starts video playback
3. Allocate/prioritize resources for video pipeline
4. Receive `IDLE` when playback stops
5. Release resources for other applications

### UC-2: Power Management
**Actor:** Power Manager Service  
**Goal:** Optimize power consumption based on decoder activity  
**Flow:**
1. Query `getAVDecoderStatus` periodically
2. When status is `IDLE` for extended period
3. Transition to low-power mode
4. On `ACTIVE` event, return to full-power mode

### UC-3: Analytics & Telemetry
**Actor:** Telemetry Service  
**Goal:** Track video playback usage patterns  
**Flow:**
1. Subscribe to status change events
2. Log timestamp on `ACTIVE` (playback start)
3. Log timestamp on `IDLE` (playback stop)
4. Calculate playback duration and usage metrics
5. Report to analytics backend

### UC-4: Diagnostic Monitoring
**Actor:** System Health Monitor  
**Goal:** Detect decoder stuck states  
**Flow:**
1. Monitor status change events
2. Set watchdog timer on `ACTIVE` state
3. If status remains `PAUSED` > 60s, log anomaly
4. Trigger diagnostic report or recovery action

## Error Scenarios

| Scenario | Behavior | Result |
|----------|----------|--------|
| ERM initialization fails | Logs error, no polling thread | Status always `IDLE` |
| ENABLE_ERM not defined | No ERM integration | Status always `IDLE`, warning logged |
| ERM query fails | Returns last known status | Silent failure (no explicit error) |
| Poll thread fails to start | No automatic monitoring | Manual `getAVDecoderStatus` still works |
| Mutex deadlock | Thread blocked | Timeout mechanism via condition variable |

## Known Limitations

1. **Polling Latency:** Up to 30 seconds delay in status change detection
2. **Single Status:** Reports only "most active" decoder (multi-decoder details lost)
3. **No Decoder Enumeration:** Cannot query individual decoder states
4. **No Historical Data:** No built-in status history or trend tracking
5. **ERM Dependency:** Requires platform-specific ERM library integration
6. **Fixed Polling Interval:** 30-second interval is hardcoded, not configurable
7. **No Event Filtering:** All status changes generate events (no threshold/hysteresis)

## Security Considerations

- **Hardware Access:** ERM library may have direct hardware access
- **Resource Enumeration:** Status reveals system resource utilization patterns
- **Event Privacy:** Status changes may correlate with user viewing behavior
- **Local Access Only:** No remote status query authentication required

## Future Enhancements

1. **Configurable Polling Interval:** Allow tuning via plugin configuration
2. **Multi-Decoder Details:** Report individual decoder states, not just aggregate
3. **Event-Driven ERM:** Replace polling with ERM callback registration (if supported)
4. **Status History:** Maintain rolling buffer of recent status changes
5. **Threshold Filtering:** Only generate events if state persists > N seconds
6. **Performance Metrics:** Track decoder utilization percentage, not just state
7. **Decoder Identification:** Tag status with decoder type (video/audio/subtitle)
8. **Buffering Metrics:** Report buffer health during PAUSED state

## Testing Requirements

### Unit Tests (L1)
- Status mapping (ERM codes to strings)
- Status comparison logic (change detection)
- Thread creation and termination
- Mutex locking correctness
- ERM initialization failure handling
- ENABLE_ERM disabled behavior (always IDLE)

### Integration Tests (L2)
- End-to-end status query
- Event subscription and notification delivery
- Polling thread lifecycle across plugin init/deinit
- Multiple concurrent status queries
- Status change during active polling
- Mock ERM library with state transitions

### Performance Tests
- Measure polling overhead
- Event dispatch latency
- Memory footprint of polling thread
- Mutex contention under load

### Hardware Tests (Platform-Specific)
- Verify status matches actual decoder state
- Test with real video playback (IDLE → ACTIVE → IDLE)
- Multi-decoder scenarios (if applicable)
- Stress testing with rapid state changes

## Platform Considerations

### ERM Availability
- **RDK TV:** ERM typically available
- **RDK STB:** ERM availability varies by SoC vendor
- **Development Builds:** May not have ERM library

### Build Variants
```cmake
# Enable ERM integration
set(BUILD_ENABLE_ERM ON)
```

### Runtime Behavior by Platform

| Platform | ENABLE_ERM | Behavior |
|----------|-----------|----------|
| Production STB | Yes | Full decoder monitoring |
| Development VM | No | Always returns IDLE |
| Legacy STB | No | Always returns IDLE |
| Smart TV | Yes | Full decoder monitoring |

## Operational Considerations

### Monitoring
- Alert if status stuck in `PAUSED` > 5 minutes (possible hang)
- Track ACTIVE→IDLE transitions per day (usage metric)
- Monitor polling thread CPU usage (should be negligible)

### Debugging
- Check `m_pollThreadRun` flag if events not received
- Verify ERM library loaded: `ldd libWPEFrameworkDeviceDiagnosticsImplementation.so`
- Enable DEBUG logging to see ERM query results

## References

- [DeviceDiagnostics Plugin API](../../DeviceDiagnostics.md)
- [ERM Library Documentation](https://github.com/rdkcentral/Essos)
- [IDeviceDiagnostics Interface](https://github.com/rdkcentral/entservices-apis/tree/main/apis/DeviceDiagnostics/IDeviceDiagnostics.h)
- Implementation: `plugin/DeviceDiagnosticsImplementation.cpp::GetAVDecoderStatus()`, `AVPollThread()`
