/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2026 RDK Management
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
*/

#include "SystemServices.h"

#define API_VERSION_NUMBER_MAJOR 1
#define API_VERSION_NUMBER_MINOR 0
#define API_VERSION_NUMBER_PATCH 0

namespace WPEFramework
{

    namespace {

        static Plugin::Metadata<Plugin::SystemServices> metadata(
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

<<<<<<< HEAD
    namespace Plugin {
        namespace {
            
            uint32_t GetValueFromPropertiesFile(const char* filename, const char* key, string& response, const char *delimiter = "=")
            {
                uint32_t result = Core::ERROR_GENERAL;
                
                if (!Utils::fileExists(filename)) {
                    return result;
                }

                char buf[1024];

                FILE *f = fopen(filename, "r");

                if(!f) {
                    LOGWARN("failed to open %s:%s", filename, strerror(errno));
                    return result;
                }

                std::string line;
                std::string make;
                while(fgets(buf, sizeof(buf), f) != NULL) {
                    line = buf;
                    size_t eq = line.find_first_of(delimiter);

                    if (std::string::npos != eq) {
                        std::string k = line.substr(0, eq);

                        if (k == key) {
                            response = line.substr(eq + strlen(delimiter));
                            Utils::String::trim(response);
                            result = Core::ERROR_NONE;
                            break;
                        }
                    }
                }

                fclose(f);

                return result;
            }

            uint32_t SetValueFromPropertiesFile(const char* filename, const char* key, JsonObject& response, const char* parameterName, const char *delimiter = "=")
            {
                uint32_t result = Core::ERROR_GENERAL;
                std::string value;

                if (Core::ERROR_NONE == GetValueFromPropertiesFile(filename, key, value, delimiter)) {
                    LOGWARN("Got %s from %s", parameterName, filename);
                    response[parameterName] = value;
                    result = Core::ERROR_NONE;
                }
                else
                    LOGWARN("Failed to get %s from %s", key, filename);

                return result;                    
            }

            uint32_t GetFileRegex(const char* filename, const std::regex& regex, string& response)
            {
                uint32_t result = Core::ERROR_GENERAL;

                if (!Utils::fileExists(filename)) {
                    LOGWARN("GetFileRegex: file %s does not exist", filename);
                    return result;
                }
                std::ifstream file(filename);
                if (file) {
                    string line;
                    while (std::getline(file, line)) {
                        std::smatch sm;
                        if (std::regex_match(line, sm, regex)) {
                            if (sm.size() == 2) {
                                response = sm[1];
                                result = Core::ERROR_NONE;
                            } else {
                                LOGERR("GetFileRegex: Unexpected capture group count %zu (expected 2) in file %s",
                                    sm.size(), filename);
                                result = Core::ERROR_GENERAL;
                            }
                            break;
                        }
                    }

                    if (result != Core::ERROR_NONE) {
                        LOGWARN("Regex pattern did not match any line in %s", filename);
                    }
                }
                else {
                    LOGWARN("failed to open %s:%s", filename, strerror(errno));
                }

                return result;
            }
        }

        //Prototypes
        std::string   SystemServices::m_currentMode = "";
        cTimer    SystemServices::m_operatingModeTimer;
        int       SystemServices::m_remainingDuration = 0;
        JsonObject SystemServices::_systemParams;
        const string SystemServices::MODEL_NAME = "modelName";
        const string SystemServices::HARDWARE_ID = "hardwareID";
	const string SystemServices::FRIENDLY_ID = "friendly_id";

        IARM_Bus_SYSMgr_GetSystemStates_Param_t SystemServices::paramGetSysState = {};

#ifdef ENABLE_THERMAL_PROTECTION
        static void handleThermalLevelChange(const int &currentThermalLevel, const int &newThermalLevel, const float &currentTemperature);
#endif /* ENABLE_THERMAL_PROTECTION */
#ifdef ENABLE_SYSTIMEMGR_SUPPORT
        void _timerStatusEventHandler(const char *owner, IARM_EventId_t eventId,
                void *data, size_t len);
#endif// ENABLE_SYSTIMEMGR_SUPPORT

#if defined(USE_IARMBUS) || defined(USE_IARM_BUS)
        static IARM_Result_t _SysModeChange(void *arg);
        static void _systemStateChanged(const char *owner,
                IARM_EventId_t eventId, void *data, size_t len);
        static void _deviceMgtUpdateReceived(const char *owner,
                IARM_EventId_t eventId, void *data, size_t len);	
#endif /* defined(USE_IARMBUS) || defined(USE_IARM_BUS) */

        SERVICE_REGISTRATION(SystemServices, API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH);

        SystemServices* SystemServices::_instance = nullptr;
        cSettings SystemServices::m_temp_settings(SYSTEM_SERVICE_TEMP_FILE);

        /**
         * Register SystemService module as wpeframework plugin
         */
        SystemServices::SystemServices()
	    : PluginHost::JSONRPCErrorAssessor<PluginHost::JSONRPCErrorAssessorTypes::FunctionCallbackType>(SystemServices::OnJSONRPCError)
            , _pwrMgrNotification(*this)
            , _registeredEventHandlers(false)
        {
            SystemServices::_instance = this;
            //Updating the standard territory
            m_strStandardTerritoryList =   "ABW AFG AGO AIA ALA ALB AND ARE ARG ARM ASM ATA ATF ATG AUS AUT AZE BDI BEL BEN BES BFA BGD BGR BHR BHS BIH BLM BLR BLZ BMU BOL                BRA BRB BRN BTN BVT BWA CAF CAN CCK CHE CHL CHN CIV CMR COD COG COK COL COM CPV CRI CUB Cuba CUW CXR CYM CYP CZE DEU DJI DMA DNK DOM DZA ECU EGY ERI ESH ESP                EST ETH FIN FJI FLK FRA FRO FSM GAB GBR GEO GGY GHA GIB GIN GLP GMB GNB GNQ GRC GRD GRL GTM GUF GUM GUY HKG HMD HND HRV HTI HUN IDN IMN IND IOT IRL IRN IRQ                 ISL ISR ITA JAM JEY JOR JPN KAZ KEN KGZ KHM KIR KNA KOR KWT LAO LBN LBR LBY LCA LIE LKA LSO LTU LUX LVA MAC MAF MAR MCO MDA MDG MDV MEX MHL MKD MLI MLT MMR                 MNE MNG MNP MOZ MRT MSR MTQ MUS MWI MYS MYT NAM NCL NER NFK NGA NIC NIU NLD NOR NPL NRU NZL OMN PAK PAN PCN PER PHL PLW PNG POL PRI PRK PRT PRY PSE PYF QAT                 REU ROU RUS RWA SAU SDN SEN SGP SGS SHN SJM SLB SLE SLV SMR SOM SPM SRB SSD STP SUR SVK SVN SWE SWZ SXM SYC SYR TCA TCD TGO THA TJK TKL TKM TLS TON TTO TUN                 TUR TUV TWN TZA UGA UKR UMI URY USA UZB VAT VCT VEN VGB VIR VNM VUT WLF WSM YEM ZAF ZMB ZWE";

            CreateHandler({ 2 });

            SystemServices::m_FwUpdateState_LatestEvent=FirmwareUpdateStateUninitialized;

            m_networkStandbyModeValid = false;
            m_powerStateBeforeRebootValid = false;
            m_friendlyName = "Living Room";


#ifdef ENABLE_DEVICE_MANUFACTURER_INFO
	    m_ManufacturerDataHardwareIdValid = false;
	    m_ManufacturerDataModelNameValid = false;
            m_MfgSerialNumberValid = false;
#endif
            m_uploadLogsPid = -1;

            regcomp (&m_regexUnallowedChars, REGEX_UNALLOWABLE_INPUT, REG_EXTENDED);

            /**
             * @brief Invoking Plugin API register to WPEFRAMEWORK.
             */
#ifdef DEBUG
            registerMethod("sampleSystemServiceAPI", &SystemServices::sampleAPI, this);
#endif /* DEBUG */
            registerMethod("getDeviceInfo", &SystemServices::getDeviceInfo, this);
#ifdef ENABLE_DEVICE_MANUFACTURER_INFO
            registerMethod("getMfgSerialNumber", &SystemServices::getMfgSerialNumber, this);
#endif
            registerMethod("reboot", &SystemServices::requestSystemReboot, this);
            registerMethod("requestSystemUptime",
                    &SystemServices::requestSystemUptime, this);
#if defined(HAS_API_SYSTEM) && defined(HAS_API_POWERSTATE)
            registerMethod("getPowerState", &SystemServices::getDevicePowerState,
                    this);
            registerMethod("setPowerState", &SystemServices::setDevicePowerState,
                    this);
#endif /* HAS_API_SYSTEM && HAS_API_POWERSTATE */
#ifdef ENABLE_SYSTIMEMGR_SUPPORT
            registerMethod("getTimeStatus", &SystemServices::getSystemTimeStatus,this);
#endif// ENABLE_SYSTIMEMGR_SUPPORT
            registerMethod("updateFirmware", &SystemServices::updateFirmware, this);
            registerMethod("setMode", &SystemServices::setMode, this);
	    registerMethod("setBootLoaderSplashScreen", &SystemServices::setBootLoaderSplashScreen, this);	    
            registerMethod("getFirmwareUpdateInfo",
                    &SystemServices::getFirmwareUpdateInfo, this);
            registerMethod("setDeepSleepTimer", &SystemServices::setDeepSleepTimer,
                    this);
            registerMethod("getSerialNumber", &SystemServices::getSerialNumber,
                    this);
            registerMethod("getDownloadedFirmwareInfo",
                    &SystemServices::getDownloadedFirmwareInfo, this);
            registerMethod("getFirmwareDownloadPercent",
                    &SystemServices::getFirmwareDownloadPercent, this);
            registerMethod("getFirmwareUpdateState",
                    &SystemServices::getFirmwareUpdateState, this);
            registerMethod("setTimeZoneDST", &SystemServices::setTimeZoneDST, this);
            registerMethod("getTimeZoneDST", &SystemServices::getTimeZoneDST, this);
            registerMethod("getRFCConfig", &SystemServices::getRFCConfig, this);
            registerMethod("getSystemVersions", &SystemServices::getSystemVersions, this);
            registerMethod("setNetworkStandbyMode", &SystemServices::setNetworkStandbyMode, this);
            registerMethod("getNetworkStandbyMode", &SystemServices::getNetworkStandbyMode, this);
    	    registerMethod("setTerritory", &SystemServices::setTerritory, this);
	    registerMethod("getTerritory", &SystemServices::getTerritory, this);

            // version 2 APIs
            registerMethod(_T("getTimeZones"), &SystemServices::getTimeZones, this);
#ifdef ENABLE_DEEP_SLEEP
            registerMethod(_T("getWakeupReason"), &SystemServices::getWakeupReason, this);
            registerMethod(_T("getLastWakeupKeyCode"), &SystemServices::getLastWakeupKeyCode, this);
#endif

            registerMethod("uploadLogsAsync", &SystemServices::uploadLogsAsync, this);
            registerMethod("abortLogUpload", &SystemServices::abortLogUpload, this);

            registerMethod("getPowerStateBeforeReboot", &SystemServices::getPowerStateBeforeReboot,
                    this);
            registerMethod("getLastFirmwareFailureReason", &SystemServices::getLastFirmwareFailureReason, this);
            registerMethod("setOptOutTelemetry", &SystemServices::setOptOutTelemetry, this);
            registerMethod("isOptOutTelemetry", &SystemServices::isOptOutTelemetry, this);
            registerMethod("setFirmwareAutoReboot", &SystemServices::setFirmwareAutoReboot, this);
	    registerMethod("getFriendlyName", &SystemServices::getFriendlyName, this);
            registerMethod("setFriendlyName", &SystemServices::setFriendlyName, this);
            registerMethod("setFSRFlag", &SystemServices::setFSRFlag, this);
            registerMethod("getFSRFlag", &SystemServices::getFSRFlag, this);
            registerMethod("setBlocklistFlag", &SystemServices::setBlocklistFlag, this);
            registerMethod("getBlocklistFlag", &SystemServices::getBlocklistFlag, this);
            registerMethod("getBootTypeInfo", &SystemServices::getBootTypeInfo, this);
            registerMethod("getBuildType", &SystemServices::getBuildType, this);
	    registerMethod("setMigrationStatus", &SystemServices::setMigrationStatus, this);
            registerMethod("getMigrationStatus", &SystemServices::getMigrationStatus, this);
            registerMethod("setWakeupSrcConfiguration", &SystemServices::setWakeupSrcConfiguration, this);
            registerMethod("getMacAddresses",&SystemServices::getMacAddresses, this);
            Register<JsonObject, PlatformCaps>("getPlatformConfiguration",
                &SystemServices::getPlatformConfiguration, this);
            GetHandler(2)->Register<JsonObject, PlatformCaps>("getPlatformConfiguration",
                &SystemServices::getPlatformConfiguration, this);
#if 0
            registerMethod("getXconfParams", &SystemServices::getXconfParams, this);
            registerMethod("getPreferredStandbyMode",
                    &SystemServices::getPreferredStandbyMode, this);
#ifdef ENABLE_SYSTEM_GET_STORE_DEMO_LINK
            registerMethod("getStoreDemoLink", &SystemServices::getStoreDemoLink, this);
#endif
            registerMethod("getPowerStateIsManagedByDevice", &SystemServices::getPowerStateIsManagedByDevice, this);
            registerMethod("setFirmwareRebootDelay", &SystemServices::setFirmwareRebootDelay, this);
	        registerMethod("getWakeupSrcConfiguration", &SystemServices::getWakeupSrcConfiguration, this);
            registerMethod("getPreviousRebootInfo",
                    &SystemServices::getPreviousRebootInfo, this);
            registerMethod("getLastDeepSleepReason",
                    &SystemServices::getLastDeepSleepReason, this);
            registerMethod("clearLastDeepSleepReason",
                    &SystemServices::clearLastDeepSleepReason, this);
            registerMethod("enableMoca", &SystemServices::requestEnableMoca, this);
            registerMethod("queryMocaStatus", &SystemServices::queryMocaStatus, this);
            registerMethod("getStateInfo", &SystemServices::getStateInfo, this);
            registerMethod("setGzEnabled", &SystemServices::setGZEnabled, this);
            registerMethod("isGzEnabled", &SystemServices::isGZEnabled, this);
            registerMethod("getMode", &SystemServices::getMode, this);
            registerMethod("setBootLoaderPattern", &SystemServices::setBootLoaderPattern, this);
            registerMethod("setPreferredStandbyMode",
                    &SystemServices::setPreferredStandbyMode, this);
            registerMethod("getAvailableStandbyModes",
                    &SystemServices::getAvailableStandbyModes, this);
            registerMethod("getPreviousRebootInfo2",
                    &SystemServices::getPreviousRebootInfo2, this);
            registerMethod("getPreviousRebootReason",
                    &SystemServices::getPreviousRebootReason, this);
            registerMethod("getMilestones", &SystemServices::getMilestones, this);
            registerMethod("enableXREConnectionRetention",
                    &SystemServices::enableXREConnectionRetention, this);
            registerMethod("fireFirmwarePendingReboot", &SystemServices::fireFirmwarePendingReboot, this);
            registerMethod("deletePersistentPath", &SystemServices::deletePersistentPath, this);
            registerMethod("getThunderStartReason", &SystemServices::getThunderStartReason, this);
            registerMethod("getCoreTemperature", &SystemServices::getCoreTemperature,
                    this);
#ifdef ENABLE_THERMAL_PROTECTION
            registerMethod("getTemperatureThresholds",
                    &SystemServices::getTemperatureThresholds, this);
            registerMethod("setTemperatureThresholds",
                    &SystemServices::setTemperatureThresholds, this);
	        registerMethod("getOvertempGraceInterval",
                    &SystemServices::getOvertempGraceInterval, this);
            registerMethod("setOvertempGraceInterval",
                    &SystemServices::setOvertempGraceInterval, this);
#endif /* ENABLE_THERMAL_PROTECTION */
#endif
        }

        SystemServices::~SystemServices()
        {
            regfree (&m_regexUnallowedChars);
        }

        const string SystemServices::Initialize(PluginHost::IShell* service)
        {
#if defined(USE_IARMBUS) || defined(USE_IARM_BUS)
            InitializeIARM();
#endif /* defined(USE_IARMBUS) || defined(USE_IARM_BUS) */
            m_shellService = service;
            m_shellService->AddRef();
            InitializePowerManager();

            //Initialise timer with interval and callback function.
            m_operatingModeTimer.setInterval(updateDuration, MODE_TIMER_UPDATE_INTERVAL);

            //first boot? then set to NORMAL mode
            if (!m_temp_settings.contains("mode") && m_currentMode == "") {
                JsonObject mode,param,response;
                param["duration"] = -1;
                param["mode"] = MODE_NORMAL;
                mode["modeInfo"] = param;

                LOGINFO("first boot so setting mode to '%s' ('%s' does not contain(\"mode\"))\n",
                        (param["mode"].String()).c_str(), SYSTEM_SERVICE_TEMP_FILE);

                setMode(mode, response);
            } else if (m_currentMode.empty()) {
                JsonObject mode,param,response;
                param["duration"] = m_temp_settings.getValue("mode_duration");
                param["mode"] = m_temp_settings.getValue("mode");
                mode["modeInfo"] = param;

                LOGINFO("receiver restarted so setting mode:%s duration:%d\n",
                        (param["mode"].String()).c_str(), (int)param["duration"].Number());

                setMode(mode, response);
            }

#ifdef DISABLE_GEOGRAPHY_TIMEZONE
            std::string timeZone = getTimeZoneDSTHelper();

            if (!timeZone.empty()) {
                std::string tzenv = ":";
                tzenv += timeZone;
                Core::SystemInfo::SetEnvironment(_T("TZ"), tzenv.c_str());
            }
#endif
            RFC_ParamData_t param = {0};
            WDMP_STATUS status = getRFCParameter((char*)"thunderapi", TR181_SYSTEM_FRIENDLY_NAME, &param);
            if(WDMP_SUCCESS == status && param.type == WDMP_STRING)
            {
                m_friendlyName = param.value;
                LOGINFO("Success Getting the friendly name value :%s \n",m_friendlyName.c_str());
            }

            /* On Success; return empty to indicate no error text. */
            return (string());
        }

        void SystemServices::Deinitialize(PluginHost::IShell*)
        {
            if (_powerManagerPlugin) {
                _powerManagerPlugin->Unregister(_pwrMgrNotification.baseInterface<Exchange::IPowerManager::INetworkStandbyModeChangedNotification>());
                _powerManagerPlugin->Unregister(_pwrMgrNotification.baseInterface<Exchange::IPowerManager::IThermalModeChangedNotification>());
                _powerManagerPlugin->Unregister(_pwrMgrNotification.baseInterface<Exchange::IPowerManager::IRebootNotification>());
                _powerManagerPlugin->Unregister(_pwrMgrNotification.baseInterface<Exchange::IPowerManager::IModeChangedNotification>());		    
                _powerManagerPlugin.Reset();
            }

            _registeredEventHandlers = false;
            m_operatingModeTimer.stop();
#if defined(USE_IARMBUS) || defined(USE_IARM_BUS)
            DeinitializeIARM();
#endif /* defined(USE_IARMBUS) || defined(USE_IARM_BUS) */
            SystemServices::_instance = nullptr;
            m_shellService->Release();
            m_shellService = nullptr;
        }

        void SystemServices::InitializePowerManager()
        {
            LOGINFO("Connect the COM-RPC socket\n");
            _powerManagerPlugin = PowerManagerInterfaceBuilder(_T("org.rdk.PowerManager"))
                .withIShell(m_shellService)
                .withRetryIntervalMS(200)
                .withRetryCount(25)
                .createInterface();

            registerEventHandlers();
        }

#if defined(USE_IARMBUS) || defined(USE_IARM_BUS)
        void SystemServices::InitializeIARM()
        {
            if (Utils::IARM::init())
            {
                IARM_Result_t res;
                IARM_CHECK( IARM_Bus_RegisterCall(IARM_BUS_COMMON_API_SysModeChange, _SysModeChange));
                IARM_CHECK( IARM_Bus_RegisterEventHandler(IARM_BUS_SYSMGR_NAME, IARM_BUS_SYSMGR_EVENT_SYSTEMSTATE, _systemStateChanged));
                IARM_CHECK( IARM_Bus_RegisterEventHandler(IARM_BUS_SYSMGR_NAME, IARM_BUS_SYSMGR_EVENT_DEVICE_UPDATE_RECEIVED, _deviceMgtUpdateReceived));
#ifdef ENABLE_SYSTIMEMGR_SUPPORT
                IARM_CHECK( IARM_Bus_RegisterEventHandler(IARM_BUS_SYSTIME_MGR_NAME, cTIMER_STATUS_UPDATE, _timerStatusEventHandler));
#endif// ENABLE_SYSTIMEMGR_SUPPORT
            }
	    
        }

        void SystemServices::DeinitializeIARM()
        {
            if (Utils::IARM::isConnected())
            {
                IARM_Result_t res;
                IARM_CHECK( IARM_Bus_RemoveEventHandler(IARM_BUS_SYSMGR_NAME, IARM_BUS_SYSMGR_EVENT_SYSTEMSTATE, _systemStateChanged));
		        IARM_CHECK( IARM_Bus_RemoveEventHandler(IARM_BUS_SYSMGR_NAME, IARM_BUS_SYSMGR_EVENT_DEVICE_UPDATE_RECEIVED, _deviceMgtUpdateReceived));
            }
        }
#endif /* defined(USE_IARMBUS) || defined(USE_IARM_BUS) */

#ifdef DEBUG
        /**
         * @brief : sampleAPI
         */
        uint32_t SystemServices::sampleAPI(const JsonObject& parameters,
                JsonObject& response)
        {
            response["sampleAPI"] = "Success";
            /* Kept for debug purpose/future reference. */
            sendNotify(EVT_ONSYSTEMSAMPLEEVENT, parameters);
            returnResponse(true);
        }
#endif /* DEBUG */


        IPowerManager* SystemServices::getPwrMgrPluginInstance()
        {
            return _powerManagerPlugin.operator->();
        }

        void SystemServices::registerEventHandlers()
        {
            ASSERT (_powerManagerPlugin);

            if(!_registeredEventHandlers && _powerManagerPlugin) {
                _registeredEventHandlers = true;
                _powerManagerPlugin->Register(_pwrMgrNotification.baseInterface<Exchange::IPowerManager::INetworkStandbyModeChangedNotification>());
                _powerManagerPlugin->Register(_pwrMgrNotification.baseInterface<Exchange::IPowerManager::IThermalModeChangedNotification>());
                _powerManagerPlugin->Register(_pwrMgrNotification.baseInterface<Exchange::IPowerManager::IRebootNotification>());
                _powerManagerPlugin->Register(_pwrMgrNotification.baseInterface<Exchange::IPowerManager::IModeChangedNotification>());
            }
        }

        void SystemServices::onPowerModeChanged(const PowerState currentState, const PowerState newState)
        {
            std::string curPowerState,newPowerState = "";

            curPowerState = powerModeEnumToString(currentState);
            newPowerState = powerModeEnumToString(newState);

            LOGWARN("IARM Event triggered for PowerStateChange.\
                    Old State %s, New State: %s\n",
                    curPowerState.c_str() , newPowerState.c_str());
            // Coverity Fix: ID 35, 36 - COPY_INSTEAD_OF_MOVE: onSystemPowerStateChanged will copy if needed
            if (SystemServices::_instance) {
                SystemServices::_instance->onSystemPowerStateChanged(std::move(curPowerState), std::move(newPowerState));
            } else {
                LOGERR("SystemServices::_instance is NULL.\n");
            }
        }

        std::string SystemServices::powerModeEnumToString(PowerState state)
        {
            std::string powerState = "";
            switch (state) 
            {
                case WPEFramework::Exchange::IPowerManager::POWER_STATE_ON: powerState = "ON"; break;
                case WPEFramework::Exchange::IPowerManager::POWER_STATE_OFF: powerState = "OFF"; break;
                case WPEFramework::Exchange::IPowerManager::POWER_STATE_STANDBY: powerState = "LIGHT_SLEEP"; break;
                case WPEFramework::Exchange::IPowerManager::POWER_STATE_STANDBY_LIGHT_SLEEP: powerState = "LIGHT_SLEEP"; break;
                case WPEFramework::Exchange::IPowerManager::POWER_STATE_STANDBY_DEEP_SLEEP: powerState = "DEEP_SLEEP"; break;
                default: break;
            }
            return powerState;
        }


        void SystemServices::onNetworkStandbyModeChanged(const bool enabled)
        {
            if (SystemServices::_instance) {
                SystemServices::_instance->onNetworkModeChanged(enabled);
            } else {
                LOGERR("SystemServices::_instance is NULL.\n");
            }
        }

        void SystemServices::onThermalModeChanged(const ThermalTemperature currentThermalLevel, const ThermalTemperature newThermalLevel, const float currentTemperature)
        {
            handleThermalLevelChange(currentThermalLevel, newThermalLevel, currentTemperature);
        }

        void SystemServices::onRebootBegin(const string &rebootReasonCustom, const string &rebootReasonOther, const string &rebootRequestor)
        {
            if (SystemServices::_instance) {
                SystemServices::_instance->onPwrMgrReboot(rebootRequestor, rebootReasonOther);
            } else {
                LOGERR("SystemServices::_instance is NULL.\n");
            }
        }

        uint32_t SystemServices::requestSystemReboot(const JsonObject& parameters,
                JsonObject& response)
        {
            Core::hresult status = Core::ERROR_GENERAL;
            bool nfxResult = false;
            string customReason = "No custom reason provided";
            string otherReason = "No other reason supplied";
            string requestor = "SystemServices";
            bool result = false;
            string fname = "nrdplugin";

            nfxResult = Utils::killProcess(fname);
            if (true == nfxResult) {
                LOGINFO("SystemService shutting down Netflix...\n");
                //give Netflix process some time to terminate gracefully.
                sleep(5);
            } else {
                LOGINFO("SystemService unable to shutdown Netflix \
                        process. nfxResult = %ld\n", (long int)nfxResult);
            }

            if (parameters.HasLabel("rebootReason")) {
                customReason = parameters["rebootReason"].String();
                otherReason = customReason;
            }

            LOGINFO("requestSystemReboot: custom reason: %s, other reason: %s\n", customReason.c_str(),
                otherReason.c_str());

            ASSERT (_powerManagerPlugin);
            if (_powerManagerPlugin){
                status = _powerManagerPlugin->Reboot(requestor, customReason, otherReason);
                result = true;
            } else {
                status = Core::ERROR_ILLEGAL_STATE;
            }

            if (status != Core::ERROR_NONE){
                 LOGWARN("requestSystemReboot: powerManagerPlugin->rebooot failed\n");
            }

            response["IARM_Bus_Call_STATUS"] = static_cast <int32_t> (status);

            returnResponse(result);
        }//end of requestSystemReboot

#if 0
        /*
         * @brief This function delays the reboot in seconds.
         * This will internally sets the tr181 fwDelayReboot parameter.
         * @param1[in]: {"jsonrpc":"2.0","id":"3","method":"org.rdk.System.2.setFirmwareRebootDelay",
         *                  "params":{"delaySeconds": int seconds}}''
         * @param2[out]: {"jsonrpc":"2.0","id":3,"result":{"success":<bool>}}
         * @return: Core::<StatusCode>
         */

        uint32_t SystemServices::setFirmwareRebootDelay(const JsonObject& parameters,
                JsonObject& response)
        {
            bool result = false;
            uint32_t delay_in_sec = 0;

            if ( parameters.HasLabel("delaySeconds") ){
                /* get the value */
                delay_in_sec = static_cast<unsigned int>(parameters["delaySeconds"].Number());

                /* we can delay with max 24 Hrs = 86400 sec */
                if (delay_in_sec > 0 && delay_in_sec <= MAX_REBOOT_DELAY ){

                    std::string delaySeconds = parameters["delaySeconds"].String();
                    const char * set_rfc_val = delaySeconds.c_str();

                    LOGINFO("set_rfc_value %s\n",set_rfc_val);

                    /*set tr181Set command from here*/
                    WDMP_STATUS status = setRFCParameter((char*)"thunderapi",
                            TR181_FW_DELAY_REBOOT, set_rfc_val, WDMP_INT);
                    if ( WDMP_SUCCESS == status ){
                        result=true;
                        LOGINFO("Success Setting setFirmwareRebootDelay value\n");
                    }
                    else {
                        LOGINFO("Failed Setting setFirmwareRebootDelay value %s\n",getRFCErrorString(status));
                    }
                }
                else {
                    /* we didnt get a valid Auto Reboot delay */
                    LOGERR("Invalid setFirmwareRebootDelay Value Max.Value is 86400 sec\n");
                }
            }
            else {
                /* havent got the correct label */
                LOGERR("setFirmwareRebootDelay Missing Key Values\n");
                populateResponseWithError(SysSrv_MissingKeyValues,response);
            }
            returnResponse(result);
        }
#endif

        /*
         * @brief This function Enable/Disable the AutReboot Feature.
         * This will internally sets the tr181 AutoReboot.Enable to True/False.
         * @param1[in]: {"jsonrpc":"2.0","id":"3","method":"org.rdk.System.2.setFirmwareAutoReboot",
         *                  "params":{"enable": bool }}''
         * @param2[out]: {"jsonrpc":"2.0","id":3,"result":{"success":<bool>}}
         * @return: Core::<StatusCode>
         */

        uint32_t SystemServices::setFirmwareAutoReboot(const JsonObject& parameters,
                JsonObject& response)
        {
            bool result = false;
            bool enableFwAutoreboot = false;

           if ( parameters.HasLabel("enable") ){
               /* get the value */
               enableFwAutoreboot = (parameters["enable"].Boolean());
               LOGINFO("setFirmwareAutoReboot : %s\n",(enableFwAutoreboot)? "true":"false");

               std::string enable = parameters["enable"].String();
               const char * set_rfc_val = enable.c_str();

               /* set tr181Set command from here */
               WDMP_STATUS status = setRFCParameter((char*)"thunderapi",
                       TR181_AUTOREBOOT_ENABLE,set_rfc_val,WDMP_BOOLEAN);
               if ( WDMP_SUCCESS == status ){
                   result=true;
                   LOGINFO("Success Setting the setFirmwareAutoReboot value\n");
               }
               else {
                   LOGINFO("Failed Setting the setFirmwareAutoReboot value %s\n",getRFCErrorString(status));
               }
           }
           else {
               /* havent got the correct label */
               LOGERR("setFirmwareAutoReboot Missing Key Values\n");
               populateResponseWithError(SysSrv_MissingKeyValues,response);
           }
           returnResponse(result);
        }

#if 0
        /*
         * @brief This function notifies about pending Reboot.
         * This will internally set 120 sec and trigger event to application.
         * @param1[in]: {"jsonrpc":"2.0","id":"3","method":"org.rdk.System.2.fireFirmwarePendingReboot",
         *                  "params":{}}
         * @param2[out]: {"jsonrpc":"2.0","id":3,"result":{"success":true}}
         * @return: Core::<StatusCode>
         */

        uint32_t SystemServices::fireFirmwarePendingReboot(const JsonObject& parameters,
                JsonObject& response)
        {
            bool result = false;
            int seconds = 600; /* 10 Minutes to Reboot */

            /* trigger event saying we are in Maintenance Window */

            /* check if we have valid instance */
            if ( _instance ){
                /* clear any older values, Reset the fwDelayReboot = 0 */
                LOGINFO("Reset Older FwDelayReboot to 0, if any\n");

                WDMP_STATUS status = setRFCParameter((char*)"thunderapi",
                        TR181_FW_DELAY_REBOOT,"0", WDMP_INT);

                /* call the event handler if reset SUCCESS */
                if ( WDMP_SUCCESS == status ){
                    /* trigger event saying we are in Maintenance Window */
                    _instance->onFirmwarePendingReboot(seconds);
                    result=true;
                }
                else {
                    LOGINFO("Failed to reset FwDelayReboot due to %s\n",getRFCErrorString(status));
                }
            }
            else {
                LOGERR("_instance in fireFirmwarePendingReboot is NULL.\n");
            }

            returnResponse(result);
        }
#endif

        /*
         * @brief : send event when system is in maintenance window
         * @param1[in]  : int seconds
         */

        void SystemServices::onFirmwarePendingReboot(int seconds)
        {
            JsonObject params;
            params["fireFirmwarePendingReboot"] = seconds;
            LOGINFO("Notifying onFirmwarePendingReboot received \n");
            sendNotify(EVT_ONFWPENDINGREBOOT, params);
        }

        /***
         * @brief : send notification when system power state is changed
         *
         * @param1[in]  : powerState
         * @param2[out] : {"jsonrpc": "2.0",
         *		"method": "org.rdk.SystemServices.events.1.onSystemPowerStateChanged",
         *		"param":{"powerState": <string new power state mode>}}
         */
        void SystemServices::onSystemPowerStateChanged(string currentPowerState, string powerState)
        {

            if ("LIGHT_SLEEP" == powerState || "STANDBY" == powerState) {
                if ("ON" == currentPowerState) {
                    RFC_ParamData_t param = {0};
                    WDMP_STATUS status = getRFCParameter(NULL, RFC_LOG_UPLOAD, &param);
                    if(WDMP_SUCCESS == status && param.type == WDMP_BOOLEAN && (strncasecmp(param.value,"true",4) == 0))
                    {
                        JsonObject p;
                        JsonObject r;
                        uploadLogsAsync(p, r);
                    }
                }
            } else if ("DEEP_SLEEP" == powerState) {

                pid_t uploadLogsPid = -1;

                {
                    lock_guard<mutex> lck(m_uploadLogsMutex);
                    uploadLogsPid = m_uploadLogsPid;
                }

                if (-1 != uploadLogsPid)
                {
                    JsonObject p;
                    JsonObject r;
                    abortLogUpload(p, r);
                }
            }

            JsonObject params;
            params["powerState"] = powerState;
            params["currentPowerState"] = currentPowerState;
            LOGWARN("power state changed from '%s' to '%s'", currentPowerState.c_str(), powerState.c_str());

			char value[256] = {0};
			snprintf(value, sizeof(value), "power state changed from");
			t2_event_s((char*)"PwrStateChng_split", value);

			if (currentPowerState == "ON" && powerState == "LIGHT_SLEEP")
			{
				t2_event_d((char*)"SYST_INFO_ThunderSleep1", 1);
			}
			else if (currentPowerState == "LIGHT_SLEEP" && powerState == "DEEP_SLEEP")
			{
				t2_event_d((char*)"SYST_INFO_ThunderSleep2", 1);
			}
			else if (currentPowerState == "DEEP_SLEEP" && powerState == "LIGHT_SLEEP")
			{
				t2_event_d((char*)"SYST_INFO_ThunderWake1", 1);
			}
			else if (currentPowerState == "LIGHT_SLEEP" && powerState == "ON")
			{
				t2_event_d((char*)"SYST_INFO_ThunderWake2", 1);
			}
            sendNotify(EVT_ONSYSTEMPOWERSTATECHANGED, params);
        }

        void SystemServices::onPwrMgrReboot(string requestedApp, string rebootReason)
        {
            JsonObject params;
            params["requestedApp"] = requestedApp;
            params["rebootReason"] = rebootReason;

            sendNotify(EVT_ONREBOOTREQUEST, params);
        }

        void SystemServices::onNetworkModeChanged(bool bNetworkStandbyMode)
        {
            m_networkStandbyMode = bNetworkStandbyMode;
            m_networkStandbyModeValid = true;
            JsonObject params;
            params["nwStandby"] = bNetworkStandbyMode;
            sendNotify(EVT_ONNETWORKSTANDBYMODECHANGED , params);
        }

#if 0
        /**
         * @breif : to enable Moca Settings
         * @param1[in] : {"params":{"value":true}}
         * @param2[out] :  {"success":<bool>}
         */
        uint32_t SystemServices::requestEnableMoca(const JsonObject& parameters,
                JsonObject& response)
        {
            int32_t eRetval = E_NOK;
            bool enableMoca = false;
            ofstream mocaFile;
             if (parameters.HasLabel("value")) {
                 enableMoca = parameters["value"].Boolean();
                 if (enableMoca) {
                 mocaFile.open(MOCA_FILE, ios::out);
                     if (mocaFile) {
                         mocaFile.close();
                         eRetval = v_secure_system("/etc/init.d/moca_init start");
                     } else {
                         LOGERR("moca file open failed\n");
                         populateResponseWithError(SysSrv_FileAccessFailed, response);
                     }
                 } else {
                     std::remove(MOCA_FILE);
                     if (!Utils::fileExists(MOCA_FILE)) {
                         eRetval = v_secure_system("/etc/init.d/moca_init start");
                     } else {
                         LOGERR("moca file remove failed\n");
                         populateResponseWithError(SysSrv_FileAccessFailed, response);
                     }
                 }
            } else {
                populateResponseWithError(SysSrv_MissingKeyValues, response);
            }
            LOGERR("eRetval = %d\n", eRetval);
            returnResponse((E_OK == eRetval)? true: false);
        } //End of requestEnableMoca
#endif
        /**
         * @brief  : To fetch system uptime
         * @param2[out] : {"result":{"systemUptime":"378641.03","success":true}}
         */
        uint32_t SystemServices::requestSystemUptime(const JsonObject& parameters,
                JsonObject& response)
        {
            struct timespec time;
            bool result = false;

            if (clock_gettime(CLOCK_MONOTONIC_RAW, &time) == 0)
            {
                float uptime = (float)time.tv_sec + (float)time.tv_nsec / 1e9;
                std::string value = std::to_string(uptime);
                value = value.erase(value.find_last_not_of("0") + 1);

                if (value.back() == '.')
                    value += '0';

                response["systemUptime"] = value;
                LOGINFO("uptime is %s seconds", value.c_str());
                result = true;
                }
            else
                LOGERR("unable to evaluate uptime by clock_gettime");

            returnResponse(result);
        }

        /**
         * @brief : API to query DeviceInfo details
         *
         * @param1[in]  : {"params":{"params":["<key>"]}}
         * @param2[out] : "result":{<key>:<Device Info Details>,"success":<bool>}
         * @return      : Core::<StatusCode>
         */
        uint32_t SystemServices::getDeviceInfo(const JsonObject& parameters,
                JsonObject& response)
        {
            bool retAPIStatus = false;
            string queryParams;

            if (parameters.HasLabel("params")) {
                queryParams = parameters["params"].String();
                removeCharsFromString(queryParams, "[\"]");

                regmatch_t  m_regmatchUnallowedChars[1];
                if (REG_NOERROR == regexec(&m_regexUnallowedChars, queryParams.c_str(), 1, m_regmatchUnallowedChars, 0))
                {
                    response["message"] = "Input has unallowable characters";
                    LOGERR("Input has unallowable characters: '%s'", queryParams.c_str());

                    returnResponse(false);
                }

           }

            // there is no /tmp/.make from /lib/rdk/getDeviceDetails.sh, but it can be taken from /etc/device.properties
            if (queryParams.empty() || queryParams == "make") {
                std::string device_name{};
                GetValueFromPropertiesFile(DEVICE_PROPERTIES_FILE, "DEVICE_NAME", device_name);
				if (device_name == "PLATCO") {
                    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
					memset(&param, 0, sizeof(param));
                    param.type = mfrSERIALIZED_TYPE_MANUFACTURER;

                    IARM_Result_t result = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));
                    param.buffer[param.bufLen] = '\0';
                    LOGINFO("SystemService getDeviceInfo param type %d result %s bufLen = %d", param.type, param.buffer, param.bufLen);

                    if (result == IARM_RESULT_SUCCESS) {
                        response["make"] = string(param.buffer);
                        retAPIStatus = true;
				       } else {
                        LOGERR("IARM_BUS_MFRLIB_API_GetSerializedData call was failed");
						populateResponseWithError(SysSrv_MissingKeyValues, response); // Set an error in the response
                        retAPIStatus = false;
					}
				} else {
                std::string make;
                GetValueFromPropertiesFile(DEVICE_PROPERTIES_FILE, "MFG_NAME", make);

                if (make.size() > 0) {
                    response["make"] = make;
                    retAPIStatus = true;
                } else {
                    populateResponseWithError(SysSrv_MissingKeyValues, response);
                }
				}
                if (!queryParams.empty()) {


                    returnResponse(retAPIStatus);
                }
            }

            if (!queryParams.compare("model_number") && Core::ERROR_NONE == SetValueFromPropertiesFile(DEVICE_PROPERTIES_FILE, "MODEL_NUM", response, "model_number"))
                returnResponse(true);

            if (!queryParams.compare("imageVersion") && Core::ERROR_NONE == SetValueFromPropertiesFile("/version.txt", "imagename", response, "imageVersion", ":"))
                returnResponse(true);
            
            if (!queryParams.compare("build_type") && Core::ERROR_NONE == SetValueFromPropertiesFile(DEVICE_PROPERTIES_FILE, "BUILD_TYPE", response, "build_type")) {
                string bt = response["build_type"].String();
                Utils::String::toUpper(bt);
                response["build_type"] = bt;
                returnResponse(true);
            }

            if (!queryParams.compare("device_type") && Core::ERROR_NONE == SetValueFromPropertiesFile(DEVICE_PROPERTIES_FILE, "DEVICE_TYPE", response, "device_type"))
                returnResponse(true);

#ifdef ENABLE_DEVICE_MANUFACTURER_INFO
            if (!queryParams.compare(MODEL_NAME) || !queryParams.compare(HARDWARE_ID)) {
                returnResponse(getManufacturerData(queryParams, response));
		}

	    if(!queryParams.compare(FRIENDLY_ID))
	    {
		    if(getModelName(queryParams, response))
			    returnResponse(true);
            }
#endif
            std::string cmd = "";
            if (!queryParams.empty()) {
                cmd += queryParams;
            }

            std::string res = "";
            FILE* pipe = v_secure_popen("r", "/lib/rdk/getDeviceDetails.sh %s %s", GET_STB_DETAILS_SCRIPT_READ_COMMAND, cmd.c_str());
            if(pipe){
              char buff[1024] = { '\0' };
              while (fgets(buff, sizeof(buff), pipe)) {
                    res += buff;
                    memset(buff, 0, sizeof(buff));
              }
              v_secure_pclose(pipe);
            }
            if (res.size() > 0) {
                std::string model_number;
		std::string device_type;
                if (queryParams.empty()) {
                    retAPIStatus = true;

                    std::stringstream ss(res);
                    std::string line;
                    while(std::getline(ss, line))
                    {
                        size_t eq = line.find_first_of("=");

                        if (std::string::npos != eq)
                        {
                            std::string key = line.substr(0, eq);
                            std::string value = line.substr(eq + 1);

                            response[key.c_str()] = value;

                            // some tweaks for backward compatibility
                            if (key == "imageVersion") {
                                response["version"] = value; 
                                response["software_version"] = value;
                            }
                            else if (key == "cableCardVersion") {
                                response["cable_card_firmware_version"] = value;
                            }
                            else if (key == "model_number") {
                                // Coverity Fix: IDs 37-38 - COPY_INSTEAD_OF_MOVE: Use std::move for assignments
                                model_number = std::move(value);
                            }
			    else if(key == "device_type") {
				device_type = std::move(value);
			    }
                        }
                    }
#ifdef ENABLE_DEVICE_MANUFACTURER_INFO
                    queryParams = FRIENDLY_ID;
                    getModelName(queryParams, response);
#endif
                } else {
                    retAPIStatus = true;
                    Utils::String::trim(res);
                        response[queryParams.c_str()] = res;
                    }
                }
            returnResponse(retAPIStatus);
        }
#ifdef ENABLE_DEVICE_MANUFACTURER_INFO


