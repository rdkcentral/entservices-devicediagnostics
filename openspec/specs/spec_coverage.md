# OpenSpec Coverage Report — `entservices-devicediagnostics`

**Generated:** 2026-04-27 (updated)
**Skill:** openspec-coverage

---

## Summary

| Category | Score | Max | Δ vs Previous |
|---|---|---|---|
| Code to Spec Coverage | 40 | 40 | **+40** ✓ |
| Architecture HLA Specification | 9 | 10 | — |
| External Interface Specification | 9 | 10 | — |
| Security Specification | 2 | 10 | — |
| Versioning & Compatibility Specification | 2 | 10 | — |
| Performance Specification | 3 | 10 | — |
| Conformance Testing Automation & Validation | 6 | 10 | — |
| **TOTAL** | **71** | **100** | **+40** |

---

## 1. Code to Spec Coverage — 40 / 40

### Signal Sources

| Signal | Count | Notes |
|---|---|---|
| `## Covered Code` entries in specs | 52 entries across 7 files | `openspec/specs/spec.md` — fully populated |
| `// Spec: <name>` comments in code | 0 | No such comments found in any plugin or test file |

### Spec Inventory

| Spec File | Exists | Has Overview | Has Description | Has Requirements |
|---|---|---|---|---|
| `openspec/specs/spec.md` | ✓ | ✓ | ✓ | ✓ |

### Method Coverage

**Total tracked methods/functions: 44**
**Covered by spec `## Covered Code`: 44 (100%)**
**Orphaned: 0 (0%)**

#### plugin/DeviceDiagnostics.cpp (4/4 covered ✓)
- `DeviceDiagnostics::Initialize` ✓
- `DeviceDiagnostics::Deinitialize` ✓
- `DeviceDiagnostics::Information` ✓
- `DeviceDiagnostics::Deactivated` ✓

#### plugin/DeviceDiagnostics.h (3/3 covered ✓)
- `DeviceDiagnostics::Notification::Activated` ✓
- `DeviceDiagnostics::Notification::Deactivated` ✓
- `DeviceDiagnostics::Notification::OnAVDecoderStatusChanged` ✓

#### plugin/DeviceDiagnosticsImplementation.cpp (15/15 covered ✓)
- `DeviceDiagnosticsImplementation::DeviceDiagnosticsImplementation` ✓
- `DeviceDiagnosticsImplementation::~DeviceDiagnosticsImplementation` ✓
- `DeviceDiagnosticsImplementation::Register` ✓
- `DeviceDiagnosticsImplementation::Unregister` ✓
- `DeviceDiagnosticsImplementation::dispatchEvent` ✓
- `DeviceDiagnosticsImplementation::Dispatch` ✓
- `DeviceDiagnosticsImplementation::getMostActiveDecoderStatus` ✓
- `DeviceDiagnosticsImplementation::AVPollThread` ✓ _(ENABLE_ERM only)_
- `DeviceDiagnosticsImplementation::onDecoderStatusChange` ✓
- `DeviceDiagnosticsImplementation::GetConfiguration` ✓
- `DeviceDiagnosticsImplementation::GetMilestones` ✓
- `DeviceDiagnosticsImplementation::LogMilestone` ✓
- `DeviceDiagnosticsImplementation::GetAVDecoderStatus` ✓
- `DeviceDiagnosticsImplementation::getConfig` ✓
- `getFileContent` ✓

#### plugin/DeviceDiagnosticsImplementation.h (1/1 covered ✓)
- `DeviceDiagnosticsImplementation::Job::Dispatch` ✓

#### IDeviceDiagnostics.h (7/7 covered ✓)
- `Exchange::IDeviceDiagnostics::Register` ✓
- `Exchange::IDeviceDiagnostics::Unregister` ✓
- `Exchange::IDeviceDiagnostics::GetConfiguration` ✓
- `Exchange::IDeviceDiagnostics::GetMilestones` ✓
- `Exchange::IDeviceDiagnostics::LogMilestone` ✓
- `Exchange::IDeviceDiagnostics::GetAVDecoderStatus` ✓
- `Exchange::IDeviceDiagnostics::INotification::OnAVDecoderStatusChanged` ✓

#### Tests/L1Tests/tests/test_DeviceDiagnostics.cpp (3/3 covered ✓)
- `DeviceDiagnosticsTest::RegisterMethod` ✓
- `DeviceDiagnosticsTest::getConfiguration` ✓
- `DeviceDiagnosticsTest::getAVDecoderStatus` ✓

