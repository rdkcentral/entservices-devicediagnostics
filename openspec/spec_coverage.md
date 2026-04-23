# OpenSpec Coverage Report — DeviceDiagnostics Plugin

**Generated:** 2026-04-23
**Spec File:** openspec/specs/DeviceDiagnostics.spec.md
**Plugin Version:** 1.1.2

---

## Overall Score: 37 / 100

| Category | Score | Max |
|---|---|---|
| Code to Spec Coverage | 5 | 40 |
| Architecture HLA Specification | 9 | 10 |
| External Interface Specification | 8 | 10 |
| Security Specification | 1 | 10 |
| Versioning & Compatibility Specification | 6 | 10 |
| Conformance Testing Automation & Validation | 7 | 10 |
| Performance Specification | 1 | 10 |
| **TOTAL** | **37** | **100** |

---

## Category Breakdown

### 1. Code to Spec Coverage (5 / 40)

| Sub-Criterion | Score | Max | Evidence |
|---|---|---|---|
| Reference Coverage | 0 | 20 | 0 of 6 plugin files contain a `// Spec:` comment |
| Spec Existence | 0 | 10 | No spec reference comments found; score not applicable |
| Spec Completeness | 5 | 5 | `DeviceDiagnostics.spec.md` is complete with all required sections |
| No Orphaned Code | 0 | 5 | All 6 plugin files lack `// Spec:` references |

**File scan results (`plugin/`):**

| File | Has `// Spec:` Reference |
|---|---|
| `plugin/DeviceDiagnostics.cpp` | No |
| `plugin/DeviceDiagnostics.h` | No |
| `plugin/DeviceDiagnosticsImplementation.cpp` | No |
| `plugin/DeviceDiagnosticsImplementation.h` | No |
| `plugin/Module.cpp` | No |
| `plugin/Module.h` | No |

No file in `plugin/` contains a `// Spec: <spec_name>` reference comment. While the spec file itself (`openspec/specs/DeviceDiagnostics.spec.md`) exists and is complete (earning the full 5 pts for Spec Completeness), the absence of any in-code traceability comments results in zero for Reference Coverage, Spec Existence, and No Orphaned Code sub-criteria.

---

### 2. Architecture HLA Specification (9 / 10)

| Sub-Criterion | Score | Max | Evidence |
|---|---|---|---|
| Presence of HLA Spec | 3 | 3 | `## Architecture / Design` section present with Mermaid diagram |
| Clarity of Architecture Diagrams | 3 | 3 | `graph TD` Mermaid diagram with labeled arrows covering all data flows |
| Component/Module Mapping | 2 | 2 | All major components named: Client, Thunder, Plugin, Impl, ERM, SysConfig, MilestoneLog, AVHAL, Notification, Job, JDeviceDiagnostics |
| Traceability to Code | 1 | 2 | Diagram maps to `plugin/` source files via Covered Code section, but auto-generated `JDeviceDiagnostics` and external subsystems (ERM, HAL) are not traceable to files under `plugin/` |

The architecture diagram is detailed and readable, covering the full call flow from client through Thunder, the plugin wrapper, the out-of-process implementation, and all external integrations. One point deducted because `JDeviceDiagnostics` (auto-generated, not in `plugin/`) and external systems like `AVHAL` and `ERM` have no file path traceability within the repository.

---

### 3. External Interface Specification (8 / 10)

| Sub-Criterion | Score | Max | Evidence |
|---|---|---|---|
| Presence of Interface Spec | 3 | 3 | `## External Interfaces` section present with all methods and notification |
| Defined Inputs/Outputs | 3 | 3 | All 4 methods and 1 notification have parameter tables with types, required/optional flags, and error conditions |
| Documentation Completeness | 2 | 2 | All JSON-RPC methods registered via `Exchange::JDeviceDiagnostics::Register` in `DeviceDiagnostics.cpp` line ~82 are documented in spec |
| Validation/Examples | 0 | 2 | No JSON-RPC request/response body examples present in spec |

