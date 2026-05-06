#include <cstdio>


/**
 * If not stated otherwise in this file or this component's LICENSE
 * file the following copyright and licenses apply:
 *
 * Copyright 2020 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#include "gtest/gtest.h"
#include <gmock/gmock.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "DeviceDiagnostics.h"
#include "ThunderPortability.h"
#include "DeviceDiagnosticsImplementation.h"
#include "WorkerPoolImplementation.h"
#include "ServiceMock.h"
#include "DeviceDiagnosticsMock.h"
#include "COMLinkMock.h"
#include "WrapsMock.h"

using namespace WPEFramework;
using ::testing::NiceMock;

// Helper to assert remove success or file not found
static void AssertRemove(const char* path) {
    int rc = remove(path);
    ASSERT_TRUE(rc == 0 || errno == ENOENT) << "Failed to remove file: " << path << ", errno: " << errno;
}
#include <cerrno>

// Helper to assert mkdir success or EEXIST
static void AssertMkdir(const char* path, mode_t mode) {
    int rc = mkdir(path, mode);
    ASSERT_TRUE(rc == 0 || errno == EEXIST) << "Failed to create directory: " << path << ", errno: " << errno;
}

class DeviceDiagnosticsTest : public ::testing::Test {
protected:
    Core::ProxyType<Plugin::DeviceDiagnostics> deviceDiagnostic_;
    Core::ProxyType<Plugin::DeviceDiagnosticsImplementation> deviceDiagnosticsImpl;
    Core::JSONRPC::Handler& handler_;
    DECL_CORE_JSONRPC_CONX connection;
    NiceMock<ServiceMock> service;
    NiceMock<COMLinkMock> comLinkMock;
    Core::ProxyType<WorkerPoolImplementation> workerPool;
    Exchange::IDeviceDiagnostics::INotification *DevDiagNotification = nullptr;
    string response;
    WrapsImplMock *p_wrapsImplMock   = nullptr;
    ServiceMock  *p_serviceMock  = nullptr;
    DeviceDiagnosticsMock *p_devDiagMock = nullptr;

    DeviceDiagnosticsTest()
        : deviceDiagnostic_(Core::ProxyType<Plugin::DeviceDiagnostics>::Create())
        , handler_(*deviceDiagnostic_)
        , INIT_CONX(1, 0)
        , workerPool(Core::ProxyType<WorkerPoolImplementation>::Create(
            2, Core::Thread::DefaultStackSize(), 16))
    {
        p_serviceMock = new NiceMock <ServiceMock>;

        p_devDiagMock = new NiceMock <DeviceDiagnosticsMock>;

        p_wrapsImplMock  = new NiceMock <WrapsImplMock>;
        Wraps::setImpl(p_wrapsImplMock);

        ON_CALL(*p_devDiagMock, Register(::testing::_))
        .WillByDefault(::testing::Invoke(
            [&](Exchange::IDeviceDiagnostics::INotification *notification){
                DevDiagNotification = notification;
                return Core::ERROR_NONE;;
            }));

        // Mock service->COMLink() to return comLinkMock
        ON_CALL(service, COMLink())
            .WillByDefault(::testing::Invoke(
                [this]() -> WPEFramework::PluginHost::IShell::ICOMLink* {
                    return &comLinkMock;
                }));

        // Mock comLinkMock->Instantiate() to return DeviceDiagnosticsImplementation
#ifdef USE_THUNDER_R4
        ON_CALL(comLinkMock, Instantiate(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Invoke(
                [&](const RPC::Object& object, const uint32_t waitTime, uint32_t& connectionId) -> void* {
                    deviceDiagnosticsImpl = Core::ProxyType<Plugin::DeviceDiagnosticsImplementation>::Create();
                    return &deviceDiagnosticsImpl;
                }));
#else
        ON_CALL(comLinkMock, Instantiate(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Invoke(
                [&](const RPC::Object& object, const uint32_t waitTime, uint32_t& connectionId, const string& className, const string& callsign) -> void* {
                    deviceDiagnosticsImpl = Core::ProxyType<Plugin::DeviceDiagnosticsImplementation>::Create();
                    return deviceDiagnosticsImpl;
                }));
#endif

        Core::IWorkerPool::Assign(&(*workerPool));
        workerPool->Run();

        deviceDiagnostic_->Initialize(&service);
    }
  
    virtual ~DeviceDiagnosticsTest()
     {
        deviceDiagnostic_->Deinitialize(&service);

        Core::IWorkerPool::Assign(nullptr);
        workerPool.Release();

        if (p_serviceMock != nullptr)
        {
            delete p_serviceMock;
            p_serviceMock = nullptr;
        }

        if (p_devDiagMock != nullptr)
        {
            delete p_devDiagMock;
            p_devDiagMock = nullptr;
        }

        Wraps::setImpl(nullptr);
        if (p_wrapsImplMock != nullptr)
        {
            delete p_wrapsImplMock;
            p_wrapsImplMock = nullptr;
        }
    }    
};

TEST_F(DeviceDiagnosticsTest, RegisterMethod)
{
    EXPECT_EQ(Core::ERROR_NONE, handler_.Exists(_T("getConfiguration")));
    EXPECT_EQ(Core::ERROR_NONE, handler_.Exists(_T("getAVDecoderStatus")));
}

/**
 * fails without valgrind
 * 2022-12-22T13:10:15.1620146Z Value of: bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0
2022-12-22T13:10:15.1620468Z   Actual: true
2022-12-22T13:10:15.1620732Z Expected: false
2022-12-22T13:10:15.1635528Z [  FAILED  ] DeviceDiagnosticsTest.getConfiguration (9 ms)
 */
