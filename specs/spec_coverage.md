# Openspec Coverage Report — DeviceDiagnostics

**Generated:** 2026-04-27  
**Repository:** `entservices-devicediagnostics`  
**Spec directory:** `specs/`  
**Skill:** `openspec-coverage`

---

## Overall Score: 86.6 / 100

| Category | Weight | Raw Score | Weighted Points |
|---|---|---|---|
| Code to Spec Coverage | 40% | 95.3% | **38.1** |
| Architecture HLA Specification | 10% | 100% | **10.0** |
| External Interface Specification | 10% | 95% | **9.5** |
| Security Specification | 10% | 65% | **6.5** |
| Performance Specification | 10% | 70% | **7.0** |
| Versioning & Compatibility | 10% | 80% | **8.0** |
| Conformance Testing & Validation | 10% | 75% | **7.5** |
| **TOTAL** | **100%** | | **86.6 / 100** |

---

## 1. Code to Spec Coverage — 38.1 / 40

### 1.1 Method Inventory

The following table lists all coverable symbols identified across production source files.

| File | Total Symbols | Covered by Spec | Orphaned |
|---|---|---|---|
| `IDeviceDiagnostics.h` | 11 | 11 | 0 |
| `plugin/DeviceDiagnostics.h` | 5 | 4 | 1 |
| `plugin/DeviceDiagnostics.cpp` | 6 | 6 | 0 |
| `plugin/DeviceDiagnosticsImplementation.h` | 2 | 2 | 0 |
| `plugin/DeviceDiagnosticsImplementation.cpp` | 16 | 14 | 2 |
| `plugin/Module.h` | 1 | 1 | 0 |
| `helpers/UtilsJsonRpc.h` | macros only | N/A | — |
| `helpers/UtilsLogging.h` | macros only | N/A | — |
| `helpers/PluginInterfaceBuilder.h` | macros only | N/A | — |
| `Tests/L1Tests/tests/test_DeviceDiagnostics.cpp` | 1 fixture | 1 | 0 |
| `Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp` | 2 helpers | 2 | 0 |
| **TOTAL** | **41** | **38** | **3** |

### 1.2 Covered Symbols (via `## Covered Code` in spec)

The following 38 symbols are declared in `specs/DeviceDiagnostics_Spec.md`:

**`IDeviceDiagnostics.h` (11):**
- `Exchange::IDeviceDiagnostics`
- `Exchange::IDeviceDiagnostics::INotification`
- `Exchange::IDeviceDiagnostics::ParamList`
- `Exchange::IDeviceDiagnostics::AvDecoderStatusResult`
- `Exchange::IDeviceDiagnostics::RebootInfo`
- `Exchange::IDeviceDiagnostics::Register`
- `Exchange::IDeviceDiagnostics::Unregister`
- `Exchange::IDeviceDiagnostics::GetConfiguration`
- `Exchange::IDeviceDiagnostics::GetMilestones`
- `Exchange::IDeviceDiagnostics::LogMilestone`
- `Exchange::IDeviceDiagnostics::GetAVDecoderStatus`

**`plugin/DeviceDiagnostics.h` (4/5):**
- `Plugin::DeviceDiagnostics`
- `Plugin::DeviceDiagnostics::Notification`
- `Plugin::DeviceDiagnostics::Notification::OnAVDecoderStatusChanged`
- `Plugin::DeviceDiagnostics::Notification::Deactivated`

**`plugin/DeviceDiagnostics.cpp` (6):**
- `Plugin::DeviceDiagnostics::DeviceDiagnostics`
- `Plugin::DeviceDiagnostics::~DeviceDiagnostics`
- `Plugin::DeviceDiagnostics::Initialize`
- `Plugin::DeviceDiagnostics::Deinitialize`
- `Plugin::DeviceDiagnostics::Information`
- `Plugin::DeviceDiagnostics::Deactivated`