	bool SystemServices::getModelName(const string& parameter, JsonObject& response)
	{
		LOGWARN("SystemService getDeviceInfo query %s", parameter.c_str());

		bool status = false;
		std::string device_name{};
		std::string friendly_id;
		GetValueFromPropertiesFile(DEVICE_PROPERTIES_FILE, "DEVICE_NAME", device_name);

		if ((device_name == "PLATCO") || (device_name == "LLAMA")) {
		IARM_Bus_MFRLib_GetSerializedData_Param_t param;
		param.bufLen = 0;
		param.type = mfrSERIALIZED_TYPE_PROVISIONED_MODELNAME;
		IARM_Result_t result = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));
		param.buffer[param.bufLen] = '\0';
		LOGWARN("SystemService getDeviceInfo param type %d result %s", param.type, param.buffer);
		if (result == IARM_RESULT_SUCCESS && param.bufLen) {
			response[parameter.c_str()] = string(param.buffer);
			status = true;
		}
		else{
			LOGWARN("SystemService getDeviceInfo - Manufacturer Data Read Failed");
			GetFileRegex(DEVICE_PROPERTIES_FILE, std::regex("^FRIENDLY_ID(?:\\s*)=(?:\\s*)(?:\"{0,1})([^\"\\n]+)(?:\"{0,1})(?:\\s*)$"), friendly_id);
                        if (friendly_id.size() > 0) {
                                response[parameter.c_str()] = friendly_id;
                                status = true;
                        } else {
                                populateResponseWithError(SysSrv_MissingKeyValues, response);
                        }
		}
		}
		else {
			GetFileRegex(DEVICE_PROPERTIES_FILE, std::regex("^FRIENDLY_ID(?:\\s*)=(?:\\s*)(?:\"{0,1})([^\"\\n]+)(?:\"{0,1})(?:\\s*)$"), friendly_id);
			if (friendly_id.size() > 0) {
				response[parameter.c_str()] = friendly_id;
				status = true;
			} else {
				populateResponseWithError(SysSrv_MissingKeyValues, response);
			}
		}
		return status;
	}

        /***
         * @brief : To retrieve Manufacturing Serial Number.
         * @param1[in] : {"params":{}}
         * @param2[out] : {"result":{"mfgSerialNumber":"<string>","success":<bool>}}
         */
        uint32_t SystemServices::getMfgSerialNumber(const JsonObject& parameters, JsonObject& response)
        {
            LOGWARN("SystemService getMfgSerialNumber query");

            if (m_MfgSerialNumberValid) {
                response["mfgSerialNumber"] = m_MfgSerialNumber;
                LOGWARN("Got cached MfgSerialNumber %s", m_MfgSerialNumber.c_str());
                returnResponse(true);
            }

            IARM_Bus_MFRLib_GetSerializedData_Param_t param;
            param.bufLen = 0;
            param.type = mfrSERIALIZED_TYPE_MANUFACTURING_SERIALNUMBER;
            IARM_Result_t result = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));
            param.buffer[param.bufLen] = '\0';

            bool status = false;
            if (result == IARM_RESULT_SUCCESS) {
                response["mfgSerialNumber"] = string(param.buffer);
                status = true;

                m_MfgSerialNumber = string(param.buffer);
                m_MfgSerialNumberValid = true;

                LOGWARN("SystemService getMfgSerialNumber Manufacturing Serial Number: %s", param.buffer);
            } else {
                LOGERR("SystemService getMfgSerialNumber Manufacturing Serial Number: NULL");
            }

            returnResponse(status);;
        }

        bool SystemServices::getManufacturerData(const string& parameter, JsonObject& response)
        {
            LOGWARN("SystemService getDeviceInfo query %s", parameter.c_str());

	    if (m_ManufacturerDataModelNameValid && !parameter.compare(MODEL_NAME)) {
		    response[parameter.c_str()] = m_ManufacturerDataModelName;
		    LOGWARN("Got cached ManufacturerData %s", m_ManufacturerDataModelName.c_str());
		    return true;
	    }

	    if (m_ManufacturerDataHardwareIdValid && !parameter.compare(HARDWARE_ID)) {
		    response[parameter.c_str()] = m_ManufacturerDataHardwareID;
		    LOGWARN("Got cached ManufacturerData %s", m_ManufacturerDataHardwareID.c_str());
		    return true;
	    }


            IARM_Bus_MFRLib_GetSerializedData_Param_t param;
            param.bufLen = 0;
            param.type = mfrSERIALIZED_TYPE_MANUFACTURER;
            if (!parameter.compare(MODEL_NAME)) {
                param.type = mfrSERIALIZED_TYPE_PROVISIONED_MODELNAME;
            } else if (!parameter.compare(HARDWARE_ID)) {
                param.type = mfrSERIALIZED_TYPE_HWID;
            }
            IARM_Result_t result = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));
            param.buffer[param.bufLen] = '\0';

            LOGWARN("SystemService getDeviceInfo param type %d result %s", param.type, param.buffer);

            bool status = false;
            if (result == IARM_RESULT_SUCCESS) {
                response[parameter.c_str()] = string(param.buffer);
                status = true;
		if(!parameter.compare(MODEL_NAME)){
			m_ManufacturerDataModelName = param.buffer;
			m_ManufacturerDataModelNameValid = true;
		}
		else if (!parameter.compare(HARDWARE_ID)) {
			m_ManufacturerDataHardwareID = param.buffer;
			m_ManufacturerDataHardwareIdValid = true;
		}
            } else {
                populateResponseWithError(SysSrv_ManufacturerDataReadFailed, response);
            }

            return status;
        }
