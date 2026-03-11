/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2019 RDK Management
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

#include "DeviceDiagnostics.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#define API_VERSION_NUMBER_MAJOR 1
#define API_VERSION_NUMBER_MINOR 1
#define API_VERSION_NUMBER_PATCH 2

namespace WPEFramework
{

    namespace {

        static Plugin::Metadata<Plugin::DeviceDiagnostics> metadata(
            // Version (Major, Minor, Patch)
            API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH,
            // Preconditions
            {},
            // Terminations
            {},
            // Controls
            {}
        );
    }

    namespace Plugin
    {

    /*
     *Register DeviceDiagnostics module as wpeframework plugin
     **/
    SERVICE_REGISTRATION(DeviceDiagnostics, API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH);

    DeviceDiagnostics::DeviceDiagnostics() : _service(nullptr), _connectionId(0), _deviceDiagnostics(nullptr), _deviceDiagnosticsNotification(this)
    {
        SYSLOG(Logging::Startup, (_T("DeviceDiagnostics Constructor")));
    }

    DeviceDiagnostics::~DeviceDiagnostics()
    {
        SYSLOG(Logging::Shutdown, (string(_T("DeviceDiagnostics Destructor"))));
    }

    const string DeviceDiagnostics::Initialize(PluginHost::IShell* service)
    {
        string message="";

        ASSERT(nullptr != service);
        ASSERT(nullptr == _service);
        ASSERT(nullptr == _deviceDiagnostics);
        ASSERT(0 == _connectionId);

        const auto initStart = std::chrono::system_clock::now();
        const std::time_t initStartTime = std::chrono::system_clock::to_time_t(initStart);
        const auto initStartMs = std::chrono::duration_cast<std::chrono::milliseconds>(initStart.time_since_epoch()).count() % 1000;
        std::tm initStartLocal{};
        localtime_r(&initStartTime, &initStartLocal);
        std::ostringstream initStartStream;
        initStartStream << std::put_time(&initStartLocal, "%Y-%m-%d %H:%M:%S") << '.'
                << std::setw(3) << std::setfill('0') << initStartMs;

        SYSLOG(Logging::Startup, (_T("DeviceDiagnostics::Initialize: PID=%u Timestamp=%s"), getpid(), initStartStream.str().c_str()));

        _service = service;
        _service->AddRef();
        _service->Register(&_deviceDiagnosticsNotification);

        _deviceDiagnostics = service->Root<Exchange::IDeviceDiagnostics>(_connectionId, 5000, _T("DeviceDiagnosticsImplementation"));

        if(nullptr != _deviceDiagnostics)
        {
            // Register for notifications
            _deviceDiagnostics->Register(&_deviceDiagnosticsNotification);
            // Invoking Plugin API register to wpeframework
            Exchange::JDeviceDiagnostics::Register(*this, _deviceDiagnostics);
        }
        else
        {
            SYSLOG(Logging::Startup, (_T("DeviceDiagnostics::Initialize: Failed to initialise DeviceDiagnostics plugin")));
            message = _T("DeviceDiagnostics plugin could not be initialised");
        }

        const auto initEnd = std::chrono::system_clock::now();
        const std::time_t initEndTime = std::chrono::system_clock::to_time_t(initEnd);
        const auto initEndMs = std::chrono::duration_cast<std::chrono::milliseconds>(initEnd.time_since_epoch()).count() % 1000;
        std::tm initEndLocal{};
        localtime_r(&initEndTime, &initEndLocal);
        std::ostringstream initEndStream;
        initEndStream << std::put_time(&initEndLocal, "%Y-%m-%d %H:%M:%S") << '.'
                      << std::setw(3) << std::setfill('0') << initEndMs;

        SYSLOG(Logging::Startup, (_T("DeviceDiagnostics::Initialize complete: Timestamp=%s"), initEndStream.str().c_str()));
        
        return message;
    }