**`plugin/DeviceDiagnosticsImplementation.h` (2):**
- `Plugin::DeviceDiagnosticsImplementation`
- `Plugin::DeviceDiagnosticsImplementation::Job`

**`plugin/DeviceDiagnosticsImplementation.cpp` (14/16):**
- `Plugin::DeviceDiagnosticsImplementation::DeviceDiagnosticsImplementation`
- `Plugin::DeviceDiagnosticsImplementation::~DeviceDiagnosticsImplementation`
- `Plugin::DeviceDiagnosticsImplementation::Register`
- `Plugin::DeviceDiagnosticsImplementation::Unregister`
- `Plugin::DeviceDiagnosticsImplementation::GetConfiguration`
- `Plugin::DeviceDiagnosticsImplementation::GetMilestones`
- `Plugin::DeviceDiagnosticsImplementation::LogMilestone`
- `Plugin::DeviceDiagnosticsImplementation::GetAVDecoderStatus`
- `Plugin::DeviceDiagnosticsImplementation::getConfig`
- `Plugin::DeviceDiagnosticsImplementation::getMostActiveDecoderStatus`
- `Plugin::DeviceDiagnosticsImplementation::onDecoderStatusChange`
- `Plugin::DeviceDiagnosticsImplementation::AVPollThread`
- `Plugin::DeviceDiagnosticsImplementation::dispatchEvent`
- `Plugin::DeviceDiagnosticsImplementation::Dispatch`

**`plugin/Module.h` (1):**
- `MODULE_NAME`

**`Tests/L1Tests/tests/test_DeviceDiagnostics.cpp` (1):**
- `DeviceDiagnosticsTest`

**`Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp` (2):**
- `DiagnosticsNotificationHandler`
- `AsyncHandlerMock_DevDiag`

---

### 1.3 Orphaned Symbols (not covered by any spec)

| Symbol | File | Line | Reason Not Covered |
|---|---|---|---|
| `Plugin::DeviceDiagnostics::Notification::Activated` | `plugin/DeviceDiagnostics.h` | 59 | Empty `RPC::IRemoteConnection::INotification::Activated` override; not mentioned in spec |
| `writeCurlResponse` | `plugin/DeviceDiagnosticsImplementation.cpp` | 53 | Static file-scope curl write callback; not listed in `## Covered Code` |
| `getFileContent` | `plugin/DeviceDiagnosticsImplementation.cpp` | 413 | File-scope utility function for reading milestone log; not listed in `## Covered Code` |

### 1.4 `// Spec:` Inline Comment Coverage

| File | `// Spec:` Comments Found |
|---|---|
| `plugin/DeviceDiagnostics.cpp` | 0 |
| `plugin/DeviceDiagnosticsImplementation.cpp` | 0 |
| `plugin/DeviceDiagnostics.h` | 0 |
| `plugin/DeviceDiagnosticsImplementation.h` | 0 |
| `IDeviceDiagnostics.h` | 0 |
| **Total** | **0 / 41 symbols** |

No inline `// Spec:` traceability annotations are present. All coverage is derived from `## Covered Code` in the spec file only.

### 1.5 Spec Existence Check

| Referenced Spec | Exists in `specs/` |
|---|---|
| `DeviceDiagnostics_Spec.md` | ✅ Yes |

### 1.6 Spec Completeness Check

| Required Section | Present |
|---|---|
| `## Overview` | ✅ |
| `## Description` | ✅ |
| `## Requirements` | ✅ |

### 1.7 Sub-criterion Scores

| Sub-criterion | Weight | Calculation | Points |
|---|---|---|---|
| Reference Coverage | 20% | 38/41 = 92.7% × 20 | **18.5** |
| Spec Existence | 10% | 1/1 = 100% × 10 | **10.0** |
| Spec Completeness | 5% | 1/1 = 100% × 5 | **5.0** |
| No Orphaned Code | 5% | 38/41 = 92.7% × 5 | **4.6** |
| **Subtotal** | **40%** | | **38.1 / 40** |

---

## 2. Architecture HLA Specification — 10.0 / 10

