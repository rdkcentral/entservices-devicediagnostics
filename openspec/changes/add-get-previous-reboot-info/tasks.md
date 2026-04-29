## 1. Declare New Method and Helper in Header

- [ ] 1.1 In `plugin/DeviceDiagnosticsImplementation.h`, add the public method declaration:
      `Core::hresult GetPreviousRebootInfo(Exchange::IDeviceDiagnostics::RebootInfo& rebootInfo, bool& success) override;`
- [ ] 1.2 In `plugin/DeviceDiagnosticsImplementation.h`, add the private helper declaration:
      `bool GetFileContent(const string& filePath, string& content);`
- [ ] 1.3 In `plugin/DeviceDiagnosticsImplementation.cpp`, add `#include <regex>` if not already present.
- [ ] 1.4 Define the file path constants near the top of `DeviceDiagnosticsImplementation.cpp`:
      ```cpp
      #define PREVIOUS_REBOOT_INFO_FILE "/opt/secure/reboot/previousreboot.info"
      #define HARD_POWER_INFO_FILE      "/opt/secure/reboot/hardpower.info"
      ```

## 2. Implement `GetFileContent` Helper

- [ ] 2.1 In `DeviceDiagnosticsImplementation.cpp`, implement `GetFileContent` using `std::ifstream` and `std::istreambuf_iterator` to read the full file content into a string.
- [ ] 2.2 Return `false` if the file cannot be opened; return `true` on success.

## 3. Implement `GetPreviousRebootInfo`

- [ ] 3.1 Use `Core::File` to check existence of `PREVIOUS_REBOOT_INFO_FILE`; if missing, set `success = false` and return `Core::ERROR_GENERAL`.
- [ ] 3.2 Call `GetFileContent(PREVIOUS_REBOOT_INFO_FILE, content)`; if it returns `false`, set `success = false` and return `Core::ERROR_GENERAL`.
- [ ] 3.3 Use `std::regex_search` to extract `PreviousRebootTime:` → `rebootInfo.timestamp`.
- [ ] 3.4 Use `std::regex_search` to extract `PreviousRebootReason:` → `rebootInfo.reason`.
- [ ] 3.5 Use `std::regex_search` to extract `PreviousRebootInitiatedBy:` → `rebootInfo.source`.
- [ ] 3.6 Use `std::regex_search` to extract `PreviousCustomReason:` → `rebootInfo.customReason`.
- [ ] 3.7 Use `std::regex_search` to extract `PreviousOtherReason:` → `rebootInfo.otherReason`.
- [ ] 3.8 Use `Core::File` to check existence of `HARD_POWER_INFO_FILE`; if present, call `GetFileContent` and read the first non-empty line into `rebootInfo.lastHardPowerReset`.
- [ ] 3.9 Set `success = true` and return `Core::ERROR_NONE`.

## 4. Verify JSON-RPC Auto-Registration

- [ ] 4.1 Confirm that `Exchange::JDeviceDiagnostics` (auto-generated from the updated interface) exposes `getPreviousRebootInfo` after the interface update is applied.
- [ ] 4.2 Verify no manual `Register()` calls are needed in `DeviceDiagnostics.cpp` for this new method.

## 5. Build Validation

- [ ] 5.1 Build the plugin and verify no compile errors related to `GetPreviousRebootInfo` or `GetFileContent`.
- [ ] 5.2 Verify the `RebootInfo` struct and `GetPreviousRebootInfo` symbol are resolved from the Exchange interface headers.

## 6. Testing

- [ ] 6.1 Add an L1 unit test in `Tests/L1Tests/tests/test_DeviceDiagnostics.cpp` for `GetPreviousRebootInfo` when both files exist and all fields are present.
- [ ] 6.2 Add an L1 unit test for `GetPreviousRebootInfo` when `previousreboot.info` is missing (expect `Core::ERROR_GENERAL`).
- [ ] 6.3 Add an L1 unit test for `GetPreviousRebootInfo` when `hardpower.info` is missing (expect `success: true`, `lastHardPowerReset` empty).
- [ ] 6.4 Add an L1 unit test for `GetFileContent` with a valid file (expect `true` and correct content).
- [ ] 6.5 Add an L1 unit test for `GetFileContent` with a non-existent file (expect `false`).
