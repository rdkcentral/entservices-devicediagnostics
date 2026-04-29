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

class DeviceDiagnosticsTest : public ::testing::Test {
protected:
    Core::ProxyType<Plugin::DeviceDiagnostics> deviceDiagnostic_;
    Core::JSONRPC::Handler& handler_;
    DECL_CORE_JSONRPC_CONX connection;
    NiceMock<ServiceMock> service;
    NiceMock<COMLinkMock> comLinkMock;
    Core::ProxyType<WorkerPoolImplementation> workerPool;
    Core::ProxyType<Plugin::DeviceDiagnosticsImplementation> DevDiagImpl;
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

        ON_CALL(comLinkMock, Instantiate(::testing::_, ::testing::_, ::testing::_))
        .WillByDefault(::testing::Invoke(
        [&](const RPC::Object& object, const uint32_t waitTime, uint32_t& connectionId) {
            DevDiagImpl = Core::ProxyType<Plugin::DeviceDiagnosticsImplementation>::Create();
            return &DevDiagImpl;
            }));

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

// ---------------------------------------------------------------------------
// GetPreviousRebootInfo tests
// ---------------------------------------------------------------------------

static const char* kPreviousRebootInfoFile = "/opt/secure/reboot/previousreboot.info";
static const char* kHardPowerInfoFile      = "/opt/secure/reboot/hardpower.info";

static void createDir(const char* path)
{
    // Ensure parent directory exists
    std::string p(path);
    size_t pos = p.rfind('/');
    if (pos != std::string::npos) {
        std::string dir = p.substr(0, pos);
        // mkdir -p equivalent: ignore errors (dir may exist)
        system(("mkdir -p " + dir).c_str());
    }
}

static void writeFile(const char* path, const std::string& content)
{
    createDir(path);
    std::ofstream f(path);
    if (f.is_open()) {
        f << content;
        f.close();
    }
}

static void removeFile(const char* path)
{
    ::unlink(path);
}

// Task 6.1: Both files exist with all fields present
TEST_F(DeviceDiagnosticsTest, getPreviousRebootInfo_AllFieldsPresent)
{
    writeFile(kPreviousRebootInfoFile,
        "PreviousRebootTime:20200128083540\n"
        "PreviousRebootReason:FIRMWARE_FAILURE\n"
        "PreviousRebootInitiatedBy:SystemPlugin\n"
        "PreviousCustomReason:API Validation\n"
        "PreviousOtherReason:API Validation\n");
    writeFile(kHardPowerInfoFile, "Tue Jan 28 08:22:22 UTC 2020\n");

    Exchange::IDeviceDiagnostics::RebootInfo rebootInfo;
    bool success = false;
    Core::hresult result = DevDiagImpl->GetPreviousRebootInfo(rebootInfo, success);

    EXPECT_EQ(result, Core::ERROR_NONE);
    EXPECT_TRUE(success);
    EXPECT_EQ(rebootInfo.timestamp,          _T("20200128083540"));
    EXPECT_EQ(rebootInfo.reason,             _T("FIRMWARE_FAILURE"));
    EXPECT_EQ(rebootInfo.source,             _T("SystemPlugin"));
    EXPECT_EQ(rebootInfo.customReason,       _T("API Validation"));
    EXPECT_EQ(rebootInfo.otherReason,        _T("API Validation"));
    EXPECT_EQ(rebootInfo.lastHardPowerReset, _T("Tue Jan 28 08:22:22 UTC 2020"));

    removeFile(kPreviousRebootInfoFile);
    removeFile(kHardPowerInfoFile);
}

// Task 6.2: previousreboot.info missing → Core::ERROR_GENERAL
TEST_F(DeviceDiagnosticsTest, getPreviousRebootInfo_MissingPrimaryFile)
{
    removeFile(kPreviousRebootInfoFile);
    removeFile(kHardPowerInfoFile);

    Exchange::IDeviceDiagnostics::RebootInfo rebootInfo;
    bool success = true;
    Core::hresult result = DevDiagImpl->GetPreviousRebootInfo(rebootInfo, success);

    EXPECT_EQ(result, Core::ERROR_GENERAL);
    EXPECT_FALSE(success);
}

// Task 6.3: hardpower.info missing → success: true, lastHardPowerReset empty
TEST_F(DeviceDiagnosticsTest, getPreviousRebootInfo_MissingHardpowerFile)
{
    writeFile(kPreviousRebootInfoFile,
        "PreviousRebootTime:20200128083540\n"
        "PreviousRebootReason:FIRMWARE_FAILURE\n"
        "PreviousRebootInitiatedBy:SystemPlugin\n"
        "PreviousCustomReason:API Validation\n"
        "PreviousOtherReason:API Validation\n");
    removeFile(kHardPowerInfoFile);

    Exchange::IDeviceDiagnostics::RebootInfo rebootInfo;
    bool success = false;
    Core::hresult result = DevDiagImpl->GetPreviousRebootInfo(rebootInfo, success);

    EXPECT_EQ(result, Core::ERROR_NONE);
    EXPECT_TRUE(success);
    EXPECT_EQ(rebootInfo.timestamp, _T("20200128083540"));
    EXPECT_TRUE(rebootInfo.lastHardPowerReset.empty());

    removeFile(kPreviousRebootInfoFile);
}

// Task 6.4: GetFileContent with a valid file → returns true and correct content
TEST_F(DeviceDiagnosticsTest, getFileContent_ValidFile)
{
    const char* tmpFile = "/tmp/test_dd_getcontent.txt";
    writeFile(tmpFile, "hello world\n");

    std::string content;
    bool result = DevDiagImpl->GetFileContent(string(tmpFile), content);

    EXPECT_TRUE(result);
    EXPECT_FALSE(content.empty());
    EXPECT_NE(content.find("hello world"), std::string::npos);

    removeFile(tmpFile);
}

// Task 6.5: GetFileContent with a non-existent file → returns false
TEST_F(DeviceDiagnosticsTest, getFileContent_MissingFile)
{
    std::string content = "should remain empty";
    bool result = DevDiagImpl->GetFileContent(string("/tmp/nonexistent_dd_file_xyz.txt"), content);

    EXPECT_FALSE(result);
}