#### Tests/L2Tests/tests/DeviceDiagnostics_L2Test.cpp (11/11 covered ✓)
- `DeviceDiagnostics_L2test::LogMilestone_JSONRPC` ✓
- `DeviceDiagnostics_L2test::IDLE_GetAVDecoderStatus_JSONRPC` ✓
- `DeviceDiagnostics_L2test::ACTIVE_GetAVDecoderStatus_JSONRPC` ✓
- `DeviceDiagnostics_L2test::GetMilestones_JSONRPC` ✓
- `DeviceDiagnostics_L2test::GetConfiguration_JSONRPC` ✓
- `DeviceDiagnostics_L2test::LogMilestone_COMRPC` ✓
- `DeviceDiagnostics_L2test::IDLE_GetAVDecoderStatus_COMRPC` ✓
- `DeviceDiagnostics_L2test::ACTIVE_GetAVDecoderStatus_COMRPC` ✓
- `DeviceDiagnostics_L2test::PAUSED_GetAVDecoderStatus_COMRPC` ✓
- `DeviceDiagnostics_L2test::GetConfiguration_COMRPC` ✓
- `DeviceDiagnostics_L2test::GetMilestones_COMRPC` ✓

### Sub-criterion Scores

| Sub-criterion | Score | Max | Calculation |
|---|---|---|---|
| Reference Coverage | 20 | 20 | 44/44 methods covered (100%) × 20 = 20 |
| Spec Existence | 10 | 10 | 1/1 referenced spec exists in `openspec/specs/` = 100% × 10 = 10 |
| Spec Completeness | 5 | 5 | 1/1 spec has Overview + Description + Requirements = 100% × 5 = 5 |
| No Orphaned Code | 5 | 5 | 0/44 orphaned (0% orphaned) = 100% × 5 = 5 |

---

## 2. Architecture HLA Specification — 9 / 10

| Sub-criterion | Score | Max | Evidence |
|---|---|---|---|
| Presence of HLA Spec | 3 | 3 | `ARCHITECTURE.md` present with full overview section |
| Clarity of Architecture Diagrams | 3 | 3 | ASCII diagrams for component structure, data flow, AV state, threading |
| Component/Module Mapping | 2 | 2 | All components mapped: plugin shell, implementation, poll thread, external deps |
| Traceability to Code | 1 | 2 | File names referenced; no method-level traceability from architecture doc |

---

## 3. External Interface Specification — 9 / 10

| Sub-criterion | Score | Max | Evidence |
|---|---|---|---|
| Presence of Interface Spec | 3 | 3 | `IDeviceDiagnostics.h` (COM-RPC) + `DeviceDiagnostics.md` (JSON-RPC) |
| Defined Inputs/Outputs | 3 | 3 | `@text`, `@brief`, `@param` annotations; response tables in `DeviceDiagnostics.md` |
| Documentation Completeness | 1 | 2 | `avDecoderStatus` enum values (`IDLE`/`PAUSED`/`ACTIVE`) absent from doc; `RebootInfo` struct defined in interface but unused/undocumented |
| Validation/Examples | 2 | 2 | `curl` command examples present for all 4 methods in `DeviceDiagnostics.md` |

---

## 4. Security Specification — 2 / 10

| Sub-criterion | Score | Max | Evidence / Gap |
|---|---|---|---|
| Presence of Security Spec | 1 | 3 | Brief mentions in `ARCHITECTURE.md`; no dedicated section or document |
| Threat Model / Analysis | 0 | 3 | No threat model exists |
| Security Requirements | 1 | 2 | Loopback-only HTTP assumption noted in architecture; non-empty `marker` validation in code |
| Validation / Testing | 0 | 2 | No security-focused tests |

**Gaps:**
- No threat model for loopback HTTP endpoint (`localhost:10999`) — trust boundary not formally defined
- No documentation of what happens if the config service is unavailable or spoofed
- No input sanitization beyond empty-string check for `logMilestone`
- No security tests

---

## 5. Versioning & Compatibility Specification — 2 / 10

