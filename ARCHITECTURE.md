# DeviceDiagnostics Plugin Architecture

## Overview

The DeviceDiagnostics plugin is a standalone Thunder (WPEFramework) plugin that provides device diagnostic capabilities for RDK-based devices. It enables real-time monitoring of audio/video decoder status, milestone logging, and device configuration retrieval through a JSON-RPC interface.

## System Architecture

### Component Structure

```
┌─────────────────────────────────────────────────────────────┐
│                    Thunder Framework                         │
│  ┌────────────────────────────────────────────────────────┐ │
│  │          DeviceDiagnostics Plugin                      │ │
│  │  ┌──────────────────┐     ┌──────────────────────┐    │ │
│  │  │  DeviceDiagnostics│     │DeviceDiagnostics     │    │ │
│  │  │  (JSON-RPC Layer) │────▶│Implementation        │    │ │
│  │  │                   │     │(Business Logic)      │    │ │
│  │  └──────────────────┘     └──────────────────────┘    │ │
│  │           │                         │                   │ │
│  │           │                         ▼                   │ │
│  │           │                 ┌─────────────────┐        │ │
│  │           │                 │  AV Poll Thread │        │ │
│  │           │                 │  (ERM Monitor)  │        │ │
│  │           │                 └─────────────────┘        │ │
│  └───────────┼─────────────────────────┼──────────────────┘ │
└──────────────┼─────────────────────────┼────────────────────┘
               │                         │
               ▼                         ▼
        ┌─────────────┐          ┌──────────────┐
        │  JSON-RPC   │          │ ERM Library  │
        │   Client    │          │ (Essos RM)   │
        └─────────────┘          └──────────────┘
               │                         │
               ▼                         ▼
        ┌─────────────┐          ┌──────────────┐
        │  HTTP/Curl  │          │  AV Decoders │
        │   (Port     │          │   Hardware   │
        │   10999)    │          └──────────────┘
        └─────────────┘
```

### Key Components

#### 1. DeviceDiagnostics (Plugin Interface)
- **Purpose**: Thunder plugin entry point implementing IPlugin and JSONRPC interfaces
- **Responsibilities**:
  - Plugin lifecycle management (Initialize/Deinitialize)
  - JSON-RPC method registration and dispatching
  - Event notification handling
  - Connection management for remote clients
- **Key Features**:
  - Aggregates IDeviceDiagnostics interface for COM-RPC access
  - Manages notification callbacks for status change events
  - Handles out-of-process communication via RPC

#### 2. DeviceDiagnosticsImplementation (Core Logic)
- **Purpose**: Implements the business logic and hardware interaction
- **Responsibilities**:
  - AV decoder status monitoring (via ERM library when ENABLE_ERM is defined)
  - Configuration retrieval via HTTP requests
  - Milestone logging functionality
  - Event dispatching to registered clients
- **Threading Model**:
  - Main thread: Handles JSON-RPC requests
  - AV Poll Thread: Periodically polls ERM library for decoder status changes (when ENABLE_ERM enabled)
  - Job Dispatch: Uses Thunder's worker pool for event notifications

#### 3. Helper Utilities
- **UtilsLogging.h**: Provides standardized logging macros (LOGINFO, LOGWARN, LOGERR)
- **UtilsJsonRpc.h**: Common JSON-RPC helper macros for parameter validation and response handling

## Data Flow

### AV Decoder Status Monitoring
1. Background thread polls ERM library every 30 seconds
2. Compares current status with last known status
3. On status change:
   - Creates JsonObject with new status
   - Dispatches event via worker pool
   - Notifies all registered INotification clients
   - Sends OnAVDecoderStatusChanged event to JSON-RPC clients

### Configuration Retrieval
1. Client sends GetConfiguration request with parameter names
2. Plugin constructs JSON request payload
3. Makes HTTP POST to localhost:10999 using libcurl
4. Parses response and extracts name-value pairs
5. Returns iterator over configuration parameters

### Milestone Logging
1. Client sends LogMilestone request with marker string
2. Plugin validates marker is non-empty
3. Calls logMilestone() from RDK logger when RDK_LOG_MILESTONE is defined
4. Returns success/failure status

## Plugin Framework Integration

### Thunder Plugin Architecture
- **Interface Definition**: Uses Thunder's Exchange::IDeviceDiagnostics interface
- **COM-RPC Support**: Supports both in-process and out-of-process communication
- **Service Registration**: Registered as a Thunder service via SERVICE_REGISTRATION macro
- **Plugin Configuration**: Configured via JSON configuration file (DeviceDiagnostics.config)

### Build System Integration
- **CMake-based**: Uses Thunder's plugin CMake infrastructure
- **Modular Build**: Separates plugin shell and implementation libraries
- **Conditional Compilation**: Supports ENABLE_ERM and RDK_LOG_MILESTONE feature flags

## Dependencies and Interfaces

### External Dependencies
- **Thunder Framework**: Core framework, plugins, and definitions
- **libcurl**: HTTP client for configuration retrieval
- **ERM (Essos Resource Manager)**: Optional AV decoder monitoring (essosrmgr library)
- **RDK Logger**: Optional milestone logging support

### Internal Dependencies
- **Helper utilities**: UtilsLogging.h, UtilsJsonRpc.h
- **Thread synchronization**: C++11 threading primitives (mutex, condition_variable)

### API Interfaces
1. **IDeviceDiagnostics**: COM-RPC interface for programmatic access
2. **JSON-RPC**: RESTful API over HTTP for web/script access
3. **INotification**: Event callback interface for status change notifications

## Technical Implementation Details

### Thread Safety
- Uses mutex locking for AV decoder status access
- Condition variables for efficient polling thread wake-up
- JSONRPC layer handles concurrent request serialization
- Notification list protected by admin lock

### Memory Management
- Thunder's smart pointers (Core::ProxyType) for object lifecycle
- AddRef/Release pattern for COM-RPC objects
- RAII for thread management (std::thread with joinable check)

### Error Handling
- Returns Thunder error codes (ERROR_NONE, ERROR_GENERAL)
- Validates parameters before processing
- Handles file access failures gracefully
- curl error checking with detailed logging

### Performance Characteristics
- Polling interval: 30 seconds for decoder status (configurable)
- Curl timeout: 30 seconds for HTTP requests
- Event dispatch: Non-blocking via worker pool
- Minimal memory footprint: Single instance design pattern

## Extensibility

The plugin architecture supports extension through:
- Additional JSON-RPC methods can be registered in Initialize()
- New event types can be added to the Event enumeration
- Platform-specific implementations via conditional compilation
- Custom configuration endpoints via HTTP service integration
