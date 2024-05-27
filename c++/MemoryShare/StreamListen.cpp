#include <windows.h>
#include <iostream>
#include <string>

constexpr int BUFFER_NUM = 3;
constexpr int IMAGE_SIZE = 1920 * 1080 * 3;
constexpr wchar_t const* FILE_NAME = L"Global\\SharedMemoryDemo";
constexpr int BUF_SIZE = IMAGE_SIZE * BUFFER_NUM + 2;

int main() {
    // 打开已存在的文件映射对象
    HANDLE hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,  
        true,
        FILE_NAME);      // 使用相同的文件映射名

    if (hMapFile == NULL) {
        std::cerr << "Could not open file mapping object (" << GetLastError() << ").\n";
        return 1;
    }

    // 映射视图到当前进程地址空间
    void* pBuf = (LPTSTR)MapViewOfFile(hMapFile,
        FILE_MAP_READ | FILE_MAP_WRITE,
                                 0,
                                 0,
                                 BUF_SIZE);

    if (pBuf == NULL) {
        std::cerr << "Could not map view of file (" << GetLastError() << ").\n";
        CloseHandle(hMapFile);
        return 1;
    }

    //写入位置锁
    HANDLE writeMutex = CreateMutex(NULL, FALSE, TEXT("Global\\writeMutex")); // 注意前缀"Global\"使得互斥锁跨进程可见
    if (writeMutex == NULL) {
        // 错误处理
    }
    char writeIndex = 1;

    //读取位置锁
    HANDLE readMutex = CreateMutex(NULL, FALSE, TEXT("Global\\readMutex")); // 注意前缀"Global\"使得互斥锁跨进程可见
    if (readMutex == NULL) {
        // 错误处理
    }
    char readIndex = 0;
    
    char* writeIndexBuf = static_cast<char*>(pBuf);
    char* readIndexBuf = static_cast<char*>(pBuf)+1;
    char* imageBuf = static_cast<char*>(pBuf) + 2;
    
    int index = 0;
    while(1)
    {
        //即将读的帧，不能等于正在写的帧
        {
            // 查看读取位置
            if (WaitForSingleObject(writeMutex, INFINITE) != WAIT_OBJECT_0) {
                // 错误处理
            }
            writeIndex = *writeIndexBuf;
            ReleaseMutex(writeMutex);
        }
        if(writeIndex == (readIndex + 1) % BUFFER_NUM)
        {
            continue;
        }
        else
        {
            // 设置正在读取的帧
            readIndex = (readIndex + 1) % BUFFER_NUM;
            if (WaitForSingleObject(readMutex, INFINITE) != WAIT_OBJECT_0) {
                // 错误处理
            }
            *readIndexBuf = readIndex;
            ReleaseMutex(readMutex);
        }
        
        {
            // 读取共享内存中的数据
            char* image = imageBuf + IMAGE_SIZE * readIndex;
            unsigned long long imageHash = 0;
            for (int i = 0; i < IMAGE_SIZE; i++)
            {
                imageHash += image[i];
            }
            std::cout << index << "hash:" << imageHash << std::endl;
            index++;

            Sleep(100);
        }
        
    }

    // 解除映射视图
    UnmapViewOfFile(pBuf);

    // 关闭文件映射对象
    CloseHandle(hMapFile);

    return 0;
}