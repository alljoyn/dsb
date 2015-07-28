#pragma once

HANDLE CreateCmdQueue();
void CloseCmdQueue(HANDLE cmdQueue);
bool ReadFromCmdQueue(HANDLE cmdQueue, void* buf, size_t sizeToRead, HANDLE resQueue);
bool WriteToCmdQueue(HANDLE cmdQueue, void* buf, size_t sizeToWrite, HANDLE resQueue);
bool GetCmdQueueResult(HANDLE cmdQueue, DWORD* result, HANDLE resQueue);

