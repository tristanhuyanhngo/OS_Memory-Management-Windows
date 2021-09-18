#include "Readfile.h"

int main()
{

	BYTE sector[512];
	NTFS ntfs;
	FAT32 fat32;

	ReadSector(L"\\\\.\\F:", 0, sector, 0);
	memcpy(&ntfs, sector, 512);

	long size = 0;

	cout << "/////////    INFORMATION OF BOOT SECTOR     ////////" << endl;

	if (ntfs.OEMID[0] == 'N')
		PrintFloppyInformationNTFS(ntfs);
	else
	{
		memcpy(&fat32, sector, 512);
		size = PrintFloppyInformationFAT32(fat32);
	}
	system("pause");
	system("cls");



	BYTE entry[512];

	BYTE rootEntry[32];
	mainEntry mEntry[100];
	int posEntry = 0;
	extraEntry exEntry;

	inforEntry inforEntry[100];
	int pos = 0;

	while (true)
	{
		ReadSector(L"\\\\.\\F:", 0, entry, size);
		BYTE* pEntry = entry;

		for (int i = 0; i < 512 / 32; i++)
		{
			memcpy(&rootEntry, pEntry, 32);

			if (rootEntry[0] == 0x00)
			{
				prinfInforOfEntry(inforEntry, pos);
				printf_TXT_file(mEntry, posEntry);
				return 0;
			}

			if (rootEntry[0] == 0xE5)
			{
				inforEntry[pos].fileStatus = "Deleted";
				inforEntry[pos].fileSize = 0;
				inforEntry[pos].sectorIndex = 0;
				inforEntry[pos].name = "";
				pEntry += 32;
				pos++;
				continue;
			}
			if (rootEntry[11] == 0x0F)
			{
				memcpy(&exEntry, pEntry, 32);
				string part = prinfInforOfExtraEntry(exEntry);
				inforEntry[pos].name = part + inforEntry[pos].name;
			}
			else
			{
				memcpy(&mEntry[posEntry], pEntry, 32);
				inforEntry[pos].fileStatus = "Exist";
				inforEntry[pos].fileSize = reverseByte(mEntry[posEntry].filesize, 4);
				inforEntry[pos].sectorIndex = size;
				if (inforEntry[pos].name == "")
				{
					inforEntry[pos].name = getStr(mEntry[posEntry]);
					mEntry[pos].fullname = inforEntry[pos].name;
				}
				else
					mEntry[posEntry].fullname = inforEntry[pos].name;
				pos++;
				posEntry++;
			}
			pEntry += 32;
		}
		size += 1;
	}
	return 0;
}