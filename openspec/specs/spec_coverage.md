# OpenSpec Coverage Report — entservices-devicediagnostics

**Generated:** 2026-04-28 (v2 — updated after templater pass)
**Skill:** openspec-coverage  
**Spec scanned:** `openspec/specs/device_diagnostics_spec.md`  
**`// Spec:` comment signals found:** None

---

## Overall Score

```
┌─────────────────────────────────────────────────────────────┐
│            OPENSPEC COMPLIANCE SCORE:  83 / 100             │
│            (↑ +4 from previous run: 79 → 83)                │
├──────────────────────────────────────────┬────────┬─────────┤
│ Category                                 │ Score  │   Max   │
├──────────────────────────────────────────┼────────┼─────────┤
│ Code to Spec Coverage                    │  40    │   40    │
│ Architecture HLA Specification           │  10    │   10    │
│ External Interface Specification         │   9    │   10    │
│ Security Specification                   │   7    │   10    │
│ Versioning & Compatibility               │   6    │   10    │
│ Conformance Testing & Validation         │   6    │   10    │
│ Performance Specification                │   5    │   10    │
├──────────────────────────────────────────┼────────┼─────────┤
│ TOTAL                                    │  83    │  100    │
└──────────────────────────────────────────┴────────┴─────────┘
```

---

## 1. Code to Spec Coverage — 40 / 40 ✅

### Method Inventory

| File | Total Methods | Covered in Spec | Orphaned |
|---|---|---|---|
| `plugin/DeviceDiagnostics.cpp` | 6 | 6 | 0 |
| `plugin/DeviceDiagnosticsImplementation.cpp` | 15 | 15 | 0 |
| **Total** | **21** | **21** | **0** |

**Coverage rate: 100% (21/21)**

> `// Spec:` comment supplementary signal: 0 methods tagged. No boost needed — primary coverage is already 100%.

### Orphaned Methods

_None — all code methods are covered by `openspec/specs/device_diagnostics_spec.md`._

### Spec Files Status

| Spec File | Exists | Has Overview | Has Description | Has Requirements | Complete |
|---|---|---|---|---|---|
| `openspec/specs/device_diagnostics_spec.md` | ✅ | ✅ | ✅ | ✅ | ✅ |

### Sub-criterion Scores

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Reference Coverage | 20 | 20 | 100% of methods covered (21/21) ✅ |
| Spec Existence | 10 | 10 | `device_diagnostics_spec.md` exists ✅ |
| Spec Completeness | 5 | 5 | All required sections present (Overview, Description, Requirements) ✅ |
| No Orphaned Code | 5 | 5 | 0 orphaned methods ✅ |

---

## 2. Architecture HLA Specification — 10 / 10 ✅

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of HLA spec | 3 | 3 | Dedicated `Architecture / Design (if applicable)` section present ✅ |
| Clarity of diagrams | 3 | 3 | 3 ASCII diagrams: plugin host/OOP split, AV decoder state machine, notification flow ✅ |
| Component/module mapping | 2 | 2 | All major components mapped: plugin host, `DeviceDiagnosticsImplementation`, ERM, loopback HTTP service ✅ |
| Traceability to code | 2 | 2 | Diagrams name concrete methods; `## Covered Code` provides file/method-level traceability ✅ |

---

## 3. External Interface Specification — 9 / 10

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of interface spec | 3 | 3 | COM-RPC interface tree + JSON-RPC API table present ✅ |
| Defined inputs/outputs | 3 | 3 | All 4 methods + 1 event have parameters, return types, and data types documented ✅ |
| Documentation completeness | 2 | 2 | Full method list, callsign, event payload, and backend protocol all covered ✅ |
| Validation/examples | 1 | 2 | No embedded JSON-RPC request/response examples in spec; examples exist in `DeviceDiagnostics.md` but not referenced from the interface spec section |

**Gap:** Embed or link concrete JSON-RPC request/response examples directly in the `## External Interfaces` section.

---

## 4. Security Specification — 7 / 10

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of security spec | 3 | 3 | Dedicated `Security (if applicable)` section present ✅ |
| Threat model/analysis | 2 | 3 | Notes on SSRF non-applicability, marker injection risk, log output trust — no formal STRIDE table or threat register |
| Security requirements | 2 | 2 | SHOULD-level requirement for marker length limit; consumer trust responsibility noted ✅ |
| Validation/testing | 0 | 2 | No security-specific tests; no evidence of security validation or audit |

**Gaps:**
- No formal threat model (STRIDE or equivalent)
- No security tests (e.g., fuzz `LogMilestone` with oversized/special-character markers)
- No security audit or penetration test results documented

---

## 5. Versioning & Compatibility — 6 / 10

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of versioning spec | 3 | 3 | `Versioning & Compatibility (if applicable)` section present with plugin and interface versions ✅ |
| Versioning scheme defined | 2 | 3 | Plugin uses `MAJOR.MINOR.PATCH` (1.1.2), interface uses `@json 1.0.0` — scheme not explicitly named as semver or defined in terms of breaking changes |
| Backward/forward compatibility | 1 | 2 | Notes that interface changes require stub regeneration; no explicit compatibility guarantee or policy stated |
| Migration/upgrade path | 0 | 2 | No migration or upgrade guidance documented |

