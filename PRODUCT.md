# DeviceDiagnostics Plugin - Product Documentation

## Product Overview

The DeviceDiagnostics plugin is a Thunder (WPEFramework) service that provides comprehensive device diagnostic capabilities for RDK-based set-top boxes, streaming devices, and smart TVs. It enables operators, device manufacturers, and application developers to monitor device health, track system milestones, and retrieve critical device configuration parameters in real-time.

## Key Features

### 1. Real-Time AV Decoder Status Monitoring
Monitor audio/video decoder states to detect playback issues and resource utilization:
- **Decoder States**: IDLE, PAUSED, ACTIVE
- **Real-Time Notifications**: Automatic event generation on status changes
- **Hardware Integration**: Direct integration with Essos Resource Manager (ERM) for accurate decoder state tracking
- **Polling Mechanism**: Efficient 30-second polling interval with immediate notification on changes

**Use Case**: Content providers can detect when video playback stalls or when decoders become unavailable, enabling proactive customer support and issue resolution.

### 2. System Milestone Logging
Track critical system events and lifecycle markers for debugging and analytics:
- **Persistent Logging**: Milestones stored in `/opt/logs/rdk_milestones.log`
- **Flexible Markers**: Custom milestone markers for application-specific events
- **RDK Logger Integration**: Seamless integration with RDK logging infrastructure
- **Retrieval API**: Query logged milestones programmatically

**Use Case**: Device manufacturers can track boot sequences, app launches, and critical transitions to identify performance bottlenecks and improve user experience.

### 3. Dynamic Device Configuration Retrieval
Access device configuration parameters without direct file system access:
- **RESTful Interface**: HTTP-based configuration service integration
- **Flexible Parameter Query**: Request specific configuration parameters by name
- **Batch Retrieval**: Efficiently fetch multiple parameters in a single request
- **Structured Response**: Name-value pairs returned as iterable collection

**Use Case**: Management applications can audit device settings, validate configurations, and ensure compliance with deployment policies.

## Product Capabilities

### JSON-RPC API
The plugin exposes a standards-based JSON-RPC 2.0 API accessible over HTTP (default port 9998):

#### Methods
- **`getConfiguration`**: Retrieve device configuration parameters
  - Input: Array of parameter names
  - Output: List of name-value pairs with success indicator
  
- **`getAVDecoderStatus`**: Query current audio/video decoder status
  - Output: Current decoder state (IDLE/PAUSED/ACTIVE)
  
- **`getMilestones`**: Retrieve logged system milestones
  - Output: Array of milestone log entries
  
- **`logMilestone`**: Record a new system milestone
  - Input: Marker string identifying the milestone
  - Output: Success/failure status

#### Events
- **`onAVDecoderStatusChanged`**: Pushed automatically when decoder status changes
  - Payload: New decoder status value

### COM-RPC Interface
For native applications and system services, the plugin provides a COM-RPC interface (`Exchange::IDeviceDiagnostics`) enabling:
- Low-latency in-process communication
- Type-safe C++ interface
- Efficient binary serialization
- Support for out-of-process clients via Thunder's RPC mechanism

## Target Scenarios

### Scenario 1: Video Service Provider Dashboard
**Context**: Operators managing thousands of devices need real-time visibility into playback health.

**Solution**: 
- Subscribe to `onAVDecoderStatusChanged` events across device fleet
- Aggregate decoder status data in monitoring dashboard
- Alert operators when devices show abnormal decoder patterns
- Correlate decoder status with customer support tickets

**Benefits**: Proactive issue detection, reduced customer complaints, faster troubleshooting

### Scenario 2: Device Manufacturing & QA
**Context**: Manufacturers testing new device firmware need detailed boot and initialization logs.

**Solution**:
- Instrument firmware with `logMilestone` calls at key initialization points
- Retrieve milestones via `getMilestones` after boot completion
- Analyze timing between milestones to identify delays
- Compare milestone logs across device variants

**Benefits**: Faster firmware development cycles, improved device reliability, reduced field issues

### Scenario 3: Smart Home Integration
**Context**: Smart home applications need to verify device configuration matches user preferences.

**Solution**:
- Use `getConfiguration` to query current device settings
- Compare against user profile and automation rules
- Reconfigure device if settings drift detected
- Log configuration changes as milestones

**Benefits**: Consistent user experience, automated device management, reduced support calls

### Scenario 4: Content Playback Optimization
**Context**: Streaming applications need to adapt content quality based on device capabilities.

**Solution**:
- Monitor decoder status via `getAVDecoderStatus` before stream start
- Adjust bitrate and codec selection based on decoder availability
- Subscribe to status change events during playback
- Switch streams dynamically if decoder becomes constrained

**Benefits**: Reduced buffering, optimal quality selection, improved viewer satisfaction

## Integration Benefits

### For Application Developers
- **Easy Integration**: RESTful JSON-RPC API works with any HTTP client library
- **Language Agnostic**: Access from JavaScript, Python, C++, or any language
- **Event-Driven**: React to status changes without polling
- **Well-Documented**: Clear API contracts with examples

### For System Integrators
- **Thunder Ecosystem**: Seamless integration with other Thunder plugins
- **Standardized Interface**: Consistent with RDK plugin architecture
- **Flexible Deployment**: In-process or out-of-process hosting
- **Minimal Dependencies**: Lightweight with optional feature flags

### For Device Manufacturers
- **Hardware Abstraction**: Platform-independent API with hardware-specific backends
- **Conditional Compilation**: Enable/disable features based on platform capabilities
- **Extensible Design**: Add platform-specific diagnostics easily
- **Production Ready**: Tested in large-scale RDK deployments

## Performance and Reliability

### Performance Characteristics
- **Low Overhead**: Minimal CPU usage during idle periods
- **Efficient Polling**: 30-second interval balances responsiveness and efficiency
- **Asynchronous Events**: Non-blocking event delivery via Thunder worker pool
- **Fast Response Times**: < 100ms typical latency for status queries

### Reliability Features
- **Thread-Safe**: Concurrent request handling with proper synchronization
- **Graceful Degradation**: Functions with reduced capabilities if optional features unavailable
- **Error Recovery**: Robust error handling and detailed logging
- **Resource Management**: Automatic cleanup and proper lifecycle management
- **Timeout Protection**: Curl timeouts prevent hung requests

### Scalability
- **Single Instance**: One plugin instance serves all clients
- **Multiple Subscribers**: Supports unlimited event subscribers
- **Concurrent Requests**: Handles multiple simultaneous JSON-RPC requests
- **Efficient Notification**: Event multicast to all registered clients

## Deployment Configuration

### Build Options
- **ENABLE_ERM**: Enable AV decoder monitoring via Essos Resource Manager
- **RDK_LOG_MILESTONE**: Enable milestone logging via RDK logger
- **PLUGIN_DEVICEDIAGNOSTICS**: Enable plugin in build

### Runtime Configuration
- **Startup Order**: Configurable plugin initialization order
- **Port Configuration**: JSON-RPC endpoint port (default 9998)
- **Logging Level**: Adjustable via Thunder configuration

### Platform Requirements
- Thunder Framework R4.4+
- libcurl for HTTP client functionality
- ERM library (optional, for decoder monitoring)
- RDK logger (optional, for milestone logging)

## Summary

The DeviceDiagnostics plugin provides essential device health monitoring and diagnostic capabilities for RDK platforms. Its combination of real-time decoder monitoring, milestone logging, and configuration access makes it an invaluable tool for operators, developers, and device manufacturers. The plugin's standards-based API, efficient design, and proven reliability make it production-ready for deployment in millions of devices worldwide.
