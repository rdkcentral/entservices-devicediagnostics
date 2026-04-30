## 1. Declare New Method in Header

- [x] 1.1 In `plugin/DeviceDiagnosticsImplementation.h`, add the public method declaration:
      `Core::hresult GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success) override;`
- [x] 1.2 In `plugin/DeviceDiagnosticsImplementation.cpp`, add `#include <sstream>`.
- [x] 1.3 Define the file path constants near the top of `DeviceDiagnosticsImplementation.cpp`:
      ```cpp
      #define PREVIOUS_REBOOT_INFO_FILE "/opt/secure/reboot/previousreboot.info"
      #define HARD_POWER_INFO_FILE      "/opt/secure/reboot/hardpower.info"
      ```
- [x] 1.4 Forward-declare the string overload of `getFileContent` in `DeviceDiagnosticsImplementation.cpp`:
      `bool getFileContent(std::string fileName, std::string& fileContent);`

## 2. Implement `getFileContent` String Overload

- [x] 2.1 At the bottom of `DeviceDiagnosticsImplementation.cpp` (after the namespace), implement the overloaded free function `getFileContent(std::string, std::string&)` using `std::ifstream` and `std::stringstream` to read the full file content into a string.
- [x] 2.2 Return `false` if the file cannot be opened; return `true` on success.

## 3. Implement `GetPreviousRebootInfo`

- [x] 3.1 Use `Core::File(string(PREVIOUS_REBOOT_INFO_FILE)).Exists()` to check for the primary file; if missing, set `success = false` and return `Core::ERROR_GENERAL`.
- [x] 3.2 Use `Core::File(string(HARD_POWER_INFO_FILE)).Exists()` to check for the hardpower file; if missing, set `success = false` and return `Core::ERROR_GENERAL`.
- [x] 3.3 Call `getFileContent(PREVIOUS_REBOOT_INFO_FILE, rebootInfoContent)`; if it returns `false` or content is empty, set `success = false` and return `Core::ERROR_GENERAL`.
- [x] 3.4 Parse `rebootInfoContent` with `JsonObject::FromString()`; if parse fails, return `Core::ERROR_GENERAL`.
- [x] 3.5 Extract `timestamp`, `source`, `reason`, `customReason`, `otherReason` from the parsed JSON object.
- [x] 3.6 Call `getFileContent(HARD_POWER_INFO_FILE, hardPowerInfo)`; if it returns `false` or content is empty, return `Core::ERROR_GENERAL`.
- [x] 3.7 Parse `hardPowerInfo` with `JsonObject::FromString()`; if parse fails, return `Core::ERROR_GENERAL`.
- [x] 3.8 Extract `lastHardPowerReset` from the hardpower JSON object.
- [x] 3.9 Assign all fields to `rebootInfo` using `std::move`, set `success = true`, and return `Core::ERROR_NONE`.

## 4. Verify JSON-RPC Auto-Registration

- [x] 4.1 Confirm that `Exchange::JDeviceDiagnostics` (auto-generated from the updated interface) exposes `getPreviousRebootInfo` after the interface update is applied.
- [x] 4.2 Verify no manual `Register()` calls are needed in `DeviceDiagnostics.cpp` for this new method.

## 5. Build Validation

- [x] 5.1 Build the plugin and verify no compile errors related to `GetPreviousRebootInfo` or the `getFileContent` overload.
- [x] 5.2 Verify the `RebootInfo` struct and `GetPreviousRebootInfo` symbol are resolved from the Exchange interface headers.

## 6. Testing

- [x] 6.1 Add L1 test `GetPreviousRebootInfo_Success_AllFields`: both files exist with valid JSON and all fields present — expect `Core::ERROR_NONE` and all fields populated.
- [x] 6.2 Add L1 test `GetPreviousRebootInfo_HardPowerFileMissing`: primary file exists, `hardpower.info` missing — expect `Core::ERROR_GENERAL`.
- [x] 6.3 Add L1 test `GetPreviousRebootInfo_PrimaryFileMissing`: neither file exists — expect `Core::ERROR_GENERAL`.
- [x] 6.4 Add L1 test `GetPreviousRebootInfo_InvalidPrimaryJSON`: primary file contains invalid JSON — expect `Core::ERROR_GENERAL`.
- [x] 6.5 Add L1 test `GetPreviousRebootInfo_InvalidHardPowerJSON`: hardpower file contains invalid JSON — expect `Core::ERROR_GENERAL`.
- [x] 6.6 Add L1 test `GetPreviousRebootInfo_MissingFields`: both files valid JSON but some fields absent — expect `Core::ERROR_NONE`, missing fields return empty strings.
- [x] 6.7 Add L1 test `GetPreviousRebootInfo_EmptyPrimaryFile`: primary file exists but is empty — expect `Core::ERROR_GENERAL`.
