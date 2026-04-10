# Event Notification System Specification

## Overview

The Event Notification System provides a publish-subscribe mechanism for real-time delivery of diagnostic events from the DeviceDiagnostics plugin to registered clients. Supports both COM-RPC (C++) and JSON-RPC (language-agnostic) notification patterns.

**Target Devices:** RDK TV and STB platforms  
**Version:** 1.0.0  
**Status:** Active

## Purpose

Enables reactive monitoring by allowing clients to subscribe to diagnostic events without polling. Provides decoupled, asynchronous communication between the plugin and system components.

## Architecture

### Dual-Protocol Support

```
┌────────────────────────────────────────────────┐
│     DeviceDiagnostics Plugin                   │
│                                                │
│   Event Generation (e.g., decoder change)      │
│             │                                  │
│             ▼                                  │
│   ┌─ dispatchEvent() ──────────────────┐      │
│   │                                     │      │
│   │  Submit Job to Worker Pool          │      │
│   └─────────────────────────────────────┘      │
│             │                                  │
│             ▼                                  │
│   ┌─ Job::Dispatch() ─────────────────┐       │
│   │                                    │       │
│   │  Lock notification list            │       │
│   │  Iterate registered subscribers    │       │
│   │                                    │       │
│   │  ┌──────────────┬─────────────┐   │       │
│   │  │              │             │   │       │
│   │  ▼              ▼             ▼   │       │
│   │ COM-RPC      COM-RPC      JSON-RPC│       │
│   │ Client 1     Client 2     Broadcast│      │
│   │ (INotify)    (INotify)    (Auto)   │      │
│   └────────────────────────────────────┘      │
└────────────────────────────────────────────────┘
```

## Functional Requirements

### FR-1: Notification Registration (COM-RPC)

#### FR-1.1: Interface Contract
- **Requirement:** MUST implement `Exchange::IDeviceDiagnostics::INotification` interface
- **Methods Required:**
  ```cpp
  void OnAVDecoderStatusChanged(const string& status);
  ```

#### FR-1.2: Registration Method
- **Signature:** `Core::hresult Register(INotification* notification)`
- **Requirement:** MUST add notification callback to internal list
- **Duplicate Prevention:** MUST NOT register same notification pointer twice
- **Reference Counting:** MUST call `AddRef()` on registered notification
- **Thread Safety:** Protected by `_adminLock` critical section

#### FR-1.3: Unregistration Method
- **Signature:** `Core::hresult Unregister(INotification* notification)`
- **Requirement:** MUST remove notification callback from internal list
- **Single Removal:** Removes only first matching notification (if duplicates somehow exist)
- **Reference Counting:** MUST call `Release()` on unregistered notification
- **Error Handling:** Returns `Core::ERROR_GENERAL` if notification not found
- **Thread Safety:** Protected by `_adminLock` critical section

### FR-2: Event Dispatch

#### FR-2.1: Asynchronous Dispatch
- **Requirement:** Events MUST be dispatched asynchronously via Thunder's worker pool
- **Implementation:** `dispatchEvent()` submits `Job` to `Core::IWorkerPool::Instance()`
- **Rationale:** Prevents blocking event generation thread

#### FR-2.2: Job Pattern
- **Requirement:** MUST use Thunder's `Core::IDispatch` job pattern
- **Lifecycle:** Job holds reference to plugin implementation (AddRef/Release)
- **Thread Safety:** Job execution protected by plugin's `_adminLock`

#### FR-2.3: Notification Delivery
- **Requirement:** MUST invoke callback on ALL registered COM-RPC notifications
- **Algorithm:** Iterate `_deviceDiagnosticsNotification` list and call interface method
- **Error Handling:** Individual notification failures do not halt iteration
- **Lock Duration:** Entire notification delivery happens under lock (potential bottleneck)

### FR-3: JSON-RPC Event Broadcasting

#### FR-3.1: Automatic Registration
- **Requirement:** JSON-RPC events MUST be automatically registered via `Exchange::JDeviceDiagnostics::Register()`
- **Configuration:** No explicit subscription required (broadcast to all JSON-RPC clients)
- **Lifecycle:** Registered during `Initialize()`, unregistered during `Deinitialize()`

#### FR-3.2: Event Generation
- **Trigger:** Internal notification class forwards COM-RPC events to JSON-RPC
- **Method:** Calls `Exchange::JDeviceDiagnostics::Event::OnAVDecoderStatusChanged()`
- **Autogeneration:** Event dispatch code auto-generated from Thunder interface definition

### FR-4: Supported Events

#### FR-4.1: OnAVDecoderStatusChanged
- **Event Type:** `ON_AVDECODER_STATUSCHANGED` (enum value)
- **Payload:** JsonObject with `avDecoderStatusChange` key
- **Values:** `"IDLE"`, `"PAUSED"`, `"ACTIVE"`
- **Frequency:** Only on actual status changes (not periodic)

## Non-Functional Requirements

### NFR-1: Performance
- **Registration/Unregistration:** < 1ms (simple list operation)
- **Event Dispatch Latency:** < 10ms from generation to delivery (worker pool overhead)
- **Throughput:** Support 100+ events/second without degradation

