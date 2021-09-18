#include "Readfile.h"

uint64_t reverseByte(uint8_t* byte, unsigned int count)
{
	uint64_t result = 0;
	int i;
	for (i = count - 1; i >= 0; i--)
		result = (result << 8) | byte[i];

	return result;
}



string getStr(mainEntry entry)
{
	string name;
	for (int i = 0; i < 8; i += 1)
		if (int(entry.name) != 32)
			name += entry.name[i];
	for (int i = 0; i < 3; i += 1)
		name += entry.fileType[i];
	return name;
}



string getName(extraEntry entry, int n, int type)
{
	string name;
	for (int i = 0; i < n; i += 2)
	{
		if (type == 1)
			name += entry.fileName_Part1[i];
		if (type == 2)
			name += entry.fileName_Part2[i];
		if (type == 3)
			name += entry.fileName_Part3[i];
	}
	return name;
}



int ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512], long posSector)
{
	int retCode = 0;
	DWORD bytesRead;
	HANDLE device = NULL;

	device = CreateFile(drive,    // Drive to open
		GENERIC_READ,           // Access mode
		FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
		NULL,                   // Security Descriptor
		OPEN_EXISTING,          // How to create
		0,                      // File attributes
		NULL);                  // Handle to template

	if (device == INVALID_HANDLE_VALUE) // Open Error
	{
		printf("CreateFile: %u\n", GetLastError());
		return 1;
	}

	SetFilePointer(device, readPoint + 512 * posSector, NULL, FILE_BEGIN);//Set a Point to Read

	if (!ReadFile(device, sector, 512, &bytesRead, NULL))
	{
		printf("ReadFile: %u\n", GetLastError());
	}
}



void PrintFloppyInformationNTFS(NTFS _ntfs)
{
	printf("Floppy Disk Information: \n");

	printf("===========================\n");
	printf("FAT: %s\n", _ntfs.OEMID); //Loai FAT
	printf("Bytes per sector: %d\n", reverseByte(_ntfs.Bytes_Sector, 2)); // So Byte cho 1 sector
	printf("Sector per cluster: %d\n", _ntfs.Sectors_Cluster); // So Sector cho 1 cluster
	printf("Reserved Sector: %d\n", reverseByte(_ntfs.Reserved_Sector, 2)); // So Sector vung boot Sector
	printf("Total Sectors : %lld\n", _ntfs.total_sectors); //Tong so sector, kich thuoc Volume
	printf("Logical Cluster Number for the file $MFT : %lld\n", _ntfs.Logical_MFT); // so cum cluster cho MFT
	printf("Logical Cluster Number for the file $MFTMirr : %lld\n", _ntfs.Logical_MFTMirr); // so cum cluster cho MFTMIRR
}



int PrintFloppyInformationFAT32(FAT32 fat32)
{
	printf("FAT: %s\n", fat32.Fat_name); //Loai FAT
	printf("Bytes per sector: %d\n", reverseByte(fat32.bytePerSector, 2));// So Byte cho 1 sector
	printf("Sector per cluster: %d\n", fat32.sectorPerCluster);// So Sector cho 1 cluster
	printf("Reserved sectors (Sb): %d\n", reverseByte(fat32.reservedSector, 2));// So Sector vung boot Sector
	printf("FAT copies: %d\n", fat32.fatCopy); // So bang FAT
	printf("Total sector: %d\n", reverseByte(fat32.Total_sector, 4));//Tong so sector, kich thuoc Volume
	printf("Sector per FAT: %d\n", reverseByte(fat32.SectorperFAT, 4));// so sector cho 1 bang fat
	printf("Root directory entries: %d\n", reverseByte(fat32.rdetEntry, 2)); // So Entry cho bang RDET

	printf("first sector of FAT1: %d\n", reverseByte(fat32.reservedSector, 2)); // sector dau tien cua bang FAT1
	int s = int(reverseByte(fat32.reservedSector, 2)) + int(reverseByte(fat32.SectorperFAT, 4)) * int(fat32.fatCopy);
	printf("first sector of RDET: %d\n", s); // sector dau tien cua vung RDET
	int d = s + int(reverseByte(fat32.rdetEntry, 2) * 32 / 512);
	printf("first sector of DATA: %d\n", s); // sector dau tien cua vung DATA
	return s;
}