TEST_F(DeviceDiagnosticsTest, getConfiguration)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_TRUE(sockfd != -1);

    //for port reuse
    int pt = 1;
    ASSERT_FALSE(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &pt, sizeof(pt)) < 0);

    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(10999);
    ASSERT_FALSE(bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0);
    ASSERT_FALSE(listen(sockfd, 10) < 0);

    std::thread thread = std::thread([&]() {
        auto addrlen = sizeof(sockaddr);
        const int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
        ASSERT_FALSE(connection < 0);
        char buffer[2048] = { 0 };
        ASSERT_TRUE(read(connection, buffer, 2048) > 0);
        EXPECT_EQ(string(buffer), string(_T("POST / HTTP/1.1\r\nHost: 127.0.0.1:10999\r\nAccept: */*\r\nContent-Length: 31\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n{\"paramList\":[{\"name\":\"test\"}]}")));
        std::string response = _T("HTTP/1.1 200\n\rContent-type: application/json\n\r{\"paramList\":[\"Device.X_CISCO_COM_LED.RedPwm\":123],\"success\":true}");
        send(connection, response.c_str(), response.size(), 0);
        close(connection);
    });

    EXPECT_EQ(Core::ERROR_NONE, handler_.Invoke(connection, _T("getConfiguration"), _T("{\"names\":[\"test\"]}"), response));

    thread.join();

    close(sockfd);
}

TEST_F(DeviceDiagnosticsTest, getAVDecoderStatus)
{
    EXPECT_EQ(Core::ERROR_NONE, handler_.Invoke(connection, _T("getAVDecoderStatus"), _T("{}"), response));
    EXPECT_EQ(response, _T("{\"avDecoderStatus\":\"IDLE\"}"));
}

/************Test case Details **************************
** Test 3.1: Successful reboot info retrieval with both files present and all fields populated
*******************************************************/
TEST_F(DeviceDiagnosticsTest, GetPreviousRebootInfo_Success_AllFields)
{
    // Create test directory
    AssertMkdir("/opt/secure", 0755);
    AssertMkdir("/opt/secure/reboot", 0755);
    
    // Create primary reboot info file with all fields
    std::ofstream primaryFile("/opt/secure/reboot/previousreboot.info");
    primaryFile << "{\"timestamp\":\"2024-01-15T10:30:45Z\","
                << "\"source\":\"PowerKey\","
                << "\"reason\":\"UserRequested\","
                << "\"customReason\":\"Remote control power button\","
                << "\"otherReason\":\"Scheduled maintenance\"}";
    primaryFile.close();
    
    // Create hard power info file
    std::ofstream hardPowerFile("/opt/secure/reboot/hardpower.info");
    hardPowerFile << "{\"lastHardPowerReset\":\"2024-01-10T08:15:30Z\"}";
    hardPowerFile.close();
    
    // Test the API via JSON-RPC
    response.clear();
    Core::hresult result = handler_.Invoke(connection, _T("getPreviousRebootInfo"), _T("{}"), response);
    EXPECT_EQ(result, Core::ERROR_NONE);

    JsonObject respJson;
    ASSERT_TRUE(respJson.FromString(response));
    ASSERT_TRUE(respJson.HasLabel("rebootInfo"));
    const JsonObject& rebootInfo = respJson["rebootInfo"].Object();
    EXPECT_EQ(rebootInfo["timestamp"].String(), "2024-01-15T10:30:45Z");
    EXPECT_EQ(rebootInfo["source"].String(), "PowerKey");
    EXPECT_EQ(rebootInfo["reason"].String(), "UserRequested");
    EXPECT_EQ(rebootInfo["customReason"].String(), "Remote control power button");
    EXPECT_EQ(rebootInfo["otherReason"].String(), "Scheduled maintenance");
    EXPECT_EQ(rebootInfo["lastHardPowerReset"].String(), "2024-01-10T08:15:30Z");
    ASSERT_TRUE(respJson.HasLabel("success"));
    EXPECT_TRUE(respJson["success"].Boolean());
    
    // Cleanup
    AssertRemove("/opt/secure/reboot/previousreboot.info");
    AssertRemove("/opt/secure/reboot/hardpower.info");
}