### NFR-2: Reliability
- **No Event Loss:** All registered clients receive all events (synchronous broadcast)
- **Client Isolation:** Failure in one client does not affect others (caught exceptions)
- **Ordered Delivery:** Events delivered in generation order

### NFR-3: Resource Management
- **Memory:** O(n) memory per registered client (notification pointer + refcount)
- **Thread Safety:** All operations protected by critical sections
- **Cleanup:** Automatic reference counting prevents leaks

## Implementation Details

### Notification List Management

```cpp
// Member variable
std::list<Exchange::IDeviceDiagnostics::INotification*> _deviceDiagnosticsNotification;

// Registration
_adminLock.Lock();
if (std::find(_deviceDiagnosticsNotification.begin(), 
              _deviceDiagnosticsNotification.end(), 
              notification) == _deviceDiagnosticsNotification.end()) {
    _deviceDiagnosticsNotification.push_back(notification);
    notification->AddRef();
}
_adminLock.Unlock();
```

### Event Dispatch Flow

```cpp
// Step 1: Generate event from background thread
void onDecoderStatusChange(int status) {
    JsonObject params;
    params["avDecoderStatusChange"] = decoderStatusStr[status];
    dispatchEvent(ON_AVDECODER_STATUSCHANGED, params);
}

// Step 2: Submit to worker pool
void dispatchEvent(Event event, const JsonValue &params) {
    Core::IWorkerPool::Instance().Submit(Job::Create(this, event, params));
}

// Step 3: Worker thread executes job
void Dispatch(Event event, const JsonValue params) {
    _adminLock.Lock();
    auto index = _deviceDiagnosticsNotification.begin();
    while (index != _deviceDiagnosticsNotification.end()) {
        (*index)->OnAVDecoderStatusChanged(params.String());
        ++index;
    }
    _adminLock.Unlock();
}
```

### JSON-RPC Integration

```cpp
// Internal notification class forwards to JSON-RPC
class Notification : public Exchange::IDeviceDiagnostics::INotification {
    void OnAVDecoderStatusChanged(const string& status) override {
        LOGINFO("OnAVDecoderStatusChanged: %s", status.c_str());
        Exchange::JDeviceDiagnostics::Event::OnAVDecoderStatusChanged(_parent, status);
    }
};
```

## Client Integration Patterns

### COM-RPC Client (C++)

```cpp
#include <interfaces/IDeviceDiagnostics.h>

class DiagnosticsListener : public Exchange::IDeviceDiagnostics::INotification {
public:
    DiagnosticsListener() = default;
    virtual ~DiagnosticsListener() = default;

    BEGIN_INTERFACE_MAP(DiagnosticsListener)
        INTERFACE_ENTRY(Exchange::IDeviceDiagnostics::INotification)
    END_INTERFACE_MAP

    void OnAVDecoderStatusChanged(const string& status) override {
        printf("Decoder status changed to: %s\n", status.c_str());
        // Handle status change
        if (status == "ACTIVE") {
            // Resume monitoring
        } else if (status == "IDLE") {
            // Release resources
        }
    }
};

// Usage
auto diagnostics = /* obtain IDeviceDiagnostics interface */;
auto listener = Core::Service<DiagnosticsListener>::Create<INotification>();
diagnostics->Register(listener);

// Later: cleanup
diagnostics->Unregister(listener);
listener->Release();
```

### JSON-RPC Client (curl)

```bash
# Subscribe to events (Thunder client-side implementation varies)
# Typically requires WebSocket connection for asynchronous events

# Example using Thunder Controller plugin to monitor events
wscat -c ws://127.0.0.1:9998/jsonrpc

# Send subscription request
{"jsonrpc":"2.0","id":1,"method":"client.events.1.register","params":{"event":"onAVDecoderStatusChanged","id":"org.rdk.DeviceDiagnostics"}}

# Receive events
{"jsonrpc":"2.0","method":"org.rdk.DeviceDiagnostics.onAVDecoderStatusChanged","params":{"avDecoderStatusChange":"ACTIVE"}}
```

### JSON-RPC Client (JavaScript/Thunder.js)

```javascript
import ThunderJS from 'ThunderJS';

const thunder = ThunderJS({
    host: '127.0.0.1',
    port: 9998
});

// Subscribe to events
thunder.DeviceDiagnostics.on('onAVDecoderStatusChanged', (data) => {
    console.log('Decoder status changed:', data.avDecoderStatusChange);
    
    if (data.avDecoderStatusChange === 'ACTIVE') {
        // Video playback starting
        updateUI('Playing');
    } else if (data.avDecoderStatusChange === 'IDLE') {
        // Video playback stopped
        updateUI('Stopped');
    }
});
```

## Use Cases

### UC-1: System Telemetry
**Actor:** Telemetry Service  
**Goal:** Record all diagnostic events for analytics  
**Flow:**
1. Telemetry service registers COM-RPC notification on boot
2. Receives all decoder status change events
3. Timestamps and logs to telemetry backend
4. Maintains registration until service shutdown

