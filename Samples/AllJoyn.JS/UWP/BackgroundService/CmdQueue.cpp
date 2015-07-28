#include <Windows.h>
#include <vector>
#include <set>
#include <map>

#include "CmdQueue.h"

using namespace std;

struct Cmd
{
    void* buf;
    size_t size;

    Cmd(void* b, size_t s) : buf(NULL), size(0)
    {
        if (b && s > 0)
        {
            buf = (void*)new uint8_t[s];
            if (buf)
            {
                memcpy(buf, b, s);
                size = s;
            }
        }
    }

    ~Cmd()
    {
        delete[] buf;
    }
};

class CmdQueue
{
public:
    CmdQueue();
    ~CmdQueue();

    bool Write(HANDLE resQueue, void* buf, size_t sizeToWrite);
    bool Read(HANDLE resQueue, void* buf, size_t sizeToRead);
    bool GetResult(HANDLE resQueue, size_t* sizeRead);
    void Clear();
    void Lock();
    void Unlock();

private:
    vector<Cmd*> cmdArray;              // Command array
    map<HANDLE, DWORD> opResults;       // Operation results (Read and Write results are stored separately)
    HANDLE writeEvent;                  // Set after each write operation
    HANDLE abortEvent;                  // Set to abort read wait
    CRITICAL_SECTION cs;
};


// Command Queue set - use to identify if the passed in HANDLE is a CmdQueue or regular WIN32 HANDLE
static set<CmdQueue*> CmdQueueSet;



CmdQueue::CmdQueue()
{
    writeEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
    abortEvent = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
    InitializeCriticalSectionEx(&cs, 0, 0);
}

CmdQueue::~CmdQueue()
{
    SetEvent(abortEvent);

    Clear();
    CloseHandle(writeEvent);
    CloseHandle(abortEvent);
    DeleteCriticalSection(&cs);
}

void CmdQueue::Lock()
{
    EnterCriticalSection(&cs);
}

void CmdQueue::Unlock()
{
    LeaveCriticalSection(&cs);
}

bool CmdQueue::Write(HANDLE resQueue, void* buf, size_t sizeToWrite)
{
    opResults[resQueue] = 0;

    if (buf && sizeToWrite > 0)
    {
        Cmd* cmd = NULL;
        try
        {
            cmd = new Cmd(buf, sizeToWrite);
        }
        catch (const std::bad_alloc&)
        {
            return false;
        }

        Lock();
        try
        {
            cmdArray.push_back(cmd);
        }
        catch (const std::bad_alloc&)
        {
            delete cmd;
        }
        Unlock();

        opResults[resQueue] = sizeToWrite;

        SetEvent(writeEvent);
    }
    return true;
}

/// Blocking call
bool CmdQueue::Read(HANDLE resQueue, void* buf, size_t sizeToRead)
{
    opResults[resQueue] = 0;

    if (buf && sizeToRead)
    {
        DWORD wait;
        if (cmdArray.empty())
        {
            HANDLE handles[] = { writeEvent, abortEvent };
            wait = WaitForMultipleObjectsEx(2, handles, FALSE, INFINITE, FALSE);
            if (WAIT_OBJECT_0 != wait)
            {
                return false;
            }
        }

        Lock();
        if (!cmdArray.empty())
        {
            Cmd* cmd = cmdArray.front();
            if (cmd)
            {
                if (cmd->buf)
                {
                    size_t size = (cmd->size > sizeToRead) ? sizeToRead : cmd->size;
                    if (size > 0)
                    {
                        memcpy(buf, cmd->buf, size);
                    }
                    opResults[resQueue] = size;
                }
                delete cmd;
            }
            cmdArray.erase(cmdArray.begin());
        }
        Unlock();
    }
    return true;
}

bool CmdQueue::GetResult(HANDLE resQueue, size_t* resultSize)
{
    if (resultSize)
    {
        *resultSize = opResults[resQueue];
    }
    return true;
}

void CmdQueue::Clear()
{
    Lock();
    for (auto iter = cmdArray.begin(); iter != cmdArray.end(); iter++)
    {
        Cmd* cmd = (Cmd*)*iter;
        delete cmd;
    }

    cmdArray.clear();
    Unlock();
}

HANDLE CreateCmdQueue()
{
    CmdQueue *p = new (std::nothrow) CmdQueue();
    if (p)
    {
        CmdQueueSet.insert(p);
        return (HANDLE)p;
    }

   return INVALID_HANDLE_VALUE;
}

void CloseCmdQueue(HANDLE cmdQueue)
{
    if (cmdQueue == NULL || cmdQueue == INVALID_HANDLE_VALUE)
    {
        return;
    }

    auto iter = CmdQueueSet.find((CmdQueue*)cmdQueue);
    if (iter != CmdQueueSet.end())
    {
        CmdQueueSet.erase(iter);
        delete (CmdQueue*)cmdQueue;
    }
    else
    {
        CloseHandle(cmdQueue);
    }
}

bool ReadFromCmdQueue(HANDLE cmdQueue, void* buf, size_t sizeToRead, HANDLE resQueue)
{
    auto iter = CmdQueueSet.find((CmdQueue*)cmdQueue);
    if (iter != CmdQueueSet.end())
    {
        return ((CmdQueue*)cmdQueue)->Read(resQueue, buf, sizeToRead);
    }
    return false;
}

bool WriteToCmdQueue(HANDLE cmdQueue, void* buf, size_t sizeToWrite, HANDLE resQueue)
{
    auto iter = CmdQueueSet.find((CmdQueue*)cmdQueue);
    if (iter != CmdQueueSet.end())
    {
        return ((CmdQueue*)cmdQueue)->Write(resQueue, buf, sizeToWrite);
    }
    return false;
}

bool GetCmdQueueResult(HANDLE cmdQueue, DWORD* result, HANDLE resQueue)
{
    auto iter = CmdQueueSet.find((CmdQueue*)cmdQueue);
    if (iter != CmdQueueSet.end())
    {
        return ((CmdQueue*)cmdQueue)->GetResult(resQueue, (size_t*)result);
    }
    return false;
}

