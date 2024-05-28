#include <iostream>
#include <string>
#include <cstring>

//#define WINDOWS

#ifdef WINDOWS
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

constexpr int BUFFER_NUM = 3;
constexpr int IMAGE_SIZE = 1920 * 1080 * 3;
#ifdef WINDOWS
constexpr wchar_t const* FILE_NAME = L"Global/SharedMemoryDemo";
#else
const char* FILE_NAME = "/SharedMemoryDemo";
#endif
constexpr int BUF_SIZE = IMAGE_SIZE * BUFFER_NUM + 2;

#ifndef WINDOWS
int main() __attribute__((optimize("O0")));
#endif

int main() {
#ifdef WINDOWS
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
#else
    //创建和打开共享内存
    int shm_fd = shm_open(FILE_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Could not create file mapping object";
        std::strerror(errno);
    }
    //设置共享内存大小
    ftruncate(shm_fd, BUF_SIZE);
    //映射共享内存到进程地址空间
    void* pBuf = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
#endif

#ifdef WINDOWS
    //写入位置锁
    HANDLE writeMutex = CreateMutex(NULL, FALSE, TEXT("writeMutex"));
    //读取位置锁
    HANDLE readMutex = CreateMutex(NULL, FALSE, TEXT("readMutex"));
#endif

    //缓冲区位置索引
    char* writeIndexBuf = static_cast<char*>(pBuf);
    char* readIndexBuf = static_cast<char*>(pBuf)+1;
    char* imageBuf = static_cast<char*>(pBuf) + 2;

    //读写位置索引
    char writeIndex = 1;
    char readIndex = 0;

    int index = 0;
    while(1)
    {
        //即将读的帧，不能等于正在写的帧
        {
            // 查看读取位置
#ifdef WINDOWS
            WaitForSingleObject(writeMutex, INFINITE);
#endif
            writeIndex = *writeIndexBuf;
#ifdef WINDOWS
            ReleaseMutex(writeMutex);
#endif
        }
        if(writeIndex == (readIndex + 1) % BUFFER_NUM)
        {
            continue;
        }
        else
        {
            // 设置正在读取的帧
            readIndex = (readIndex + 1) % BUFFER_NUM;
#ifdef WINDOWS
            WaitForSingleObject(readMutex, INFINITE);
#endif
            *readIndexBuf = readIndex;
#ifdef WINDOWS
            ReleaseMutex(readMutex);
#endif
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

#ifdef WINDOWS
            //延迟100毫秒
            Sleep(100);
#else
            //延迟1秒
            sleep(1);
#endif
        }
        
    }

#ifdef WINDOWS
    // 解除映射视图
    UnmapViewOfFile(pBuf);
    // 关闭文件映射对象
    CloseHandle(hMapFile);
#else
    //关闭共享内存描述符
    close(shm_fd);
#endif

    return 0;
}