| Sub-criterion | Weight | Score | Evidence |
|---|---|---|---|
| Presence of HLA Spec | 3% | **3/3** | `## Architecture / Design` section present in `DeviceDiagnostics_Spec.md` |
| Clarity of Architecture Diagrams | 3% | **3/3** | ASCII box diagram shows Thunder framework, plugin shell, OOP impl, COM-RPC bridge with clear annotations |
| Component/Module Mapping | 2% | **2/2** | All major components mapped: plugin shell, OOP implementation, `JDeviceDiagnostics` stubs, `EssRMgr`, `libcurl`, `rdk_logger_milestone` |
| Traceability to Code | 2% | **2/2** | Library names (`libWPEFrameworkDeviceDiagnostics.so`), class names, interface filenames, and source files all cross-referenced |

---

## 3. External Interface Specification — 9.5 / 10

| Sub-criterion | Weight | Score | Evidence / Gap |
|---|---|---|---|
| Presence of Interface Spec | 3% | **3/3** | `## External Interfaces` section with JSON-RPC and COM-RPC subsections |
| Defined Inputs/Outputs | 3% | **3/3** | All 4 methods and 1 event have typed parameter tables, direction annotations (`in`/`out`), and enumerated value sets |
| Documentation Completeness | 2% | **2/2** | Full JSON request/response examples, COM-RPC method signatures, error return table, data type definitions, external system dependency table |
| Validation/Examples | 2% | **1.5/2** | JSON examples present for all methods; test case references listed — but no verified/passing test result artifacts or curl-tested example outputs linked |

**Gap:** Validation examples for the interfaces lack evidence of having been executed against a real device or mock server.

---

## 4. Security Specification — 6.5 / 10

| Sub-criterion | Weight | Score | Evidence / Gap |
|---|---|---|---|
| Presence of Security Spec | 3% | **3/3** | `## Security` section present with 6 identified concerns |
| Threat Model/Analysis | 3% | **2/3** | Risks identified (no auth on daemon, no input validation, no ACL, no length limits) — but no formal threat model (STRIDE, DFD, attack surface diagram) |
| Security Requirements | 2% | **1.5/2** | Security observations are present but not formalized as SHALL/MUST requirements with acceptance criteria |
| Validation/Testing | 2% | **0/2** | No security test cases, no static analysis results (Coverity/ASAN), no penetration test evidence |

**Gaps:**
- No STRIDE or equivalent threat model
- Security observations not written as verifiable requirements (REQ-DD-SECxxx)
- No security-focused test cases in L1 or L2 suites
- No documented Coverity scan results (see `cov_build.sh` in repo root — not referenced in spec)

---

## 5. Performance Specification — 7.0 / 10

| Sub-criterion | Weight | Score | Evidence / Gap |
|---|---|---|---|
| Presence of Performance Spec | 3% | **3/3** | `## Performance` section present |
| Defined Performance Metrics | 3% | **3/3** | AV poll interval (30s), curl timeout (30s), OOP init timeout (5s), async dispatch model all documented |
| Test Coverage for Performance | 2% | **1/2** | `AV_POLL_TIMEOUT(31)` in `DeviceDiagnostics_L2Test.cpp` implicitly validates one poll cycle — no explicit latency, throughput, or memory benchmarks |
| Results & Validation | 2% | **0/2** | No measured performance data from a device or test environment documented |

**Gaps:**
- No explicit performance test cases in L1 or L2 suites (only an implicit poll cycle timeout)
- No documented performance targets for `getConfiguration` curl response time
- No memory or CPU profiling results

---

## 6. Versioning & Compatibility — 8.0 / 10