#endif

#if 0
        /***
         * @brief : Checks if Moca is Enabled or Not.
         *
         * @param1[in]  : {"params":{}}
         * @param2[out] : "result":{"mocaEnabled":<bool>,"success":<bool>}
         * @return      : Core::<StatusCode>
         */
        uint32_t SystemServices::queryMocaStatus(const JsonObject& parameters,
                JsonObject& response)
        {
            response["mocaEnabled"] = getMocaStatus();
            returnResponse(true);
        }
#endif
        /***
         * @brief : sends notification when system mode has changed.
         *
         * @param1[in]  : state
         * @param2[out] : {"mode": <string_mode>}
         */
        void SystemServices::onSystemModeChanged(string mode)
        {
            JsonObject params;
            params["mode"] = mode;
            LOGINFO("mode changed to '%s'\n", mode.c_str());
            sendNotify(EVT_ONSYSTEMMODECHANGED, params);
        }

        /***
         * @brief Initiates a firmware update.
         * This has no affect if update is not available. The State Observer API
         * may be used to listen to firmware update events.
         *
         * @param1[in]	: {"params":{}}
         * @param2[out]	: {"jsonrpc":"2.0","id":3,"result":{"success":<bool>}}
         * @return		: Core::<StatusCode>
         */
        uint32_t SystemServices::updateFirmware(const JsonObject& parameters,
                JsonObject& response)
        {
            LOGWARN("SystemService updatingFirmware\n");
            FILE* pipe = v_secure_popen("r", "/usr/bin/rdkvfwupgrader 0 4 >> /opt/logs/swupdate.log &");
            if(pipe){
               v_secure_pclose(pipe);
            }
            returnResponse(true);
        }

