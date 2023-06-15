#include "Utils.h"
#include <windows.h>

void GetProcessorCoreCounts(unsigned short* physicalCoreCount, unsigned short* logicalCoreCount) {
	DWORD len = 0;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION processorInfo = NULL;

	// Set
	GetLogicalProcessorInformation(NULL, &len);
	processorInfo = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(len);
	const int infoSize = len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);

	// Get
	GetLogicalProcessorInformation(processorInfo, &len);

	*physicalCoreCount = 0;
	*logicalCoreCount = 0;
	for (DWORD i = 0; i < infoSize; i++) {
		if (processorInfo[i].Relationship == RelationProcessorCore) {
			++(*physicalCoreCount);

			DWORD_PTR mask = processorInfo[i].ProcessorMask;
			while (mask) {
				*logicalCoreCount += mask & 1;
				mask >>= 1;
			}
		}
	}
	free(processorInfo);
}