All four API methods (`getAVDecoderStatus`, `getConfiguration`, `getMilestones`, `logMilestone`) and the `onAVDecoderStatusChanged` notification are fully documented with input/output types. Two points lost because the spec contains no concrete JSON-RPC example payloads (e.g., `{"jsonrpc":"2.0","id":1,"method":"DeviceDiagnostics.1.getAVDecoderStatus","params":{}}` → `{"result":{"avDecoderStatus":"IDLE"}}`).

---

### 4. Security Specification (1 / 10)

| Sub-Criterion | Score | Max | Evidence |
|---|---|---|---|
| Presence of Security Spec | 1 | 3 | `## Security` section present but marked "Not applicable — no explicit security model defined" |
| Threat Model/Analysis | 0 | 3 | No threat model or attack surface analysis included |
| Security Requirements | 0 | 2 | No specific mitigations, access controls, or trust levels defined |
| Validation/Testing | 0 | 2 | No security tests or validation criteria documented |

The Security section exists as a placeholder, earning partial credit for presence. No threat model, no security requirements, and no security test coverage are defined anywhere in the spec or test suite. This is the largest area flagged in Open Queries.

---

### 5. Versioning & Compatibility Specification (6 / 10)

| Sub-Criterion | Score | Max | Evidence |
|---|---|---|---|
| Presence of Versioning Spec | 3 | 3 | `## Versioning & Compatibility` section present |
| Versioning Scheme Defined | 2 | 3 | Version 1.1.2 (Major.Minor.Patch) documented, callsign and library name listed; versioning semantics (what triggers a Major/Minor/Patch bump) not described |
| Backward/Forward Compatibility | 1 | 2 | "Additions should be additive to avoid breaking existing clients" — minimal statement, no formal guarantee |
| Migration/Upgrade Path | 0 | 2 | No migration or upgrade path described |

The version number is documented and traces directly to `DeviceDiagnostics.cpp` (`API_VERSION_NUMBER_MAJOR/MINOR/PATCH`). However, the spec does not define what constitutes a breaking vs. non-breaking change, nor does it describe how clients should handle version transitions.

---

### 6. Conformance Testing Automation & Validation (7 / 10)

| Sub-Criterion | Score | Max | Evidence |
|---|---|---|---|
| Presence of Conformance Tests | 3 | 3 | L1 tests: `Tests/L1Tests/tests/test_DeviceDiagnostics.cpp` exists; L2 tests: `Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp` exists |
| Test Coverage | 2 | 3 | L1 covers `getAVDecoderStatus`, `getConfiguration`, plugin registration; `getMilestones` and `logMilestone` not visible in L1 test scans; spec states coverage goal for all 4 methods |
| Test Documentation | 2 | 2 | `Tests/README.md` provides full instructions: building mock lib, running L1/L2/L2-OOP via `act` locally and via GitHub Actions |
| Validation Results | 0 | 2 | No actual test pass/fail results, CI badges, or run artifacts documented in spec or test files |

Both test suites are present and the README gives comprehensive steps for local and CI execution. One point deducted on Test Coverage because `getMilestones` and `logMilestone` are not confirmed as covered in L1 tests from the visible test file content. No historical test result records are available.

---

### 7. Performance Specification (1 / 10)

| Sub-Criterion | Score | Max | Evidence |
|---|---|---|---|
| Presence of Performance Spec | 1 | 3 | `## Performance` section present but marked "Not applicable — no explicit performance requirements defined" |
| Defined Performance Metrics | 0 | 3 | No latency, throughput, or resource usage targets defined |
| Test Coverage for Performance | 0 | 2 | No performance or benchmarking tests in L1 or L2 test suites |
| Results & Validation | 0 | 2 | No performance results or SLA targets documented |

The Performance section is a required placeholder. No measurable performance targets are defined for any API method (e.g., no max response time for `getAVDecoderStatus` or `getConfiguration`).

---

## Gaps & Issues

### Unspecified Methods (registered in code but missing from spec)