**Gaps:**
- Explicitly name and define the versioning scheme (semver: what constitutes a MAJOR vs MINOR vs PATCH change)
- Document a backward compatibility policy
- Add migration/upgrade path section (even if: "no breaking changes policy for 1.x")

---

## 6. Conformance Testing & Validation — 6 / 10

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of conformance tests | 3 | 3 | L1 (mock/unit) + L2 (integration, JSON-RPC + COM-RPC) test files referenced ✅ |
| Test coverage | 2 | 3 | L2 covers all 4 methods via both JSON-RPC and COM-RPC paths. L1 covers 3/5 key scenarios. Missing: `LogMilestone` empty-rejection test, `GetMilestones` missing-file error path |
| Test documentation | 1 | 2 | Spec lists file paths and key scenarios but no build/run instructions or environment requirements |
| Validation results | 0 | 2 | No test results tracked in spec; no pass/fail evidence documented |

**Gaps:**

| Missing Test | Requirement Covered |
|---|---|
| `LogMilestone` with empty `marker` in L1 | "MUST return `success: false` if `marker` is empty" |
| `GetMilestones` when log file does not exist in L1 | "MUST return `success: false` if the log file does not exist" |

- Add test run instructions (build commands, test runner, environment variables, expected output)
- Track validation results in spec or linked CI artifact

---

## 7. Performance Specification — 5 / 10

| Sub-criterion | Score | Max | Notes |
|---|---|---|---|
| Presence of performance spec | 3 | 3 | `Performance (if applicable)` section present ✅ |
| Defined performance metrics | 2 | 3 | Three metrics stated (30s polling interval, 30s curl timeout, no file size limit) but framed as observations rather than formal acceptance criteria with pass/fail thresholds |
| Test coverage for performance | 0 | 2 | No performance tests exist in L1 or L2 test suites |
| Results & validation | 0 | 2 | No benchmarks or measured performance results documented |

**Gaps:**
- Restate observations as formal targets (e.g., "Event delivery latency MUST be ≤ 30 seconds", "GetConfiguration MUST complete within 30 seconds under normal load")
- Add at least one automated performance test or profiling run
- Document a measured baseline on reference hardware

---

## Summary of Gaps & Priority Actions

| Pri | Action | Category | Estimated Score Gain |
|---|---|---|---|
| 1 | Embed JSON-RPC request/response examples in `## External Interfaces` section | External Interface | +1 |
| 2 | Add L1 test: `LogMilestone` with empty marker | Conformance Testing | +0.5 |
| 3 | Add L1 test: `GetMilestones` when log file is missing | Conformance Testing | +0.5 |
| 4 | Add test run instructions (how to build/run L1, L2) | Conformance Testing | +1 |
| 5 | Define semver semantics (MAJOR/MINOR/PATCH policy) | Versioning | +1 |
| 6 | Document backward compatibility policy and migration guidance | Versioning | +2 |
| 7 | Add formal threat model (STRIDE table) | Security | +1 |
| 8 | Add security test for `LogMilestone` marker fuzzing | Security | +1 |
| 9 | Convert performance observations to formal acceptance criteria | Performance | +1 |
| 10 | Add at least one performance test or profiling result | Performance | +2 |

**Projected score if all actions addressed: ~94 / 100**

---

## Spec Inventory

| Spec | Path | Status |
|---|---|---|
| DeviceDiagnostics Plugin Spec | `openspec/specs/device_diagnostics_spec.md` | ✅ Complete |

---

## Coverage Map

```
plugin/DeviceDiagnostics.cpp
  ├── DeviceDiagnostics()               ← covered: device_diagnostics_spec
  ├── ~DeviceDiagnostics()              ← covered: device_diagnostics_spec
  ├── Initialize()                      ← covered: device_diagnostics_spec
  ├── Deinitialize()                    ← covered: device_diagnostics_spec
  ├── Information()                     ← covered: device_diagnostics_spec
  └── Deactivated()                     ← covered: device_diagnostics_spec

plugin/DeviceDiagnosticsImplementation.cpp
  ├── DeviceDiagnosticsImplementation() ← covered: device_diagnostics_spec
  ├── ~DeviceDiagnosticsImplementation()← covered: device_diagnostics_spec
  ├── Register()                        ← covered: device_diagnostics_spec
  ├── Unregister()                      ← covered: device_diagnostics_spec
  ├── dispatchEvent()                   ← covered: device_diagnostics_spec
  ├── Dispatch()                        ← covered: device_diagnostics_spec
  ├── getMostActiveDecoderStatus()      ← covered: device_diagnostics_spec
  ├── AVPollThread()                    ← covered: device_diagnostics_spec
  ├── onDecoderStatusChange()           ← covered: device_diagnostics_spec
  ├── GetConfiguration()                ← covered: device_diagnostics_spec
  ├── GetMilestones()                   ← covered: device_diagnostics_spec
  ├── LogMilestone()                    ← covered: device_diagnostics_spec
  ├── GetAVDecoderStatus()              ← covered: device_diagnostics_spec
  ├── getConfig()                       ← covered: device_diagnostics_spec
  └── getFileContent()                  ← covered: device_diagnostics_spec
```

Orphaned methods: **none**

---

*Generated by openspec-coverage skill — 2026-04-28 (v2)*
