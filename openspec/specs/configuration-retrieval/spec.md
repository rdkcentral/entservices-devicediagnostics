# Configuration Retrieval Specification

## Overview

The Configuration Retrieval capability enables clients to query device configuration parameters through the DeviceDiagnostics plugin. It acts as a proxy between Thunder JSON-RPC clients and a backend HTTP configuration service.

**Target Devices:** RDK TV and STB platforms  
**Version:** 1.0.0  
**Status:** Active

## Purpose

Provides a standardized interface for retrieving device-specific configuration values without requiring clients to directly access platform-specific configuration backends. This abstraction simplifies configuration management across different RDK device models.

## API Contract

### Method: `getConfiguration`

Retrieves configuration values for specified parameter names.

**Request:**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "org.rdk.DeviceDiagnostics.getConfiguration",
  "params": {
    "names": ["param1", "param2", "paramN"]
  }
}
```

**Response (Success):**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "paramList": [
      {"name": "param1", "value": "value1"},
      {"name": "param2", "value": "value2"}
    ],
    "success": true
  }
}
```

**Response (Failure):**
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "paramList": [],
    "success": false
  }
}
```

## Functional Requirements

### FR-1: Parameter Validation
- **Requirement:** The plugin MUST accept an array of configuration parameter names
- **Behavior:** Empty parameter names are valid and will be forwarded to the backend service
- **Rationale:** Allows backend service to handle parameter validation and defaults

### FR-2: Backend Communication
- **Requirement:** The plugin MUST forward parameter requests to `http://127.0.0.1:10999` via HTTP POST
- **Request Format:** 
  ```json
  {
    "paramList": [
      {"name": "param1"},
      {"name": "param2"}
    ]
  }
  ```
- **Timeout:** 30 seconds (configurable via `curlTimeoutInSeconds`)
- **Retries:** None (single attempt)

### FR-3: Response Parsing
- **Requirement:** The plugin MUST parse name-value pairs from backend JSON response
- **Implementation:** Uses string parsing to extract `"name"` and `"value"` fields
- **Behavior:** Returns all successfully parsed parameters in `paramList`

### FR-4: Success Criteria
- **Success Conditions:**
  - CURL operation completes with `CURLE_OK`
  - HTTP response code is 0 (libcurl default) or 200
  - At least one name-value pair extracted (implicit)
- **Failure Conditions:**
  - CURL operation fails
  - HTTP response code is neither 0 nor 200
  - Backend service unreachable

### FR-5: Iterator Pattern
- **Requirement:** Result MUST be returned as `IDeviceDiagnosticsParamListIterator`
- **Rationale:** Supports large result sets and COM-RPC marshalling efficiency
- **Implementation:** Uses Thunder's RPC iterator type

## Non-Functional Requirements

### NFR-1: Performance
- **Target:** < 100ms overhead for plugin processing (excluding backend latency)
- **Backend Timeout:** 30 seconds maximum wait time
- **Concurrency:** Multiple concurrent requests supported via Thunder's worker pool

### NFR-2: Reliability
- **Error Handling:** All CURL errors logged with warnings
- **Graceful Degradation:** Returns empty list on backend failure, not a hard error
- **Thread Safety:** Backend communication protected by async execution model

### NFR-3: Observability
- **Logging:** CURL result codes and HTTP status codes logged for debugging
- **Diagnostics:** Request and response payloads logged (may contain sensitive data - review security)

## Dependencies

### Runtime Dependencies
- **libcurl:** HTTP client library for backend communication
- **Backend Service:** Configuration service listening on `localhost:10999`
- **Thunder Framework:** JSON-RPC dispatcher and iterator types

### Compile-Time Dependencies
- CMake FindCurl module
- Thunder Plugins and Definitions packages

## Integration Points

### Backend Service Contract
- **Endpoint:** `http://127.0.0.1:10999`
- **Method:** POST
- **Content-Type:** Assumed `application/json`
- **Expected Response Format:**
  ```json
  {
    "result": [
      {"name": "param1", "value": "value1"},
      {"name": "param2", "value": "value2"}
    ]
  }
  ```