#if 0
        /***
         * @brief : Returns mode Information, defines two parameters mode and duration.
         *
         * @param1[in]	: {"params":{}}
         * @param2[out]	: {"result":{"modeInfo":{"mode":"<string>","duration":<int>},"success":<bool>}}
         * @return		: Core::<StatusCode>
         */
        uint32_t SystemServices::getMode(const JsonObject& parameters,
                JsonObject& response)
        {
            JsonObject modeInfo;
            LOGWARN("current mode: '%s', duration: %d\n",
                    m_currentMode.c_str(), m_remainingDuration);
            modeInfo["mode"] = m_currentMode.c_str();
            modeInfo["duration"] = m_remainingDuration;
            response["modeInfo"] = modeInfo;
            returnResponse(true);
        }

        /***
         * @brief : Sets the bootloader pattern to MFR. 
         * @param1[in]  : {"pattern":"<string>"}
         * @param2[out] : {"result":{"success":<bool>}}
         * @return              : Core::<StatusCode>
         */
        uint32_t SystemServices::setBootLoaderPattern(const JsonObject& parameters,
                JsonObject& response)
        {
                returnIfParamNotFound(parameters, "pattern");
                bool status = false;
                IARM_Bus_MFRLib_SetBLPattern_Param_t mfrparam;
                mfrparam.pattern = mfrBL_PATTERN_NORMAL;
                string strBLPattern = parameters["pattern"].String();
                if (strBLPattern == "NORMAL") {
                    mfrparam.pattern = mfrBL_PATTERN_NORMAL;
                    status = true;
                }
                else if (strBLPattern == "SILENT") {
                    mfrparam.pattern = mfrBL_PATTERN_SILENT;
                    status = true;
                }
                else if (strBLPattern == "SILENT_LED_ON") {
                    mfrparam.pattern = mfrBL_PATTERN_SILENT_LED_ON;
                    status = true;
                }
                LOGWARN("setBootLoaderPattern :%d \n", mfrparam.pattern);
                if(status == true)
                {
                   if (IARM_RESULT_SUCCESS != IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_SetBootLoaderPattern, (void *)&mfrparam, sizeof(mfrparam))){
                        status = false;
                   }
                }
                returnResponse(status);
        }