/************Test case Details **************************
** Test 3.2: Primary file exists but hardpower.info missing scenario
*******************************************************/
TEST_F(DeviceDiagnosticsTest, GetPreviousRebootInfo_HardPowerFileMissing)
{
    // Create test directory
    AssertMkdir("/opt/secure", 0755);
    AssertMkdir("/opt/secure/reboot", 0755);
    
    // Create only primary reboot info file
    std::ofstream primaryFile("/opt/secure/reboot/previousreboot.info");
    primaryFile << "{\"timestamp\":\"2024-01-15T10:30:45Z\","
                << "\"source\":\"PowerKey\","
                << "\"reason\":\"UserRequested\","
                << "\"customReason\":\"Remote control\","
                << "\"otherReason\":\"None\"}";
    primaryFile.close();
    
    // Make sure hardpower.info doesn't exist
    AssertRemove("/opt/secure/reboot/hardpower.info");
    
    // Test the API via JSON-RPC
    response.clear();
    Core::hresult result = handler_.Invoke(connection, _T("getPreviousRebootInfo"), _T("{}"), response);
    EXPECT_EQ(result, Core::ERROR_NONE);
    
    // Cleanup
    AssertRemove("/opt/secure/reboot/previousreboot.info");
}

/************Test case Details **************************
** Test 3.3: Primary reboot info file not found scenario (should return ERROR_GENERAL)
*******************************************************/
TEST_F(DeviceDiagnosticsTest, GetPreviousRebootInfo_PrimaryFileMissing)
{
    // Ensure files don't exist
    AssertRemove("/opt/secure/reboot/previousreboot.info");
    AssertRemove("/opt/secure/reboot/hardpower.info");
    
    // Test the API via JSON-RPC
    response.clear();
    Core::hresult result = handler_.Invoke(connection, _T("getPreviousRebootInfo"), _T("{}"), response);
    EXPECT_EQ(result, Core::ERROR_GENERAL);
}

/************Test case Details **************************
** Test 3.4: Invalid JSON in primaryreboot.info (should return ERROR_GENERAL)
*******************************************************/
TEST_F(DeviceDiagnosticsTest, GetPreviousRebootInfo_InvalidPrimaryJSON)
{
    // Create test directory
    AssertMkdir("/opt/secure", 0755);
    AssertMkdir("/opt/secure/reboot", 0755);
    
    // Create primary file with invalid JSON
    std::ofstream primaryFile("/opt/secure/reboot/previousreboot.info");
    primaryFile << "This is not valid JSON content{broken";
    primaryFile.close();
    
    // Create valid hard power file
    std::ofstream hardPowerFile("/opt/secure/reboot/hardpower.info");
    hardPowerFile << "{\"lastHardPowerReset\":\"2024-01-10T08:15:30Z\"}";
    hardPowerFile.close();
    
    // Test the API via JSON-RPC
    response.clear();
    Core::hresult result = handler_.Invoke(connection, _T("getPreviousRebootInfo"), _T("{}"), response);
    EXPECT_EQ(result, Core::ERROR_GENERAL);
    
    // Cleanup
    AssertRemove("/opt/secure/reboot/previousreboot.info");
    AssertRemove("/opt/secure/reboot/hardpower.info");
}