| Sub-criterion | Weight | Score | Evidence / Gap |
|---|---|---|---|
| Presence of Versioning Spec | 3% | **3/3** | `## Versioning & Compatibility` section present |
| Versioning Scheme Defined | 3% | **3/3** | Semver plugin version `1.1.2`, JSON interface version `1.0.0`, C++11 minimum — all documented |
| Backward/Forward Compatibility | 2% | **2/2** | `RebootInfo` forward-compatibility note; `ENABLE_ERM`/`RDK_LOG_MILESTONE` reduced-functionality compat notes |
| Migration/Upgrade Path | 2% | **0/2** | No migration procedures, upgrade checklist, or breaking-change history documented |

**Gap:** No documentation of how consumers should handle a Major or Minor version bump, or what constitutes a breaking change.

---

## 7. Conformance Testing & Validation — 7.5 / 10

| Sub-criterion | Weight | Score | Evidence / Gap |
|---|---|---|---|
| Presence of Conformance Tests | 3% | **3/3** | L1 tests (3 test cases in `test_DeviceDiagnostics.cpp`) and L2 tests (11 test cases in `DeviceDiagnostics_L2Test.cpp`) present |
| Test Coverage | 3% | **2.5/3** | L2 covers all 4 JSON-RPC methods + COM-RPC variants + event notification. L1 is sparse: only `RegisterMethod`, `getConfiguration`, `getAVDecoderStatus` — no L1 tests for `getMilestones` or `logMilestone` |
| Test Documentation | 2% | **2/2** | Test files, frameworks, fixtures, and expected scenarios documented in spec's `## Conformance Testing & Validation` section |
| Validation Results | 2% | **0/2** | No pass/fail records, CI pipeline artifacts, or test execution logs attached to or linked from the spec |

**Gaps:**
- `getMilestones` has no L1 test case
- `logMilestone` has no L1 test case
- No CI result artifacts or test run evidence linked

### Detected L1 Test Cases

| Test Case | Method Under Test |
|---|---|
| `TEST_F(DeviceDiagnosticsTest, RegisterMethod)` | `Register` / notification subscription |
| `TEST_F(DeviceDiagnosticsTest, getConfiguration)` | `GetConfiguration` |
| `TEST_F(DeviceDiagnosticsTest, getAVDecoderStatus)` | `GetAVDecoderStatus` |

### Detected L2 Test Cases

| Test Case | Method Under Test | Transport |
|---|---|---|
| `LogMilestone_JSONRPC` | `LogMilestone` | JSON-RPC |
| `IDLE_GetAVDecoderStatus_JSONRPC` | `GetAVDecoderStatus` (IDLE state) | JSON-RPC |
| `ACTIVE_GetAVDecoderStatus_JSONRPC` | `GetAVDecoderStatus` (ACTIVE state) | JSON-RPC |
| `GetMilestones_JSONRPC` | `GetMilestones` | JSON-RPC |
| `GetConfiguration_JSONRPC` | `GetConfiguration` | JSON-RPC |
| `LogMilestone_COMRPC` | `LogMilestone` | COM-RPC |
| `IDLE_GetAVDecoderStatus_COMRPC` | `GetAVDecoderStatus` (IDLE state) | COM-RPC |
| `ACTIVE_GetAVDecoderStatus_COMRPC` | `GetAVDecoderStatus` (ACTIVE state) | COM-RPC |
| `PAUSED_GetAVDecoderStatus_COMRPC` | `GetAVDecoderStatus` (PAUSED state) | COM-RPC |
| `GetConfiguration_COMRPC` | `GetConfiguration` | COM-RPC |
| `GetMilestones_COMRPC` | `GetMilestones` | COM-RPC |

---

## Gaps Summary Table