| Sub-criterion | Score | Max | Evidence / Gap |
|---|---|---|---|
| Presence of Versioning Spec | 1 | 3 | Versions appear in code and interface annotation; no spec document |
| Versioning Scheme Defined | 1 | 3 | Semver used implicitly (`1.1.2` in `DeviceDiagnostics.cpp`); not formally specified |
| Backward/Forward Compatibility | 0 | 2 | Not documented anywhere |
| Migration / Upgrade Path | 0 | 2 | Not documented anywhere |

**Gaps:**
- Version mismatch: plugin registers as `1.1.2`; `IDeviceDiagnostics.h` JSON annotation declares `1.0.0`
- No compatibility guarantees documented across Thunder R4 vs earlier
- No changelog entry format or migration guidance

---

## 6. Performance Specification — 3 / 10

| Sub-criterion | Score | Max | Evidence / Gap |
|---|---|---|---|
| Presence of Performance Spec | 1 | 3 | `ARCHITECTURE.md` has a "Performance Characteristics" subsection |
| Defined Performance Metrics | 2 | 3 | Poll interval (30s), curl timeout (30s), non-blocking dispatch noted — but not as formal targets |
| Test Coverage for Performance | 0 | 2 | No performance tests |
| Results & Validation | 0 | 2 | No tracked benchmark results |

**Gaps:**
- `AVDECODERSTATUS_RETRY_INTERVAL` is a compile-time constant — no runtime configurability or SLA defined
- No latency budget for event delivery after decoder state change
- No memory/CPU usage targets or measurements

---

## 7. Conformance Testing Automation & Validation — 6 / 10

| Sub-criterion | Score | Max | Evidence / Gap |
|---|---|---|---|
| Presence of Conformance Tests | 3 | 3 | L1 (3 tests) + L2 (11 tests) covering all 4 API methods via JSON-RPC and COM-RPC |
| Test Coverage | 2 | 3 | All methods tested; gaps: no negative HTTP failure path (REQ-23), no real ERM integration test |
| Test Documentation | 1 | 2 | `Tests/README.md` exists but minimal; no instructions for running individual suites |
| Validation Results | 0 | 2 | No tracked pass/fail results or CI badge |

**Test coverage map:**

| Requirement | L1 Test | L2 Test |
|---|---|---|
| `getAVDecoderStatus` — IDLE | `getAVDecoderStatus` | `IDLE_GetAVDecoderStatus_JSONRPC/COMRPC` |
| `getAVDecoderStatus` — ACTIVE | — | `ACTIVE_GetAVDecoderStatus_JSONRPC/COMRPC` |
| `getAVDecoderStatus` — PAUSED | — | `PAUSED_GetAVDecoderStatus_COMRPC` |
| `getConfiguration` | `getConfiguration` | `GetConfiguration_JSONRPC/COMRPC` |
| `getMilestones` | — | `GetMilestones_JSONRPC/COMRPC` |
| `logMilestone` | — | `LogMilestone_JSONRPC/COMRPC` |
| `onAVDecoderStatusChanged` event | — | — ❌ No test |
| HTTP failure path (getConfiguration) | — | — ❌ No test |
| ERM-enabled decoder transition | — | — ❌ No test (mock only) |

---

## Orphaned Code — Full List

**None.** All 44 tracked methods are covered by `openspec/specs/spec.md` via the `## Covered Code` section.

---

## Improvement Plan (Priority Order)

| # | Action | Score Impact | Status |
|---|---|---|---|
| 1 | ~~Populate `openspec/specs/spec.md` with `## Covered Code` mapping all 44 methods~~ | ~~**+40 pts**~~ | ✅ Done |
| 2 | Add security section with threat model: loopback HTTP trust boundary, input validation scope | **+8 pts** | Open |
| 3 | Formalize versioning spec: semver declaration, resolve 1.1.2 vs 1.0.0 mismatch, compatibility guarantees | **+8 pts** | Open |
| 4 | Add performance targets and at least one benchmark test (event dispatch latency) | **+7 pts** | Open |
| 5 | Add missing conformance tests: `onAVDecoderStatusChanged` event, `getConfiguration` HTTP failure path | **+1 pt** | Open |
| 6 | Document test run instructions and track CI results | **+1 pt** | Open |

**Projected score after all remaining actions: ~95 / 100**

---

## Change History

- [2026-04-27] - openspec-coverage - Initial coverage report generated from codebase scan. Score: 31/100.
- [2026-04-27] - openspec-coverage - Updated after `spec.md` fully populated with `## Covered Code` (52 entries, 44 methods). Score: 71/100.