    void DeviceDiagnostics::Deinitialize(PluginHost::IShell* service)
    {
        ASSERT(_service == service);

        const auto deinitStart = std::chrono::system_clock::now();
        const std::time_t deinitStartTime = std::chrono::system_clock::to_time_t(deinitStart);
        const auto deinitStartMs = std::chrono::duration_cast<std::chrono::milliseconds>(deinitStart.time_since_epoch()).count() % 1000;
        std::tm deinitStartLocal{};
        localtime_r(&deinitStartTime, &deinitStartLocal);
        std::ostringstream deinitStartStream;
        deinitStartStream << std::put_time(&deinitStartLocal, "%Y-%m-%d %H:%M:%S") << '.'
                          << std::setw(3) << std::setfill('0') << deinitStartMs;

        SYSLOG(Logging::Shutdown, (string(_T("DeviceDiagnostics::Deinitialize Timestamp=")) + deinitStartStream.str()));

        // Make sure the Activated and Deactivated are no longer called before we start cleaning up..
        _service->Unregister(&_deviceDiagnosticsNotification);

        if (nullptr != _deviceDiagnostics)
        {

            _deviceDiagnostics->Unregister(&_deviceDiagnosticsNotification);
            Exchange::JDeviceDiagnostics::Unregister(*this);

            // Stop processing:
            RPC::IRemoteConnection* connection = service->RemoteConnection(_connectionId);
            VARIABLE_IS_NOT_USED uint32_t result = _deviceDiagnostics->Release();

            _deviceDiagnostics = nullptr;

            // It should have been the last reference we are releasing,
            // so it should endup in a DESTRUCTION_SUCCEEDED, if not we
            // are leaking...
            ASSERT(result == Core::ERROR_DESTRUCTION_SUCCEEDED);

            // If this was running in a (container) process...
            if (nullptr != connection)
            {
               // Lets trigger the cleanup sequence for
               // out-of-process code. Which will guard
               // that unwilling processes, get shot if
               // not stopped friendly :-)
               try
               {
                   connection->Terminate();
                   // Log success if needed
                   LOGWARN("Connection terminated successfully.");
               }
               catch (const std::exception& e)
               {
                   std::string errorMessage = "Failed to terminate connection: ";
                   errorMessage += e.what();
                   LOGWARN("%s",errorMessage.c_str());
               }

               connection->Release();
            }
        }

        _connectionId = 0;
        _service->Release();
        _service = nullptr;
        const auto deinitEnd = std::chrono::system_clock::now();
        const std::time_t deinitEndTime = std::chrono::system_clock::to_time_t(deinitEnd);
        const auto deinitEndMs = std::chrono::duration_cast<std::chrono::milliseconds>(deinitEnd.time_since_epoch()).count() % 1000;
        std::tm deinitEndLocal{};
        localtime_r(&deinitEndTime, &deinitEndLocal);
        std::ostringstream deinitEndStream;
        deinitEndStream << std::put_time(&deinitEndLocal, "%Y-%m-%d %H:%M:%S") << '.'
                        << std::setw(3) << std::setfill('0') << deinitEndMs;

        SYSLOG(Logging::Shutdown, (string(_T("DeviceDiagnostics de-initialised Timestamp=")) + deinitEndStream.str()));
    }

    string DeviceDiagnostics::Information() const
    {
       return ("This DeviceDiagnostics Plugin provides additional diagnostics information which includes device configuration and AV decoder status.");
    }

    void DeviceDiagnostics::Deactivated(RPC::IRemoteConnection* connection)
    {
        if (connection->Id() == _connectionId) {
            ASSERT(nullptr != _service);
            Core::IWorkerPool::Instance().Submit(PluginHost::IShell::Job::Create(_service, PluginHost::IShell::DEACTIVATED, PluginHost::IShell::FAILURE));
        }
    }
} // namespace Plugin
} // namespace WPEFramework
