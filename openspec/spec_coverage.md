# Openspec Coverage Report — DeviceDiagnostics Plugin

**Generated:** 2026-04-23  
**Spec file:** `openspec/specs/DeviceDiagnostics.spec.md`  
**Plugin:** `org.rdk.DeviceDiagnostics`  

---

## Overall Score

| Category | Score | Max |
|---|---|---|
| Code to Spec Coverage | 5 | 40 |
| Architecture HLA Specification | 9 | 10 |
| External Interface Specification | 7 | 10 |
| Security Specification | 1 | 10 |
| Versioning & Compatibility Specification | 0.5 | 10 |
| Conformance Testing Automation and Validation | 6.5 | 10 |
| Performance Specification | 0 | 10 |
| **TOTAL** | **29** | **100** |

---

## Category Breakdown

---

### 1. Code to Spec Coverage (5 / 40)

This category measures whether code modules reference their governing spec using the standard `// Spec: <spec_name>` comment format.

**Code modules scanned:**

| File | Spec Reference Present |
|---|---|
| `plugin/DeviceDiagnostics.cpp` | No |
| `plugin/DeviceDiagnostics.h` | No |
| `plugin/DeviceDiagnosticsImplementation.cpp` | No |
| `plugin/DeviceDiagnosticsImplementation.h` | No |
| `plugin/Module.cpp` | No |
| `plugin/Module.h` | No |

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Reference Coverage | 0 | 20 | 0 of 6 modules contain a `// Spec:` comment |
| Spec Existence | 0 | 10 | No spec references in code; `DeviceDiagnostics.spec.md` exists but is not referenced by modules |
| Spec Completeness | 5 | 5 | `DeviceDiagnostics.spec.md` contains all required sections: title, description, requirements |
| No Orphaned Code | 0 | 5 | All 6 modules lack spec references — all are orphaned |

**Gaps:**
- No `// Spec:` comment exists in any source file.
- All plugin modules are disconnected from the spec.

**Suggestions:**
- Add `// Spec: DeviceDiagnostics` at the top of each plugin source and header file.
- Add `// Spec: DeviceDiagnostics` to `Module.cpp` and `Module.h`.

---

### 2. Architecture HLA Specification (9 / 10)

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of HLA Spec | 3 | 3 | Full Architecture/Design section present with component descriptions and data flow summary |
| Clarity of Architecture Diagrams | 3 | 3 | Mermaid component diagram present; all components, interfaces, and data flows clearly labelled |
| Component/Module Mapping | 2 | 2 | All major components mapped: `DeviceDiagnostics`, `DeviceDiagnosticsImplementation`, `Notification` sink, `Exchange::IDeviceDiagnostics`, `SysAPI`, `AVDriver`, `MilestoneLog` |
| Traceability to Code | 1 | 2 | "Covered Code" section lists functions per file. However, no inline code-level annotations or bidirectional cross-links from diagram nodes to code symbols |

**Gaps:**
- No inline code annotations link back from architecture components to code locations.
- Architecture diagram nodes are not anchored with file or line references.

**Suggestions:**
- Add code-level cross-references in the spec (e.g., link each component in the diagram to its implementation file).

---

### 3. External Interface Specification (7 / 10)

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of Interface Spec | 3 | 3 | Dedicated "External Interfaces" section with JSON-RPC Methods and Events sub-sections |
| Defined Inputs/Outputs | 3 | 3 | All 4 methods (`getAVDecoderStatus`, `getConfiguration`, `getMilestones`, `logMilestone`) and 1 event (`onAVDecoderStatusChanged`) fully documented with parameter tables and types |
| Documentation Completeness | 1 | 2 | Missing: error codes beyond `success` boolean; `avDecoderStatus` string enum values not defined; authoritative list of `getConfiguration` property names absent |
| Validation/Examples | 0 | 2 | No JSON request/response examples provided for any method |