### UC-2: UI Status Display
**Actor:** User Interface Application  
**Goal:** Display real-time playback status indicator  
**Flow:**
1. UI subscribes to events via JSON-RPC/WebSocket
2. Receives `onAVDecoderStatusChanged` events
3. Updates status icon (idle/buffering/playing)
4. Unsubscribes when UI closed

### UC-3: Power Manager
**Actor:** Power Management Service  
**Goal:** Adjust power state based on decoder activity  
**Flow:**
1. Power manager registers COM-RPC notification
2. On ACTIVE event: prevent screen dim/sleep
3. On IDLE event: re-enable power saving after timeout
4. Maintains subscription throughout system lifecycle

## Error Scenarios

| Scenario | Behavior | Result |
|----------|----------|--------|
| Register same notification twice | Second registration rejected | No duplicate, logged warning |
| Unregister non-registered notification | Not found error | Returns `ERROR_GENERAL`, logged error |
| Client crashes without unregister | Orphaned pointer in list | **CRITICAL: Potential crash on next event** |
| Event dispatch during shutdown | List locked, may block | Events dropped if plugin deactivating |
| Client callback throws exception | Exception propagates | **RISK: May crash worker thread** |

## Known Limitations

1. **No Automatic Cleanup:** Crashed clients leave orphaned pointers (crash risk)
2. **Blocking Dispatch:** All clients notified under lock (slow client blocks all)
3. **No Event Filtering:** Clients receive all events (cannot filter by type)
4. **No Priority:** All clients treated equally (no prioritized delivery)
5. **No Queuing:** Events dispatched immediately (no buffering/batching)
6. **No History:** New subscribers don't receive current state
7. **Single Event Type:** Only decoder status change events supported

## Security Considerations

- **No Access Control:** Any client with plugin interface can register
- **Resource Exhaustion:** Malicious clients can register unlimited notifications
- **Callback Safety:** Plugin trusts client callbacks (no sandboxing)
- **Reference Leak:** Client must properly unregister to avoid leak

## Future Enhancements

1. **Automatic Cleanup:** Detect disconnected clients and auto-unregister
2. **Event Filtering:** Allow clients to specify which events they want
3. **Priority Levels:** Support high-priority clients (notify first)
4. **Current State:** Return current state to new subscribers immediately
5. **Event Queuing:** Buffer events during high-load scenarios
6. **Rate Limiting:** Throttle notifications to prevent overload
7. **Weak References:** Use weak pointers to avoid orphaned notifications
8. **Exception Handling:** Catch and log client callback exceptions
9. **Event History:** Maintain recent event log for new subscribers
10. **Per-Client Threading:** Isolate slow clients from affecting others

## Testing Requirements

### Unit Tests (L1)
- Register/unregister notification lifecycle
- Duplicate registration prevention
- Multiple concurrent registrations
- Event dispatch to single subscriber
- Event dispatch to multiple subscribers
- Reference counting correctness
- Thread safety under concurrent operations

### Integration Tests (L2)
- End-to-end COM-RPC notification delivery
- End-to-end JSON-RPC event delivery
- Client crash without unregister (graceful handling)
- High-frequency event stress test
- Multiple clients subscribing/unsubscribing dynamically

### Performance Tests
- Event dispatch latency vs number of subscribers
- Worker pool saturation under load
- Lock contention measurement
- Memory usage with 100+ subscribers

## Best Practices for Clients

### Registration Lifecycle
```cpp
class MyPlugin {
    MyPlugin() : _listener(Core::Service<Listener>::Create<INotification>()) {
        // Create listener in constructor
    }
    
    void Initialize(IShell* service) {
        auto diag = service->QueryInterface<IDeviceDiagnostics>();
        diag->Register(_listener);
        diag->Release();
    }
    
    void Deinitialize() {
        auto diag = service->QueryInterface<IDeviceDiagnostics>();
        diag->Unregister(_listener);
        diag->Release();
    }
    
    ~MyPlugin() {
        _listener->Release();
    }
    
    Core::ProxyType<Listener> _listener;
};
```

### Callback Safety
- Keep callbacks lightweight (avoid long-running operations)
- Never call back into DeviceDiagnostics from callback (deadlock risk)
- Handle all edge cases (null pointers, invalid states)
- Log errors, don't throw exceptions

## Operational Considerations

### Monitoring
- Track number of registered subscribers (alert if > 100)
- Monitor event dispatch latency (alert if > 100ms)
- Alert on repeated registration errors (misbehaving client)

### Debugging
- Log all register/unregister operations with timestamps
- Count events dispatched per minute
- Instrument worker pool queue depth

## References

- [DeviceDiagnostics Plugin API](../../DeviceDiagnostics.md)
- [Thunder JSON-RPC Events](https://rdkcentral.github.io/Thunder/docs/jsonrpc)
- [Thunder COM-RPC Patterns](https://rdkcentral.github.io/Thunder/docs/com-rpc)
- [IDeviceDiagnostics Interface](https://github.com/rdkcentral/entservices-apis/tree/main/apis/DeviceDiagnostics/IDeviceDiagnostics.h)
- Implementation: `plugin/DeviceDiagnosticsImplementation.cpp::Register()`, `Unregister()`, `Dispatch()`