| ID | Severity | Category | Gap Description | Recommended Action |
|---|---|---|---|---|
| G-1 | Medium | Code Coverage | `Notification::Activated` not in `## Covered Code` | Add to `Covered Code` section in `DeviceDiagnostics_Spec.md` |
| G-2 | Medium | Code Coverage | `writeCurlResponse` not in `## Covered Code` | Add to `Covered Code` section in `DeviceDiagnostics_Spec.md` |
| G-3 | Medium | Code Coverage | `getFileContent` not in `## Covered Code` | Add to `Covered Code` section in `DeviceDiagnostics_Spec.md` |
| G-4 | Medium | Code Coverage | No `// Spec: DeviceDiagnostics_Spec` inline comments in any source file | Add inline spec annotations to key methods in `.cpp`/`.h` files for bidirectional traceability |
| G-5 | High | Security | No formal threat model (STRIDE or equivalent) | Add threat model section or sub-section under `## Security` |
| G-6 | High | Security | Security observations not formalized as verifiable requirements | Add REQ-DD-SEC-xxx requirements with acceptance criteria |
| G-7 | High | Security | No security test cases | Add at least one security test (oversized input, empty marker, property name injection) |
| G-8 | Medium | Security | `cov_build.sh` Coverity scan script exists in repo but no scan results referenced in spec | Reference Coverity scan results or add link to CI scan artifacts in `## References` |
| G-9 | Medium | Performance | No explicit performance test cases | Add benchmark test for `getConfiguration` round-trip latency and AV poll cycle timing |
| G-10 | Medium | Performance | No measured performance results documented | Document actual device measurements or set explicit SLA targets (e.g., "`getConfiguration` MUST respond within 5 seconds under normal conditions") |
| G-11 | Medium | Versioning | No migration or upgrade path documented | Add migration section describing breaking vs. non-breaking changes and upgrade procedures |
| G-12 | Medium | Conformance | L1 tests missing for `getMilestones` and `logMilestone` | Add `TEST_F(DeviceDiagnosticsTest, getMilestones)` and `TEST_F(DeviceDiagnosticsTest, logMilestone)` |
| G-13 | Medium | Conformance | No CI test result artifacts linked in spec | Link to CI pipeline results or add a test status table in `## Conformance Testing & Validation` |
| G-14 | Low | External Interface | Validation examples not verified against device or mock | Run JSON-RPC examples with curl and record actual responses in spec |

---

## Spec File Checklist

| Check | Status |
|---|---|
| Spec exists at `specs/DeviceDiagnostics_Spec.md` | ✅ |
| `## Overview` present | ✅ |
| `## Description` present | ✅ |
| `## Requirements` present | ✅ |
| `## Architecture / Design` present | ✅ |
| `## External Interfaces` present | ✅ |
| `## Performance` present | ✅ |
| `## Security` present | ✅ |
| `## Versioning & Compatibility` present | ✅ |
| `## Conformance Testing & Validation` present | ✅ |
| `## Covered Code` present | ✅ |
| `## Open Queries` present | ✅ |
| `## References` present | ✅ |
| `## Change History` present | ✅ |
| All 3 orphaned symbols added to `## Covered Code` | ❌ (G-1, G-2, G-3) |
| `// Spec:` inline comments in source code | ❌ (G-4) |
| Security threat model present | ❌ (G-5) |
| Formal security requirements (REQ-DD-SEC-xxx) | ❌ (G-6) |
| Security test cases present | ❌ (G-7) |
| Performance benchmark results documented | ❌ (G-10) |
| Migration/upgrade path documented | ❌ (G-11) |
| L1 tests for `getMilestones` and `logMilestone` | ❌ (G-12) |
| CI test result artifacts linked | ❌ (G-13) |

---

## Improvement Roadmap

Resolving the gaps below would increase the total score to approximately **97 / 100**:

| Action | Points Gained |
|---|---|
| Add 3 orphaned symbols to `## Covered Code` + add `// Spec:` comments | +1.4 |
| Add formal threat model (STRIDE) + security requirements | +2.0 |
| Add security test cases | +2.0 |
| Add performance benchmark test + documented results | +3.0 |
| Add migration/upgrade path section | +2.0 |
| Add L1 tests for `getMilestones` and `logMilestone` | +0.5 |
| Link CI test results | +1.5 |
| **Estimated new total** | **≈ 97 / 100** |

---

## Change History

- [2026-04-27] - openspec-coverage - Initial compliance report generated for `DeviceDiagnostics_Spec.md`.