**Gaps:**
- `avDecoderStatus` / `avDecoderStatusChange` values (`IDLE`, `PAUSED`, `ACTIVE`) are only visible in implementation code — not documented in the spec.
- No sample JSON payloads.
- Error code schema beyond the boolean `success` field is absent.
- No list of supported `getConfiguration` property names.

**Suggestions:**
- Add an `avDecoderStatus` enum table with all valid string values.
- Add sample request/response JSON blocks for each method.
- Define error response schemas.
- Document the supported configuration property names for `getConfiguration`.

---

### 4. Security Specification (1 / 10)

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of Security Spec | 1 | 3 | Section exists but contains only a single placeholder statement acknowledging that access control is undefined |
| Threat Model / Analysis | 0 | 3 | No threat model or security analysis |
| Security Requirements | 0 | 2 | No security requirements or mitigations specified |
| Validation / Testing | 0 | 2 | No security-focused tests |

**Gaps:**
- No access control model, token/credential requirements, or client authorization scheme is defined.
- No threat model covering attack surfaces (e.g., unauthenticated JSON-RPC caller, milestone log injection, CURL endpoint trust).
- No security requirements or mitigations documented.
- No security validation tests.

**Suggestions:**
- Define an access control policy for `org.rdk.DeviceDiagnostics`.
- Add a threat model covering JSON-RPC caller authentication and the internal CURL call to `http://127.0.0.1:10999`.
- Document how `logMilestone` marker input is sanitized against log injection.
- Add security validation tests for boundary inputs.

---

### 5. Versioning & Compatibility Specification (0.5 / 10)

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of Versioning Spec | 0.5 | 3 | Section present but states "Not applicable" with no content |
| Versioning Scheme Defined | 0 | 3 | No versioning scheme defined (code uses `1.1.2` via `API_VERSION_NUMBER_*` macros but spec does not reference this) |
| Backward/Forward Compatibility | 0 | 2 | Not defined |
| Migration/Upgrade Path | 0 | 2 | Not defined |

**Gaps:**
- The implementation defines `API_VERSION_NUMBER_MAJOR=1`, `MINOR=1`, `PATCH=2` but the spec does not reference or explain the versioning policy.
- No compatibility guarantees, deprecation policy, or migration guidance.

**Suggestions:**
- Document the versioning scheme (e.g., semver-aligned major.minor.patch as used in `API_VERSION_NUMBER_*`).
- State backward-compatibility guarantees for each major version.
- Describe how breaking API changes will be versioned and communicated.

---

### 6. Conformance Testing Automation and Validation (6.5 / 10)

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of Conformance Tests | 3 | 3 | L1 (`Tests/L1Tests/tests/test_DeviceDiagnostics.cpp`) and L2 (`Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp`) test suites present |
| Test Coverage | 2.5 | 3 | Methods covered: `getConfiguration` (L1+L2), `getAVDecoderStatus` (L1+L2), `logMilestone` (L1+L2), `getMilestones` (L2), `onAVDecoderStatusChanged` event (L2). FR-1 through FR-5 all have at least one test. No traceability map from tests to specific FRs |
| Test Documentation | 1 | 2 | Inline `/** Test case Details **/` comments in L1/L2 tests describe intent; no formal documentation linking tests to spec requirements |
| Validation Results | 0 | 2 | No test results, pass/fail history, or CI coverage reports tracked in the spec |

**Test Coverage Map (informal):**