/************Test case Details **************************
** Test 3.5: Invalid JSON in hardpower.info (returns ERROR_NONE with lastHardPowerReset="Unknown")
*******************************************************/
TEST_F(DeviceDiagnosticsTest, GetPreviousRebootInfo_InvalidHardPowerJSON)
{
    // Create test directory
    AssertMkdir("/opt/secure", 0755);
    AssertMkdir("/opt/secure/reboot", 0755);
    
    // Create valid primary file
    std::ofstream primaryFile("/opt/secure/reboot/previousreboot.info");
    primaryFile << "{\"timestamp\":\"2024-01-15T10:30:45Z\","
                << "\"source\":\"PowerKey\","
                << "\"reason\":\"UserRequested\","
                << "\"customReason\":\"Remote control\","
                << "\"otherReason\":\"None\"}";
    primaryFile.close();
    
    // Create hard power file with invalid JSON
    std::ofstream hardPowerFile("/opt/secure/reboot/hardpower.info");
    hardPowerFile << "Invalid JSON content here{";
    hardPowerFile.close();
    
    // Test the API via JSON-RPC
    response.clear();
    Core::hresult result = handler_.Invoke(connection, _T("getPreviousRebootInfo"), _T("{}"), response);
    EXPECT_EQ(result, Core::ERROR_NONE);
    
    // Cleanup
    AssertRemove("/opt/secure/reboot/previousreboot.info");
    AssertRemove("/opt/secure/reboot/hardpower.info");
}

/************Test case Details **************************
** Test 3.6: Missing fields in JSON files (empty strings returned)
*******************************************************/
TEST_F(DeviceDiagnosticsTest, GetPreviousRebootInfo_MissingFields)
{
    // Create test directory
    AssertMkdir("/opt/secure", 0755);
    AssertMkdir("/opt/secure/reboot", 0755);
    
    // Create primary file with only some fields
    std::ofstream primaryFile("/opt/secure/reboot/previousreboot.info");
    primaryFile << "{\"timestamp\":\"2024-01-15T10:30:45Z\","
                << "\"source\":\"PowerKey\"}";
    primaryFile.close();
    
    // Create hard power file without any fields
    std::ofstream hardPowerFile("/opt/secure/reboot/hardpower.info");
    hardPowerFile << "{}";
    hardPowerFile.close();
    
    // Test the API via JSON-RPC
    response.clear();
    Core::hresult result = handler_.Invoke(connection, _T("getPreviousRebootInfo"), _T("{}"), response);
    EXPECT_EQ(result, Core::ERROR_NONE);
    JsonObject respJson;
    ASSERT_TRUE(respJson.FromString(response));
    ASSERT_TRUE(respJson.HasLabel("rebootInfo"));
    const JsonObject& rebootInfo = respJson["rebootInfo"].Object();
    EXPECT_EQ(rebootInfo["timestamp"].String(), "2024-01-15T10:30:45Z");
    EXPECT_EQ(rebootInfo["source"].String(), "PowerKey");
    // Missing JSON fields are returned as JSON null, which serializes as "\"null\""
    EXPECT_TRUE(rebootInfo["reason"].String().empty() || rebootInfo["reason"].String() == "\"null\"");
    EXPECT_TRUE(rebootInfo["customReason"].String().empty() || rebootInfo["customReason"].String() == "\"null\"");
    EXPECT_TRUE(rebootInfo["otherReason"].String().empty() || rebootInfo["otherReason"].String() == "\"null\"");
    EXPECT_TRUE(rebootInfo["lastHardPowerReset"].String().empty() || rebootInfo["lastHardPowerReset"].String() == "\"Unknown\"");
    ASSERT_TRUE(respJson.HasLabel("success"));
    EXPECT_TRUE(respJson["success"].Boolean());
    
    // Cleanup
    AssertRemove("/opt/secure/reboot/previousreboot.info");
    AssertRemove("/opt/secure/reboot/hardpower.info");
}

/************Test case Details **************************
** Test 3.7: Empty primary file (should return ERROR_GENERAL)
*******************************************************/
TEST_F(DeviceDiagnosticsTest, GetPreviousRebootInfo_EmptyPrimaryFile)
{
    // Create test directory
    AssertMkdir("/opt/secure", 0755);
    AssertMkdir("/opt/secure/reboot", 0755);
    
    // Create empty primary file
    std::ofstream primaryFile("/opt/secure/reboot/previousreboot.info");
    primaryFile << "";
    primaryFile.close();
    
    // Create valid hard power file
    std::ofstream hardPowerFile("/opt/secure/reboot/hardpower.info");
    hardPowerFile << "{\"lastHardPowerReset\":\"2024-01-10T08:15:30Z\"}";
    hardPowerFile.close();
    
    // Test the API via JSON-RPC
    response.clear();
    Core::hresult result = handler_.Invoke(connection, _T("getPreviousRebootInfo"), _T("{}"), response);
    EXPECT_EQ(result, Core::ERROR_GENERAL);
    
    // Cleanup
    AssertRemove("/opt/secure/reboot/previousreboot.info");
    AssertRemove("/opt/secure/reboot/hardpower.info");
}