No JSON-RPC gaps found. All methods exposed via `Exchange::JDeviceDiagnostics::Register(*this, _deviceDiagnostics)` (`DeviceDiagnostics.cpp`, line ~82) correspond directly to methods documented in the External Interfaces section of the spec.

> **Note:** `GetPreviousRebootInfo` is partially implemented in `DeviceDiagnosticsImplementation.cpp` (file path constants `PREVIOUS_REBOOT_INFO_FILE` and `HARD_POWER_INFO_FILE` are defined; the method body exists). It is intentionally excluded from this spec because it was introduced exclusively via the archived change `openspec/changes/archive/2026-04-16-add-new-api-get-previous-reboot-info/`. Its JSON-RPC interface stub has not yet been merged (task 6.1 in `tasks.md` remains unchecked).

Internal implementation methods (`getConfig`, `getMostActiveDecoderStatus`, `dispatchEvent`, `Dispatch`, `AVPollThread`, `onDecoderStatusChange`, `Register`, `Unregister`) are not JSON-RPC endpoints and are correctly absent from the External Interfaces section.

---

### Orphaned Code (no `// Spec:` reference comment found)

All plugin source files under `plugin/` are orphaned with respect to spec traceability:

| File | Reason |
|---|---|
| `plugin/DeviceDiagnostics.cpp` | No `// Spec:` comment |
| `plugin/DeviceDiagnostics.h` | No `// Spec:` comment |
| `plugin/DeviceDiagnosticsImplementation.cpp` | No `// Spec:` comment |
| `plugin/DeviceDiagnosticsImplementation.h` | No `// Spec:` comment |
| `plugin/Module.cpp` | No `// Spec:` comment |
| `plugin/Module.h` | No `// Spec:` comment |

This is the single largest contributor to the low overall score (35 pts lost across Reference Coverage, Spec Existence, and No Orphaned Code).

---

### Spec Sections with No Code Traceability

| Spec Reference | Issue |
|---|---|
| `JDeviceDiagnostics` (Architecture diagram) | Auto-generated stub; not a file under `plugin/` |
| `ERM` / `essos-resmgr` (Architecture diagram) | External library; no file under `plugin/` |
| `AVHAL` (Architecture diagram) | External HAL; no file under `plugin/` |
| `SysConfig` (Architecture diagram) | External properties store; no direct file under `plugin/` |
| `MilestoneLog` (Architecture diagram) | `/opt/logs/rdk_milestones.log` defined in `DeviceDiagnosticsImplementation.cpp`; path not documented in spec |

---

### Open Queries Affecting Scores

| Open Query | Impacted Category | Score Impact |
|---|---|---|
| Valid `avDecoderStatus` string values not enumerated | External Interfaces — Defined Inputs/Outputs (already scored 3/3, but enum would prevent ambiguity for L2 test validation) | Low |
| `getConfiguration` accepted property names not specified | External Interfaces — Validation/Examples | Contributes to 0/2 for examples |
| Milestone log file path not documented in spec | External Interfaces — Documentation Completeness | Minor gap in completeness |
| No security or trust-level policy defined | Security Specification | Direct cause of 1/10 |
| Non-ERM decoder status behavior not documented | Architecture / Design — Traceability to Code | Minor; diagram references ERM for the ERM path but non-ERM fallback not shown |

---

## Suggestions for Improvement

### Code to Spec Coverage (5 / 40 → target: 35+ / 40)

- **Add `// Spec: DeviceDiagnostics` reference comments** to all files in `plugin/`. At minimum, add the comment to the top of each `.cpp` and `.h` file. Example for `plugin/DeviceDiagnostics.cpp`:
  ```cpp
  // Spec: DeviceDiagnostics
  ```
  This single change unlocks 30 points (Reference Coverage, Spec Existence, and No Orphaned Code) once applied to all 6 files.
- **Standardize the comment format** across the repository: decide between file-level (`// Spec: <name>`) and function-level comments, and apply consistently.

---

### External Interface Specification (8 / 10 → target: 10 / 10)