| FR | Description | L1 | L2 |
|---|---|---|---|
| FR-1 | `getAVDecoderStatus` | `getAVDecoderStatus` | `IDLE_GetAVDecoderStatus_JSONRPC`, `ACTIVE_GetAVDecoderStatus_JSONRPC`, `IDLE_GetAVDecoderStatus_COMRPC` |
| FR-2 | `getConfiguration` | `getConfiguration` | `GetConfiguration_JSONRPC` |
| FR-3 | `getMilestones` | _(none)_ | `GetMilestones_JSONRPC` |
| FR-4 | `logMilestone` | _(none)_ | `LogMilestone_JSONRPC`, `LogMilestone_COMRPC` |
| FR-5 | `onAVDecoderStatusChanged` event | _(via notification mock)_ | `ACTIVE_GetAVDecoderStatus_JSONRPC` |
| FR-6 | Autostart | _(not tested)_ | _(not tested)_ |
| FR-7 | Callsign `org.rdk.DeviceDiagnostics` | `ActivateService` via L2 setup | `ActivateService` |
| FR-8 | COM-RPC delegation | _(mock)_ | `COMRPC` tests |

**Gaps:**
- `getMilestones` is not covered in L1 tests.
- FR-6 (autostart) has no test coverage.
- No formal traceability matrix linking test names to FR identifiers in the spec.
- No validation results or CI status documented.

**Suggestions:**
- Add a traceability matrix in the spec or in a separate test README.
- Add an L1 test for `getMilestones`.
- Document expected test results and link to CI pipeline results.

---

### 7. Performance Specification (0 / 10)

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of Performance Spec | 0 | 3 | Section present but states "Not applicable" |
| Defined Performance Metrics | 0 | 3 | No latency, throughput, or resource usage targets defined |
| Test Coverage for Performance | 0 | 2 | No performance tests |
| Results & Validation | 0 | 2 | No performance results documented |

**Gaps:**
- No response time targets for any API method.
- The ERM poll interval (`AVDECODERSTATUS_RETRY_INTERVAL = 30s`) and CURL timeout (`curlTimeoutInSeconds = 30`) are implementation-defined constants not surfaced in the spec.

**Suggestions:**
- Define acceptable latency targets for `getAVDecoderStatus` and `getConfiguration`.
- Document the ERM poll interval and CURL timeout as configurable performance parameters.
- Add performance tests (e.g., assert response time < threshold).

---

## Summary of Key Gaps

| Priority | Gap | Category |
|---|---|---|
| High | No `// Spec:` references in any source file — all code is orphaned from spec | Code to Spec |
| High | Security policy completely undefined; no threat model or access control | Security |
| High | Versioning scheme absent despite API version constants in code | Versioning |
| Medium | `avDecoderStatus` enum values not documented in spec | External Interface |
| Medium | No JSON request/response examples | External Interface |
| Medium | No formal FR-to-test traceability matrix | Conformance |
| Medium | `getMilestones` not covered in L1 tests | Conformance |
| Medium | Performance targets entirely absent | Performance |
| Low | Code-to-architecture traceability is one-way (spec lists code, code does not reference spec) | HLA |
| Low | No CI test results tracked in spec | Conformance |

---

## Suggestions for Improvement

1. **Add `// Spec: DeviceDiagnostics` comments** to every plugin source and header file to close the 35-point gap in Code to Spec Coverage.
2. **Define the `avDecoderStatus` enum** — document `IDLE`, `PAUSED`, `ACTIVE` as the normative value set.
3. **Define a security section** — at minimum: caller authentication requirement and input sanitization policy for `logMilestone`.
4. **Capture version policy** — document the semver scheme already used by `API_VERSION_NUMBER_*` macros.
5. **Add a traceability table** in the spec mapping each FR to the test(s) that validate it.
6. **Add JSON examples** for each method's request and response.
7. **Set minimum performance targets** for `getAVDecoderStatus` and `getConfiguration`.

---

## References

- Spec: [openspec/specs/DeviceDiagnostics.spec.md](specs/DeviceDiagnostics.spec.md)
- L1 Tests: [Tests/L1Tests/tests/test_DeviceDiagnostics.cpp](../Tests/L1Tests/tests/test_DeviceDiagnostics.cpp)
- L2 Tests: [Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp](../Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp)
- Plugin: [plugin/](../plugin/)