- **Note:** Exact backend service implementation is platform-specific

### Client Usage Patterns
```cpp
// C++ COM-RPC client
Exchange::IDeviceDiagnostics* diagnostics = /* obtain interface */;
auto names = /* create string iterator with parameter names */;
Exchange::IDeviceDiagnostics::IDeviceDiagnosticsParamListIterator* results;
bool success;
diagnostics->GetConfiguration(names, results, success);
```

```bash
# JSON-RPC client via curl
curl -H 'content-type:text/plain;' \
  --data-binary '{"jsonrpc":"2.0","id":1,"method":"org.rdk.DeviceDiagnostics.getConfiguration","params":{"names":["deviceModel","firmwareVersion"]}}' \
  http://127.0.0.1:9998/jsonrpc
```

## Error Scenarios

| Scenario | Behavior | Result |
|----------|----------|--------|
| Backend service down | CURL connection failure | `success: false`, empty `paramList` |
| Backend timeout (>30s) | CURL timeout | `success: false`, empty `paramList` |
| Malformed backend response | Parse failure | `success: false`, empty `paramList` |
| Empty parameter list | Valid request forwarded | Backend-dependent (typically `success: true`) |
| Non-200 HTTP status | Logged as warning | `success: false`, empty `paramList` |
| Unknown parameter names | Forwarded to backend | Backend determines success/failure behavior |

## Known Limitations

1. **No Parameter Validation:** Plugin does not validate parameter names; relies on backend
2. **No Caching:** Each request results in backend HTTP call (no local cache)
3. **No Authentication:** Backend communication is unauthenticated over localhost
4. **Parse Fragility:** String-based JSON parsing vulnerable to format changes
5. **No Retry Logic:** Single attempt per request (client must retry on failure)
6. **Synchronous Backend Call:** Blocks Thunder worker thread during HTTP request

## Security Considerations

- **Localhost-Only:** Backend service must be localhost-bound (hardcoded `127.0.0.1`)
- **No Input Sanitization:** Parameter names passed directly to backend without validation
- **Log Exposure:** Request/response data logged may contain sensitive configuration
- **No TLS:** HTTP (not HTTPS) used for backend communication

## Future Enhancements

1. **Caching Layer:** Cache frequently-accessed configuration values with TTL
2. **Async Backend Calls:** Non-blocking HTTP requests via Thunder async patterns
3. **Parameter Validation:** Client-side validation before backend forwarding
4. **Structured JSON Parsing:** Replace string parsing with proper JSON library (JsonValue)
5. **Retry Logic:** Configurable retry with exponential backoff
6. **Backend Discovery:** Support configurable backend URL (not hardcoded)
7. **Batch Optimization:** Coalesce multiple rapid requests into single backend call

## Testing Requirements

### Unit Tests (L1)
- Parse valid backend JSON response
- Handle malformed JSON response
- Handle CURL errors (connection refused, timeout, etc.)
- Handle non-200 HTTP status codes
- Validate iterator creation and traversal
- Empty parameter list handling

### Integration Tests (L2)
- Mock backend service returning known values
- Backend unavailability scenarios
- Large parameter list performance (100+ parameters)
- Concurrent request handling
- End-to-end JSON-RPC client test

### Performance Tests
- Measure plugin processing overhead
- Backend timeout behavior validation
- Memory usage for large result sets

## References

- [DeviceDiagnostics Plugin API](../../DeviceDiagnostics.md)
- [Thunder JSON-RPC Interface](https://rdkcentral.github.io/Thunder/)
- [IDeviceDiagnostics Interface](https://github.com/rdkcentral/entservices-apis/tree/main/apis/DeviceDiagnostics/IDeviceDiagnostics.h)
- Implementation: `plugin/DeviceDiagnosticsImplementation.cpp::getConfig()`