- **Add JSON-RPC request/response body examples** for each method in the `## External Interfaces` section. Example for `getAVDecoderStatus`:
  ```json
  // Request
  {"jsonrpc":"2.0","id":1,"method":"DeviceDiagnostics.1.getAVDecoderStatus","params":{}}
  // Response
  {"jsonrpc":"2.0","id":1,"result":{"avDecoderStatus":"IDLE"}}
  ```
  Adding examples for all 4 methods and the notification would earn the remaining 2 points.
- **Document the valid enum values for `avDecoderStatus`**: the implementation defines `"IDLE"`, `"PAUSED"`, `"ACTIVE"` (from `decoderStatusStr[]` in `DeviceDiagnosticsImplementation.cpp`). Add these to the spec under `getAVDecoderStatus` response fields and resolve Open Query 1.

---

### Security Specification (1 / 10 → target: 6+ / 10)

- **Define a minimal threat model** for the plugin. At minimum, document the attack surface: the plugin receives JSON-RPC calls over the Thunder WebSocket; unauthenticated clients can invoke `logMilestone` and read configuration values. Document who is trusted and who is not.
- **Specify security requirements** such as: "Only processes with `org.rdk.DeviceDiagnostics` Thunder token may call `logMilestone`" or "The plugin runs in the Thunder sandbox with no elevated privileges." Even a brief statement earns 2 pts.
- **Reference any existing Thunder security controls** (e.g., Thunder's plugin security config, token validation) in the spec to earn partial credit for security validation.

---

### Versioning & Compatibility Specification (6 / 10 → target: 9 / 10)

- **Define versioning semantics**: clarify what constitutes a Major, Minor, or Patch increment for this plugin (e.g., "Major: breaking interface change; Minor: new non-breaking API; Patch: bug fix"). This earns the missing 1 pt for Versioning Scheme Defined.
- **Strengthen the compatibility statement**: replace the current single-sentence note with an explicit policy, e.g., "New fields may be added to responses in Minor versions. Existing fields SHALL NOT be removed or renamed without a Major version increment."

---

### Conformance Testing Automation & Validation (7 / 10 → target: 9 / 10)

- **Confirm and document L1 test coverage for `getMilestones` and `logMilestone`**: review `Tests/L1Tests/tests/test_DeviceDiagnostics.cpp` fully and add test cases if missing. Update the spec's Conformance Testing section to explicitly list which test function covers which requirement.
- **Add a test results section or CI badge** to the spec or to `Tests/README.md` referencing the GitHub Actions workflow (`.github/workflows/tests-trigger.yml`) so historical pass/fail status is accessible.

---

### Performance Specification (1 / 10 → target: 4+ / 10)

- **Define at least one measurable performance target** for the most latency-sensitive method. For example: "`getAVDecoderStatus` SHALL return a response within 200 ms under normal operating conditions." Even minimal targets earn 3 pts for Defined Performance Metrics.
- **Add a note about the `getConfiguration` curl timeout** (30 seconds, defined as `curlTimeoutInSeconds` in `DeviceDiagnosticsImplementation.cpp`) — this is an implicit performance constraint that should be documented in the spec.

---

## References
- Spec: [openspec/specs/DeviceDiagnostics.spec.md](openspec/specs/DeviceDiagnostics.spec.md)
- Plugin Source: [plugin/DeviceDiagnostics.cpp](plugin/DeviceDiagnostics.cpp), [plugin/DeviceDiagnostics.h](plugin/DeviceDiagnostics.h), [plugin/DeviceDiagnosticsImplementation.cpp](plugin/DeviceDiagnosticsImplementation.cpp), [plugin/DeviceDiagnosticsImplementation.h](plugin/DeviceDiagnosticsImplementation.h)
- Tests: [Tests/L1Tests/](Tests/L1Tests/), [Tests/L2Tests/](Tests/L2Tests/), [Tests/README.md](Tests/README.md)
- Skill: [.github/skills/openspec-coverage/SKILL.md](.github/skills/openspec-coverage/SKILL.md)