#endif

        /***
         * @brief : To update bootloader splash screen.
         * @param1[in]  : {"path":"<string>"}
         * @param2[out] : {"result":{"success":<bool>}}
         * @return              : Core::<StatusCode>
         */
        uint32_t SystemServices::setBootLoaderSplashScreen(const JsonObject& parameters,
                JsonObject& response)
        {                
                bool status = false;
                string strBLSplashScreenPath = parameters["path"].String();
		bool fileExists = Utils::fileExists(strBLSplashScreenPath.c_str());
                if((strBLSplashScreenPath != "") && fileExists)
		{
			IARM_Bus_MFRLib_SetBLSplashScreen_Param_t mfrparam;
			std::strncpy(mfrparam.path, strBLSplashScreenPath.c_str(), sizeof(mfrparam.path));
            mfrparam.path[sizeof(mfrparam.path) - 1] = '\0';
			IARM_Result_t result = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_SetBlSplashScreen, (void *)&mfrparam, sizeof(mfrparam));
			if (result != IARM_RESULT_SUCCESS){
				LOGERR("Update failed. path: %s, fileExists %s, IARM result %d ",strBLSplashScreenPath.c_str(),fileExists ? "true" : "false",result);
				JsonObject error;
				error["message"] = "Update failed";
				error["code"] = "-32002";
				response["error"] = error;
				status = false;
			}
			else 
			{
				LOGINFO("BootLoaderSplashScreen updated successfully");
				status =true;
			}
		}
		else
		{
			LOGERR("Invalid path. path: %s, fileExists %s ",strBLSplashScreenPath.c_str(),fileExists ? "true" : "false");
			JsonObject error;
			error["message"] = "Invalid path";
			error["code"] = "-32001";
			response["error"] = error;
			status = false;
		}
                returnResponse(status);
        }

        /***
         * @brief : Sets the mode of the STB. The object consists of two properties, mode (String) and
         * duration (Integer) mode can be one of following:
         * - NORMAL - the STB is operating in normal mode
         * - EAS - the STB is operating in EAS mode. This mode is set when the device needs
         *   to perform certain tasks when entering EAS mode, such as setting the clock display,
         *   or preventing the user from using the diagnostics menu.
         * - WAREHOUSE - the STB is operating in warehouse mode.
         *
         * @param1[in]	: {"modeInfo":{"mode":"<string>","duration":<int>}}
         * @param2[out]	: {"result":{"success":<bool>}}
         * @return		: Core::<StatusCode>
         */
        uint32_t SystemServices::setMode(const JsonObject& parameters,
                JsonObject& response)
        {
            bool changeMode  = true;
            JsonObject param;
            std::string oldMode = m_currentMode;
            bool result = true;

            if (parameters.HasLabel("modeInfo")) {
                param.FromString(parameters["modeInfo"].String());
                if (param.HasLabel("duration") && param.HasLabel("mode")) {
                    int duration = param["duration"].Number();
                    std::string newMode = param["mode"].String();

                    LOGWARN("request to switch to mode '%s' from mode '%s' \
                            with duration %d\n", newMode.c_str(),
                            oldMode.c_str(), duration);

                    if (MODE_NORMAL != newMode && MODE_WAREHOUSE != newMode &&
                            MODE_EAS != newMode) {
                        LOGERR("value of new mode is incorrect, therefore \
                                current mode '%s' not changed.\n", oldMode.c_str());
                        returnResponse(false);
                    }
                    if (MODE_NORMAL == m_currentMode && (0 == duration ||
                                (0 != duration && MODE_NORMAL == newMode))) {
                        changeMode = false;
                    } else if (MODE_NORMAL != newMode && 0 != duration) {
                        m_currentMode = newMode;
                        duration < 0 ? stopModeTimer() : startModeTimer(duration);
                    } else {
                        m_currentMode = MODE_NORMAL;
                        stopModeTimer();
                    }

                    if (changeMode) {
                        IARM_Bus_CommonAPI_SysModeChange_Param_t modeParam;
                        // Coverity Fix: ID 21 - COPY_INSTEAD_OF_MOVE
                        stringToIarmMode(std::move(oldMode), modeParam.oldMode);
                        stringToIarmMode(m_currentMode, modeParam.newMode);

                        if (IARM_RESULT_SUCCESS == IARM_Bus_Call(IARM_BUS_DAEMON_NAME,
                                    "DaemonSysModeChange", &modeParam, sizeof(modeParam))) {
                            LOGWARN("switched to mode '%s'\n", m_currentMode.c_str());

                            if (MODE_NORMAL != m_currentMode && duration < 0) {
                                LOGWARN("duration is negative, therefore \
                                        mode timer stopped and Receiver will keep \
                                        mode '%s', untill changing it in next call",
                                        m_currentMode.c_str());
                            }
                        } else {
                            stopModeTimer();
                            m_currentMode = MODE_NORMAL;
                            LOGERR("failed to switch to mode '%s'. Receiver \
                                    forced to switch to '%s'", newMode.c_str(), m_currentMode.c_str());
                            result = false;
                        }

                        if (MODE_WAREHOUSE == m_currentMode){
                            FILE *fp = fopen(WAREHOUSE_MODE_FILE, "w+");
                            if(!fp){
                                LOGWARN("Unable to create [%s] file ",WAREHOUSE_MODE_FILE);
                            }
                            else{
                                fclose(fp);
                            }
                        }
                        else {
                            if(Utils::fileExists(WAREHOUSE_MODE_FILE)){
                                if(0 != unlink(WAREHOUSE_MODE_FILE)){
                                    LOGWARN("Unlink is failed for [%s] file",WAREHOUSE_MODE_FILE);
                                }
                            }
                        }
                        //set values in temp file so they can be restored in receiver restarts / crashes
                        m_temp_settings.setValue("mode", m_currentMode);
                        m_temp_settings.setValue("mode_duration", m_remainingDuration);
                    } else {
                        LOGWARN("Current mode '%s' not changed", m_currentMode.c_str());
                    }
                } else {
                    populateResponseWithError(SysSrv_MissingKeyValues, response);
                    result = false;
                }
            } else {
                populateResponseWithError(SysSrv_MissingKeyValues, response);
                result = false;
            }

            returnResponse(result);
        }

        void SystemServices::startModeTimer(int duration)
        {
            m_remainingDuration = duration;
            m_operatingModeTimer.start();
            //set values in temp file so they can be restored in receiver restarts / crashes
            m_temp_settings.setValue("mode_duration", m_remainingDuration);
        }

        void SystemServices::stopModeTimer()
        {
            m_remainingDuration = 0;
            m_operatingModeTimer.stop();

            //set values in temp file so they can be restored in receiver restarts / crashes
            // TODO: query & confirm time duration range.
            m_temp_settings.setValue("mode_duration", m_remainingDuration);
        }

        /**
         * @brief This function is used to update duration.
         */
        void SystemServices::updateDuration()
        {
            if (m_remainingDuration > 0) {
                m_remainingDuration--;
            } else {
                m_operatingModeTimer.stop();
		m_operatingModeTimer.detach();
                JsonObject parameters, param, response;
                param["mode"] = "NORMAL";
                param["duration"] = 0;
                parameters["modeInfo"] = param;
                if (_instance) {
                    _instance->setMode(parameters,response);
                } else {
                    LOGERR("_instance is NULL.\n");
                }
            }

            //set values in temp file so they can be restored in receiver restarts / crashes
            m_temp_settings.setValue("mode_duration", m_remainingDuration);
        }

        bool checkOpFlashStoreDir()
        {
            // Coverity Fix: ID 578 - TOCTOU: Use mkdir directly and check for EEXIST to handle race condition
            int ret = mkdir(OPFLASH_STORE, 0774);
            if (ret == 0) {
                LOGINFO(" --- Directory %s created", OPFLASH_STORE);
                return true;
            } else if (errno == EEXIST) {
                // Directory already exists, which is fine
                return true;
            } else {
                LOGERR(" --- Failed to create directory %s: %d", OPFLASH_STORE, ret);
                return false;
            }
        }

        // Function to write (update or append) parameters in the file
        bool write_parameters(const string &filename, const string &param, bool value, bool &update, bool &oldBlocklistFlag) {
            ifstream file_in(filename);
            vector<string> lines;
            bool param_found = false;
        
            // If file exists, read its content line by line
            if (file_in.is_open()) {
                string line;
                while (getline(file_in, line)) {
                    size_t pos = line.find('=');
        
                    // Check if the line contains the parameter we're searching for
                    if (pos != string::npos) {
                        string file_param = line.substr(0, pos);
                        string file_value = line.substr(pos + 1);
                        if (file_param == param) {
                            // check the file value and requested value same
                            if (file_value == (value ? "true" : "false")) {
                                file_in.close();
                                update = false;
                                LOGINFO("Persistence store has updated value. blocklist= %s, update=%d", (value ? "true" : "false"), update);
                                return true;
                            }
                            else {
                                update = true;
                                //store old value for notify.
                                if(file_value == "true"){
                                    oldBlocklistFlag = true;
                                }
                                else if(file_value == "false"){
                                    oldBlocklistFlag = false;
                                }
                                // Update the parameter value
                                line = param + "=" + (value ? "true" : "false");
                                param_found = true;
                            }
                        }
                    }
        
                    // Store the line (updated or not) in memory
                    lines.push_back(line);
                }
        
                file_in.close();
            }
        
            // If the parameter wasn't found in the file, add it
            if (!param_found) {
                lines.push_back(param + "=" + (value ? "true" : "false"));
            }
        
            // Rewrite the entire file with updated values
            ofstream file_out(filename);
            if (!file_out) {
                LOGERR("Error opening file for writing: %s", filename.c_str());
                return false;
            }
        
            for (const auto &line : lines) {
                file_out << line << '\n';
            }
        
            LOGINFO("%s flag stored successfully in persistent memory. update=%d, oldBlocklistFlag=%d", param.c_str(), update, oldBlocklistFlag);
            return true;
        }
        
        // Function to read a parameter from a file and update its value
        bool read_parameters(const string &filename, const string &param, bool &value) {
            ifstream file(filename);
        
            // Check if the file was successfully opened
            if (!file.is_open()) {
                LOGERR("Error opening file for reading: %s", filename.c_str());
                return false;
            }
        
            string line;
            bool param_found = false;
            while (getline(file, line)) {
                // Remove any trailing newline characters
                //line.erase(line.find_last_not_of("\n\r") + 1);
        
                // Split the line into parameter and value using '=' delimiter
                size_t pos = line.find('=');
                if (pos != string::npos) {
                    string file_param = line.substr(0, pos);
                    string file_value = line.substr(pos + 1);
        
                    // Check if this is the parameter we are looking for
                    if (file_param == param) {
                        param_found = true;
                        if (file_value == "true") {
                            value = true;
                        } else if (file_value == "false") {
                            value = false;
                        } else {
                            LOGERR("Error: Invalid value for parameter %s  in file: %s", param.c_str(), file_value.c_str());
                            file.close();
                            return false;  // Invalid value
                        }
                        break;
                    }
                }
            }
        
            // Check if there were any read errors
            if (file.fail() && !file.eof()) {
                LOGERR("Error reading from file: %s", filename.c_str());
                file.close();
                return false;
            }
        
            file.close();
        
            if (!param_found) {
                LOGERR("Parameter %s  not found in the file.", param.c_str());

                return false;
            }
        
            return true;
        }

        /***
         * @brief : sends notification when blocklist flag has changed.
         *
         * @param1[in]  : blocklist flag
         * @param2[out] : {"blocklist": <blocklist_flag>}
         */
        void SystemServices::onBlocklistChanged(bool newBlocklistFlag, bool oldBlocklistFlag)
        {
            JsonObject params;
            string newBloklistStr = (newBlocklistFlag? "true":"false");
            string oldBloklistStr = (oldBlocklistFlag? "true":"false");

            params["oldBlocklistFlag"] = oldBlocklistFlag;
            params["newBlocklistFlag"] = newBlocklistFlag;
            LOGINFO("blocklist changed from %s to '%s'\n", oldBloklistStr.c_str(), newBloklistStr.c_str());
            sendNotify(EVT_ONBLOCKLISTCHANGED, params);
        }

       /***
         * @brief : To update Blocklist flag.
         * @param1[in]  : {"blocklist":"<true/false>"}
         * @param2[out] : {"result":{"success":<bool>}}
         * @return              : Core::<StatusCode>
         */
        uint32_t SystemServices::setBlocklistFlag(const JsonObject& parameters,
                JsonObject& response)
        {                
            bool status = false, update = false, ret;
            bool result = true;
            bool blocklistFlag, oldBlocklistFlag;
            JsonObject error;

            if ((parameters.HasLabel(BLOCKLIST))) {
            
                /*check /opt/secure/persistent/opflashstore/ dir*/
                ret = checkOpFlashStoreDir();
                if(ret == true){
                    LOGINFO("checked opflashstore directory and it is exists. ret = %d",ret);
                }
                else {
                    LOGWARN("failed to create opflashstore directory ret =%d", ret);
                    LOGERR("Blocklist flag update failed. status %d ", status);
				    error["message"] = "Blocklist flag update failed";
				    error["code"] = "-32604";
				    response["error"] = error;
				    result = false;
                    returnResponse(result);
                }

                blocklistFlag = parameters[BLOCKLIST].Boolean();
                if((blocklistFlag == true) || (blocklistFlag == false) ) {
                    status = write_parameters(DEVICESTATE_FILE, BLOCKLIST, blocklistFlag, update, oldBlocklistFlag);
			        if ((status != true)) {
			    	    LOGERR("Blocklist flag update failed. status %d ", status);
				        error["message"] = "Blocklist flag update failed";
				        error["code"] = "-32604";
				        response["error"] = error;
				        result = false;
			        }
                    else {
                        LOGINFO("Blocklist flag stored successfully in persistent memory");
                        result = true;
                    }
                }
                else {
                    LOGWARN("Invalid value");
                    populateResponseWithError(SysSrv_MissingKeyValues, response);
                    error["message"] = "Invalid params";
				    error["code"] = "-32602";
				    response["error"] = error;
                    result = false;
                }

                LOGINFO("Update= %s", (update ? "true":"false"));
                if(update == true) {
                    /*Send ONBLOCKLISTCHANGED event notify*/
                    if (SystemServices::_instance) {
                        SystemServices::_instance->onBlocklistChanged(blocklistFlag, oldBlocklistFlag);
                    } else {
                        LOGERR("SystemServices::_instance is NULL.\n");
                    }
                }
            }
            else {
                populateResponseWithError(SysSrv_MissingKeyValues, response);
                error["message"] = "Invalid params";
				error["code"] = "-32602";
				response["error"] = error;
                result = false;
            }

            returnResponse(result);
        }

        uint32_t SystemServices::getBlocklistFlag(const JsonObject& parameters, JsonObject& response)
	    {
		
		    bool status = false, result = false, blocklistFlag, ret = false;
            JsonObject error;

            /*check /opt/secure/persistent/opflashstore/ dir*/
            ret = checkOpFlashStoreDir();
            if(ret == true){
                LOGINFO("checked opflashstore directory and it is exists. ret = %d",ret);
            }
            else {
			    LOGWARN("Blocklist flag retrieved failed from persistent memory.");
                error["message"] = "Blocklist flag retrieved failed from persistent memory.";
				error["code"] = "-32099";
				response["error"] = error;
                status = false;
                returnResponse(status);
            }

            result = read_parameters(DEVICESTATE_FILE, BLOCKLIST, blocklistFlag);
		    if (result == true) {
                LOGWARN("blocklistFlag=%d", blocklistFlag);
                response["blocklist"] = blocklistFlag;
			    status = true;
                LOGINFO("Blocklist flag retrieved successfully from persistent memory.");
		    }
		    else{
			    LOGWARN("Blocklist flag retrieved failed from persistent memory.");
                 error["message"] = "Blocklist flag retrieved failed from persistent memory.";
				error["code"] = "-32099";
				response["error"] = error;
                status = false;
		    }

            returnResponse(status);
	    }

        /***
         * @brief  : Event handler of Firmware Update Info status check.
         * @param1[in] : New FW Version
         * @param2[in] : HTTP Query Status Code
         * @param3[in] : Check status code
         * @param4[in] : Current FW Version
         * @param5[in] : HTTP Payload Response
         */
        void SystemServices::reportFirmwareUpdateInfoReceived(string
                firmwareUpdateVersion, int httpStatus, bool success,
                string firmwareVersion, string responseString)
        {
            JsonObject params;
            params["status"] = httpStatus;
            params["responseString"] = responseString.c_str();
            params["rebootImmediately"] = false;

            JsonObject xconfResponse;
            if(!responseString.empty() && xconfResponse.FromString(responseString))
            {
                params["rebootImmediately"] = xconfResponse["rebootImmediately"];
            }

            if(httpStatus == STATUS_CODE_NO_SWUPDATE_CONF)
            {
                // Empty /opt/swupdate.conf
                params["status"] = 0;
                params["updateAvailable"] = false;
                params["updateAvailableEnum"] = static_cast<int>(FWUpdateAvailableEnum::EMPTY_SW_UPDATE_CONF);
                params["success"] = true;
            }
            else if(httpStatus == 404)
            {
                // if XCONF server returns 404 there is no FW available to download
                params["updateAvailable"] = false;
                params["updateAvailableEnum"] = static_cast<int>(FWUpdateAvailableEnum::FW_MATCH_CURRENT_VER);
                params["success"] = true;
            }
            else
            {
                FWUpdateAvailableEnum updateAvailableEnum = FWUpdateAvailableEnum::NO_FW_VERSION;
                bool bUpdateAvailable = false;
                if (firmwareUpdateVersion.length() > 0) {
                    params["firmwareUpdateVersion"] = firmwareUpdateVersion.c_str();
                    if (firmwareUpdateVersion.compare(firmwareVersion)) {
                        updateAvailableEnum = FWUpdateAvailableEnum::FW_UPDATE_AVAILABLE;
                        bUpdateAvailable = true;
                    } else {
                        updateAvailableEnum = FWUpdateAvailableEnum::FW_MATCH_CURRENT_VER;
                    }
                } else {
                    params["firmwareUpdateVersion"] = "";
                    updateAvailableEnum = FWUpdateAvailableEnum::NO_FW_VERSION;
                }
                params["updateAvailable"] = bUpdateAvailable ;
                params["updateAvailableEnum"] = static_cast<int>(updateAvailableEnum);
                params["success"] = success;
            }

            string jsonLog;
            params.ToString(jsonLog);
            LOGWARN("result: %s\n", jsonLog.c_str());
            sendNotify(EVT_ONFIRMWAREUPDATEINFORECEIVED, params);
        }

        /***
         * @brief : Firmware Update Info Worker.
         */
        void SystemServices::firmwareUpdateInfoReceived(void)
        {
            string env = "";
            string firmwareVersion;
            if (_instance) {
                firmwareVersion = _instance->getStbVersionString();
            } else {
                LOGERR("_instance is NULL.\n");
            }

            LOGWARN("SystemService firmwareVersion %s\n", firmwareVersion.c_str());

            if (true == findCaseInsensitive(firmwareVersion, "DEV"))
                env = "DEV";
            else if (true == findCaseInsensitive(firmwareVersion, "VBN"))
                env = "VBN";
            else if (true == findCaseInsensitive(firmwareVersion, "PROD"))
                env = "PROD";
            else if (true == findCaseInsensitive(firmwareVersion, "CQA"))
                env = "CQA";

            std::string response;
            firmwareUpdate _fwUpdate;
            
            _fwUpdate.success = false;
            _fwUpdate.httpStatus = 0;

            bool bFileExists = false;
            string xconfOverride; 
            if(env != "PROD")
            {
                xconfOverride = getXconfOverrideUrl(bFileExists);
                if(bFileExists && xconfOverride.empty())
                {
                    // empty /opt/swupdate.conf. Don't initiate FW download
                    LOGWARN("Empty /opt/swupdate.conf. Skipping FW upgrade check with xconf");
                    if (_instance) {
                        // Coverity Fix: ID 22 - COPY_INSTEAD_OF_MOVE
                        _instance->reportFirmwareUpdateInfoReceived("",
                        STATUS_CODE_NO_SWUPDATE_CONF, true, "", std::move(response));
                    }
                    return;
                }
            }

            v_secure_system("/lib/rdk/xconfImageCheck.sh  >> /opt/logs/wpeframework.log");

            //get xconf http code
            string httpCodeStr ="";
            const char* httpCodeFile = "/tmp/xconf_httpcode_thunder.txt";
            bool httpCodeReadSuccess =  Utils::readFileContent(httpCodeFile, httpCodeStr);

            if(httpCodeReadSuccess)
            {
			    LOGINFO("xconf httpCodeStr '%s'\n", httpCodeStr.c_str());
                try
                {
                    _fwUpdate.httpStatus = std::stoi(httpCodeStr);
                }
                catch(const std::exception& e)
                {
                    LOGERR("exception in converting xconf http code %s", e.what());
                }
            }

            LOGINFO("xconf http code %d\n", _fwUpdate.httpStatus);

            const char* responseFile = "/tmp/xconf_response_thunder.txt";
            bool responseReadSuccess = Utils::readFileContent(responseFile, response);

            if(responseReadSuccess)
            {
                JsonObject httpResp;
                if(httpResp.FromString(response))
                {
                    if(httpResp.HasLabel("firmwareVersion"))
                    {
                        _fwUpdate.firmwareUpdateVersion = httpResp["firmwareVersion"].String();
                        LOGWARN("fwVersion: '%s'\n", _fwUpdate.firmwareUpdateVersion.c_str());
                        _fwUpdate.success = true;
                    }
                    else
                    {
                        LOGERR("Xconf response is not valid json and/or doesn't contain firmwareVersion. '%s'\n", response.c_str());
                        response = "";
                    }
                }
                else
                {
                    LOGERR("Error in parsing xconf json response");
                }
                 
            }
            else
            {
                LOGERR("Unable to open xconf response file");
            }
            

            if (_instance) {
                // Coverity Fix: IDs 23-24 - COPY_INSTEAD_OF_MOVE
                _instance->reportFirmwareUpdateInfoReceived(std::move(_fwUpdate.firmwareUpdateVersion),
                        _fwUpdate.httpStatus, _fwUpdate.success, std::move(firmwareVersion), std::move(response));
            } else {
                LOGERR("_instance is NULL.\n");
            }
        } //end of event onFirmwareInfoRecived

        /***
         * @brief  : To check Firmware Update Info
         * @param1[in] : {"params":{}}
         * @param2[out] : {"result":{"asyncResponse":true,"success":true}}
         */
        uint32_t SystemServices::getFirmwareUpdateInfo(const JsonObject& parameters,
                JsonObject& response)
        {
            string callGUID = parameters["GUID"].String();
            LOGINFO("GUID = %s\n", callGUID.c_str());
            try
            {
                if (m_getFirmwareInfoThread.get().joinable()) {
                    m_getFirmwareInfoThread.get().join();
                }
                m_getFirmwareInfoThread = Utils::ThreadRAII(std::thread(firmwareUpdateInfoReceived));
                response["asyncResponse"] = true;
                returnResponse(true);
            }
            catch(const std::system_error& e)
            {
                LOGERR("exception in getFirmwareUpdateInfo %s", e.what());
                response["asyncResponse"] = false;
                returnResponse(false);
            }
        } // get FirmwareUpdateInfo

        /***
         * @brief Sets the deep sleep time out period, specified in seconds by invoking the corresponding
         * systemService method. This function used as an interface function in Java script.
         * @param1[in]	: {"jsonrpc":"2.0","id":"3","method":"org.rdk.SystemServices.1.setDeepSleepTimer",
         *				"params":{"seconds":<unsigned int>}}
         * @param2[out]	: {"jsonrpc":"2.0","id":3,"result":{"success":<bool>}}
         * @return		: Core::<StatusCode>
         */
        uint32_t SystemServices::setDeepSleepTimer(const JsonObject& parameters,
                JsonObject& response)
