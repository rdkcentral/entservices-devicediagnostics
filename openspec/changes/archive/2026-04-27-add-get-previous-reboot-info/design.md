## Context

The `DeviceDiagnostics` Thunder plugin currently exposes four JSON-RPC methods (`getAVDecoderStatus`, `getConfiguration`, `getMilestones`, `logMilestone`) via a COM-RPC split-architecture (plugin shell + implementation library running in-process within Thunder). The `Exchange::IDeviceDiagnostics` interface already defines a `RebootInfo` struct with every field needed (`timestamp`, `source`, `reason`, `customReason`, `otherReason`, and `lastHardPowerReset`), but no JSON-RPC method currently exposes it (tracked as OQ-01 in the spec).

RDK devices write reboot provenance to two well-known files after every boot:
- `/opt/secure/reboot/previousreboot.info` — key=value pairs: `reboot_timestamp`, `reboot_source`, `reboot_reason`, `reboot_custom_reason`, `reboot_other_reason`
- `/opt/secure/reboot/hardpower.info` — single line: last hard-power-reset timestamp string

The reference implementation for this file-reading pattern exists in `SystemServices::getPreviousRebootInfo2()` in the enterprise services codebase.

## Goals / Non-Goals

**Goals:**
- Add `GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success)` to `Exchange::IDeviceDiagnostics` (interface change in external `entservices-apis` repo).
- Implement the method in `DeviceDiagnosticsImplementation.cpp`, reading the two reboot info files.
- Auto-generate the `getPreviousRebootInfo` JSON-RPC stub via `Exchange::JDeviceDiagnostics` (no manual `registerMethod` call needed).
- Return `Core::ERROR_NONE` with `success: true` and populated `RebootInfo` on success.
- Return `Core::ERROR_GENERAL` with `success: false` when either required file is missing or unreadable.
- Update `DeviceDiagnostics_Spec.md` with the new method, request/response schema, COM-RPC signature, and file dependencies.

**Non-Goals:**
- Modifying the Thunder plugin shell (`DeviceDiagnostics.h` / `.cpp`) — the COM-RPC auto-stub registration handles routing automatically.
- Parsing the reboot files in the plugin shell; all logic stays in the implementation.
- Caching reboot info in memory across calls — the files are read on every invocation (they change each reboot).
- Watching for file changes or emitting a notification event when reboot info appears — synchronous query only.
- Changing the on-disk file format of `/opt/secure/reboot/previousreboot.info` or `hardpower.info`.

## Decisions

### D-1: Read from files, not IARM/IPC
**Decision:** Read `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info` directly via `std::ifstream`, the same way `SystemServices::getPreviousRebootInfo2()` does.

**Alternatives considered:**
- IARM bus call to SYSMgr for reboot reason — adds IARM dependency to DeviceDiagnostics which currently has none; adds coupling to SYSMgr uptime.
- RFC parameter read — data is not stored in RFC; RFC is for configuration, not event history.

**Rationale:** File-based read is the established RDK pattern, dependency-free, and consistent with the existing `getMilestones` file-read approach in the same implementation file.

---

### D-2: Reuse existing `RebootInfo` struct, no new types
**Decision:** Use the `RebootInfo` struct already declared in `Exchange::IDeviceDiagnostics`. No new types, iterators, or namespaces.

**Rationale:** The struct is already `@json`-annotated (or will be upon interface update); all six fields map 1:1 to the spec-defined response fields. Introducing a parallel struct would be redundant.

---

### D-3: `@json` annotation on interface method drives stub generation
**Decision:** The `GetPreviousRebootInfo` method in `IDeviceDiagnostics.h` carries the same `@json 1.0.0` annotation as the existing interface. The auto-generated `JDeviceDiagnostics` class will produce the `getPreviousRebootInfo` JSON-RPC stub.

**Rationale:** Manual `registerMethod` is explicitly prohibited by the plugin coding guidelines (REQ-DD-011); all JSON-RPC wiring must go through `JDeviceDiagnostics`.

---

### D-4: File parsing strategy — key=value line scan
**Decision:** Parse `/opt/secure/reboot/previousreboot.info` line-by-line, splitting on `=`, matching known keys (`reboot_timestamp`, `reboot_source`, `reboot_reason`, `reboot_custom_reason`, `reboot_other_reason`). Parse `/opt/secure/reboot/hardpower.info` as a single trimmed line into `lastHardPowerReset`.

**Rationale:** The file format is a simple properties file (same as `DEVICE_PROPERTIES_FILE` processed by `GetValueFromPropertiesFile` in SystemServices). A full JSON parser is unnecessary and would add a dependency for a trivial structure.

---

### D-5: Error semantics
**Decision:** If `/opt/secure/reboot/previousreboot.info` does not exist or cannot be opened, return `Core::ERROR_GENERAL` / `success: false`. Partial reads (file exists but some keys missing) return whatever is populated with `success: false` only if the file could not be read at all.

**Rationale:** Callers need a reliable signal when data is genuinely unavailable. Partial population with `success: true` would mislead SIFT diagnostics.

## Risks / Trade-offs

| Risk | Mitigation |
|---|---|
| `/opt/secure/reboot/previousreboot.info` may not exist on fresh boot (no prior reboot) | Return `Core::ERROR_GENERAL` / `success: false`; callers must handle missing data gracefully |
| File key names differ across RDK platform variants | Test against known file samples; maintain unit test fixture files covering all expected key names |
| `hardpower.info` may be absent on platforms without hard-power-reset tracking | Treat absence as empty string for `lastHardPowerReset`; `success` is still driven by `previousreboot.info` presence |
| Interface change (`IDeviceDiagnostics.h`) is in a separate repo (`entservices-apis`) | Interface PR must land and be picked up before this implementation builds; tracked as task dependency |
| Auto-generated stub (`JDeviceDiagnostics`) regeneration requires re-running the JSON-RPC code generator after interface change | Regenerate and commit updated `JDeviceDiagnostics.h` as part of the interface PR; document in tasks |

## Open Questions

- **OQ-1**: What is the exact key name for the timestamp field in `/opt/secure/reboot/previousreboot.info`? Assumed `reboot_timestamp` based on `getPreviousRebootInfo2` reference. Confirm with platform team.
- **OQ-2**: Should `lastHardPowerReset` absence (no `hardpower.info`) return an empty string or a sentinel value? Current decision: empty string, `success` not affected.
- **OQ-3**: Is there a maximum file size concern for `/opt/secure/reboot/previousreboot.info`? On all known RDK platforms it is <1 KB; no streaming/chunking needed.