void prinfInforOfmMainEntry(mainEntry entry)
{
	cout << "file name: " << entry.fullname << endl;
	if (entry.attrib & 0x01)
		printf("File Attribute       : Read Only File\n");
	if (entry.attrib & 0x02)
		printf("File Attribute    : Hidden File\n");
	if (entry.attrib & 0x04)
		printf("File Attribute    : System File\n");
	if (entry.attrib & 0x08)
		printf("File Attribute    : Volume Label\n");
	if (entry.attrib & 0x0f)
		printf("File Attribute    : Long File Name\n");
	if (entry.attrib & 0x10)
		printf("File Attribute    : Directory\n");
	if (entry.attrib & 0x20)
		printf("File Attribute    : Archive\n");

	WORD nYear = (entry.createdate >> 9);
	WORD nMonth = (entry.createdate << 7);
	nMonth = nMonth >> 12;
	WORD nDay = (entry.createdate << 11);
	nDay = nDay >> 11;

	printf("Create Date    : %d/%d/%d\n", nDay, nMonth, (nYear + 1980));

	nYear = (entry.modifieddate >> 9);
	nMonth = (entry.modifieddate << 7);
	nMonth = nMonth >> 12;
	nDay = (entry.modifieddate << 11);
	nDay = nDay >> 11;

	printf("Modification Date    : %d/%d/%d\n", nDay, nMonth, (nYear + 1980));

	nYear = (entry.accessdate >> 9);
	nMonth = (entry.accessdate << 7);
	nMonth = nMonth >> 12;
	nDay = (entry.accessdate << 11);
	nDay = nDay >> 11;

	printf("Accessed Date        : %d/%d/%d\n", nDay, nMonth, (nYear + 1980));

	printf("Start Cluster Address: %d\n",
		entry.clusterlow);
	printf("File Size            : %d bytes\n", reverseByte(entry.filesize, 4));
}



string prinfInforOfExtraEntry(extraEntry entry)
{
	//printf("File Attribute    : Long File Name\n");
	//printf("sequenceNo: %d\n", entry.sequenceNo);

	string full;
	string part;

	part = getName(entry, 10, 1);
	//cout << "fileName_Part1: " << part << endl;
	full += part;

	part = getName(entry, 12, 2);
	//cout << "fileName_Part2: " << part << endl;
	full += part;

	part = getName(entry, 4, 3);
	//cout << "fileName_Part3: " << part << endl;
	full += part;

	//cout << "FULL: " << full << endl;
	//cout << endl << endl;
	return full;
}

void prinfInforOfEntry(inforEntry entry[100], int n)
{
	for (int i = 0; i < n; i++)
	{
		cout << "/////////   INFOR OF ENTRY   /////////" << endl;
		cout << "name: " << entry[i].name << endl;
		cout << "file status: " << entry[i].fileStatus << endl;
		cout << "file Size: " << entry[i].fileSize << endl;
		cout << "sector Index: " << entry[i].sectorIndex << endl;

		system("pause");
		system("cls");
	}
}

void printfTXT(string a)
{
	cout << endl << "INFOR: " << endl;
	ifstream ifs;
	ifs.open(a);
	if (ifs.fail()) {
		cout << "cant open file" << endl;
		ifs.close();
		return;
	}

	string docfile = "", dong = "";
	while (!ifs.eof()) {
		getline(ifs, dong);
		docfile += dong + "\n";
	}
	cout << docfile << endl;

	ifs.close();
}



void printf_TXT_file(mainEntry entry[100], int n)
{
	string a;
	for (int i = 0; i < n; i++)
	{
		cout << "//////   INFOR OF TXT FILE    //////" << endl;
		a = "";
		for (int j = 0; j < 3; j++)
			a += entry[i].fileType[j];
		if (a == "TXT")
		{
			prinfInforOfmMainEntry(entry[i]);
			string a = "H:/" + entry[i].fullname;
			printfTXT(a);
		}
		else
		{
			if (entry[i].name[0] == 0xE5)
				cout << "GOT DELETED" << endl;
			else
				cout << "file name: " << entry[i].fullname << endl;
			cout << "use another program to read that file" << endl;
		}
		system("pause");
		system("cls");
	}
}