=======
    namespace Plugin
>>>>>>> 4b94bc1 (RDKEMW-7075: COM-RPC support to SystemService plugin)
    {

    /*
     *Register SystemServices module as wpeframework plugin
     **/
    SERVICE_REGISTRATION(SystemServices, API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH);

    SystemServices::SystemServices() : _service(nullptr), _connectionId(0), _systemServices(nullptr), _systemServicesNotification(this)
    {
        SYSLOG(Logging::Startup, (_T("Systemservices Constructor")));
    }

    SystemServices::~SystemServices()
    {
        SYSLOG(Logging::Shutdown, (string(_T("SystemServices Destructor"))));
    }

    const string SystemServices::Initialize(PluginHost::IShell* service)
    {
        string message="";

        ASSERT(nullptr != service);
        ASSERT(nullptr == _service);
        ASSERT(nullptr == _systemServices);
        ASSERT(0 == _connectionId);

        SYSLOG(Logging::Startup, (_T("SystemServices::Initialize: PID=%u"), getpid()));

        _service = service;
        _service->AddRef();
        _service->Register(&_systemServicesNotification);
        _systemServices = _service->Root<Exchange::ISystemServices>(_connectionId, 5000, _T("SystemServicesImplementation"));

        if(nullptr != _systemServices)
        {
            _configure = _systemServices->QueryInterface<Exchange::IConfiguration>();
            if (_configure != nullptr)
            {
                uint32_t result = _configure->Configure(_service);
                if(result != Core::ERROR_NONE)
                {
                    message = _T("SystemServices could not be configured");
                }
            }
            else
            {
                message = _T("SystemServices implementation did not provide a configuration interface");
            }
            // Register for notifications
            _systemServices->Register(&_systemServicesNotification);
            // Invoking Plugin API register to wpeframework
            Exchange::JSystemServices::Register(*this, _systemServices);
        }
        else
        {
            SYSLOG(Logging::Startup, (_T("SystemServices::Initialize: Failed to initialise SystemServices plugin")));
            message = _T("SystemServices plugin could not be initialised");
        }

        return message;
    }

    void SystemServices::Deinitialize(PluginHost::IShell* service)
    {
        ASSERT(_service == service);

        SYSLOG(Logging::Shutdown, (string(_T("SystemServices::Deinitialize"))));

        // Make sure the Activated and Deactivated are no longer called before we start cleaning up..
        _service->Unregister(&_systemServicesNotification);

        if (nullptr != _systemServices)
        {
            _systemServices->Unregister(&_systemServicesNotification);
            Exchange::JSystemServices::Unregister(*this);

            if (_configure != nullptr) {
                _configure->Release();
                _configure = nullptr;
            }

            // Stop processing:
            RPC::IRemoteConnection* connection = service->RemoteConnection(_connectionId);
            VARIABLE_IS_NOT_USED uint32_t result = _systemServices->Release();

            _systemServices = nullptr;

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
        SYSLOG(Logging::Shutdown, (string(_T("SystemServices de-initialised"))));
    }

    string SystemServices::Information() const
    {
       return "The SystemServices plugin is used to manage various system-level features.";
    }

    void SystemServices::Deactivated(RPC::IRemoteConnection* connection)
    {
        if (connection->Id() == _connectionId) {
            ASSERT(nullptr != _service);
            Core::IWorkerPool::Instance().Submit(PluginHost::IShell::Job::Create(_service, PluginHost::IShell::DEACTIVATED, PluginHost::IShell::FAILURE));
        }
    }
} // namespace Plugin
} // namespace WPEFramework
