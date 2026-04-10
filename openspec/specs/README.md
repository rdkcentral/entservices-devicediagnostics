# DeviceDiagnostics Plugin Specifications

This directory contains comprehensive specifications for the DeviceDiagnostics Thunder plugin. Each specification documents API contracts, behavioral requirements, integration points, and operational considerations.

## Overview

The DeviceDiagnostics plugin provides four core diagnostic capabilities for RDK TV and STB devices:

1. **Configuration Retrieval** - Query device configuration via HTTP backend
2. **Milestone Logging** - Record and retrieve diagnostic timeline markers
3. **AV Decoder Monitoring** - Track audio/video decoder operational status
4. **Event Notifications** - Publish-subscribe event delivery system

## Specification Index

### Core Documentation

- **[Overview](overview.md)** ⭐ START HERE
  - Complete system architecture and integration guide
  - Plugin lifecycle and data flows
  - Performance characteristics and operational considerations
  - 150+ page comprehensive reference

### Capability Specifications

Each capability has a dedicated specification with detailed requirements, API contracts, use cases, and implementation guidance:

- **[Configuration Retrieval](configuration-retrieval/spec.md)**
  - HTTP proxy to backend configuration service
  - Parameter-based queries with name-value pair results
  - 30-second timeout, no caching, localhost-only
  
- **[Milestone Logging](milestone-logging/spec.md)**
  - RDK logger integration for timestamped markers
  - File-based persistence at `/opt/logs/rdk_milestones.log`
  - Used for boot analysis and performance tracking
  
- **[AV Decoder Monitoring](av-decoder-monitoring/spec.md)**
  - Real-time decoder status tracking (IDLE/PAUSED/ACTIVE)
  - ERM library integration with background polling
  - Automatic status change notifications every 30 seconds
  
- **[Event Notification System](event-notification/spec.md)**
  - Dual-protocol support (COM-RPC + JSON-RPC)
  - Publish-subscribe pattern with worker pool dispatch
  - Multiple concurrent subscriber support

## Quick Reference

### API Summary

| Method | Capability | Description |
|--------|-----------|-------------|
| `getConfiguration(names[])` | Configuration | Retrieve config values by name |
| `getMilestones()` | Milestones | Get all recorded milestones |
| `logMilestone(marker)` | Milestones | Record a milestone marker |
| `getAVDecoderStatus()` | AV Monitoring | Get current decoder status |

| Event | Capability | Description |
|-------|-----------|-------------|
| `onAVDecoderStatusChanged` | AV Monitoring | Decoder status changed notification |

### Key Dependencies

```
Thunder Framework ──┬─→ JSON-RPC Interface (Port 9998)
                    └─→ COM-RPC Interface (C++)

External Services ──┬─→ Config Backend (Port 10999)
                    ├─→ File System (/opt/logs/)
                    └─→ ERM Library (Hardware)
```

### Build Flags

| Flag | Default | Effect |
|------|---------|--------|
| `PLUGIN_DEVICEDIAGNOSTICS` | OFF | Enable plugin build |
| `BUILD_ENABLE_ERM` | OFF | Enable AV decoder monitoring |
| `RDK_LOG_MILESTONE` | Platform | Enable milestone logging |

## How to Use These Specs

### For **Plugin Developers** (Maintaining the Plugin)
1. Start with [Overview](overview.md) for architecture
2. Reference individual capability specs for implementation details
3. Follow error handling and threading patterns documented
4. Consult testing requirements before changes

### For **Client Developers** (Using the Plugin)
1. Read [Overview](overview.md) API Surface section
2. Jump to relevant capability spec for your use case
3. Review "Client Usage Patterns" examples
4. Check "Integration Points" for COM-RPC vs JSON-RPC

### For **System Integrators** (Deploying the Plugin)
1. Review [Overview](overview.md) System Integration section
2. Check build configuration and dependencies
3. Review operational considerations and monitoring
4. Consult troubleshooting guide

### For **QA/Test Engineers** (Testing the Plugin)
1. Each spec has "Testing Requirements" section
2. Review "Error Scenarios" for edge cases
3. Check "Performance Characteristics" for benchmarks
4. Consult "Use Cases" for functional test scenarios

## Document Structure

Each capability specification follows this structure:

```
1. Overview
   - Purpose and scope
   - Target devices

2. API Contract
   - Request/response formats
   - Parameter definitions

3. Functional Requirements
   - Detailed behavior specifications
   - Success/failure conditions

4. Non-Functional Requirements  
   - Performance targets
   - Reliability guarantees

5. Dependencies
   - Runtime and compile-time

6. Integration Points
   - External system contracts
   - Client usage patterns

7. Use Cases
   - Real-world scenarios
   - Actor-goal-flow format

8. Error Scenarios
   - Failure modes and behaviors

9. Known Limitations
   - Current constraints and workarounds

10. Testing Requirements
    - L1, L2, performance, hardware tests

11. Future Enhancements
    - Planned improvements

12. References
    - Related documentation
```

## Specification Status

| Specification | Version | Status | Last Updated |
|--------------|---------|--------|--------------|
| Overview | 1.0.0 | ✅ Active | 2026-04-10 |
| Configuration Retrieval | 1.0.0 | ✅ Active | 2026-04-10 |
| Milestone Logging | 1.0.0 | ✅ Active | 2026-04-10 |
| AV Decoder Monitoring | 1.0.0 | ✅ Active | 2026-04-10 |
| Event Notification | 1.0.0 | ✅ Active | 2026-04-10 |

## Related Documentation

### Plugin Documentation
- [README.md](../../README.md) - Repository overview
- [DeviceDiagnostics.md](../../DeviceDiagnostics.md) - API reference (auto-generated)
- [ARCHITECTURE.md](../../ARCHITECTURE.md) - System architecture
- [plugin/README.md](../../plugin/README.md) - Plugin-specific notes

### Thunder Framework
- [Thunder Documentation](https://rdkcentral.github.io/Thunder/)
- [Thunder Interfaces](https://github.com/rdkcentral/ThunderInterfaces)
- [Thunder Plugins Guide](https://rdkcentral.github.io/Thunder/docs/plugins)

### RDK Resources
- [RDK Central](https://rdkcentral.com)
- [RDK Wiki](https://wiki.rdkcentral.com)
- [RDK GitHub](https://github.com/rdkcentral)

## Contributing

When updating specifications:

1. **Maintain Consistency** - Follow the established structure
2. **Version Updates** - Bump version numbers for significant changes
3. **Cross-References** - Update related specs when behavior changes
4. **Examples** - Provide code examples for new features
5. **Testing** - Document test requirements for new functionality

### Specification Review Checklist

- [ ] API contract complete and unambiguous
- [ ] All functional requirements documented
- [ ] Error scenarios defined
- [ ] Integration points specified
- [ ] Usage examples provided
- [ ] Testing requirements included
- [ ] Cross-references updated
- [ ] Version number bumped if applicable

## Questions & Support

- **Issues:** [GitHub Issues](https://github.com/rdkcentral/entservices-devicediagnostics/issues)
- **Discussions:** RDK Community Forums
- **Documentation Bugs:** File issue with `documentation` label

---

**Specification Set Version:** 1.0.0  
**Generated:** 2026-04-10  
**Format:** OpenSpec (spec-driven schema)
