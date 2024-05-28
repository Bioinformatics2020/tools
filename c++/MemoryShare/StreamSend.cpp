#include <iostream>
#include <fstream>
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


using std::cout;
using std::endl;
//using namespace cv;

constexpr int BUFFER_NUM = 3;
constexpr int IMAGE_SIZE = 1920 * 1080 * 3;
#ifdef WINDOWS
constexpr wchar_t const* FILE_NAME = L"Global/SharedMemoryDemo";
#else
const char* FILE_NAME = "/SharedMemoryDemo";
#endif
constexpr int BUF_SIZE = IMAGE_SIZE * BUFFER_NUM + 2;


bool getImage(int index, char* & imageOut)
{
    static char* image = new char[IMAGE_SIZE];

    std::string fileName = "./ImageTest/" + std::to_string(index) + ".temp";
    std::ifstream imageFile(fileName, std::ios::in | std::ios::binary);
    if (!imageFile.is_open())
    {
        std::cerr << "open fileName error:" << fileName << std::endl;
        return false;
    }
        
    if (!imageFile.read(image, IMAGE_SIZE))
    {
        std::cerr << "read file error:" << fileName << std::endl;
        return false;
    }
    imageOut = image;
    return true;
}

#ifndef WINDOWS
void loopSend(void* pBuf) __attribute__((optimize("O0")));
#endif
void loopSend(void* pBuf)
{
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
    // 初始化写入位置
    *writeIndexBuf = writeIndex;
    // 初始化读取位置
    *readIndexBuf = readIndex;

    int index = 0;

    while(1)
    {
        //正在读的帧，不能等于即将写的帧
        do {
            // 查看读取位置
#ifdef WINDOWS
            WaitForSingleObject(readMutex, INFINITE);
#endif
            readIndex = *readIndexBuf;
#ifdef WINDOWS
            ReleaseMutex(readMutex);
#endif
        } while (writeIndex == readIndex);
        
        
        {
            char* image = nullptr;
            if (!getImage(index, image))
            {
                std::cerr << "getImage error" << index << std::endl;
                return;
            }
            
            // 写入数据到共享内存
            memcpy(imageBuf + IMAGE_SIZE * writeIndex, image, IMAGE_SIZE); // 包含末尾的空字符
            
            unsigned long long imageHash = 0;
            for (unsigned int i = 0; i < IMAGE_SIZE; i++)
            {
                imageHash += image[i];
            }
            std::cout << index << "hash:" << imageHash << std::endl;
            index++;
        }
        

        // 当前帧写入完毕
        writeIndex = (writeIndex + 1) % BUFFER_NUM;
#ifdef WINDOWS
        WaitForSingleObject(writeMutex, INFINITE);
#endif
        *writeIndexBuf = writeIndex;
#ifdef WINDOWS
        ReleaseMutex(writeMutex);
#endif
    }
}


void createShareMem()
{
#ifdef WINDOWS
    // 创建或打开一个文件映射对象
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // 物理文件句柄
        NULL, // 默认安全级别
        PAGE_READWRITE,          // 允许读写
        0,
        BUF_SIZE,
        FILE_NAME);               // 文件映射名，作为共享标识

    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object (" << GetLastError() << ").\n";
        return;
    }

    // 映射视图到当前进程地址空间
    void* pBuf = MapViewOfFile(hMapFile,   // 文件映射句柄
                                 FILE_MAP_READ | FILE_MAP_WRITE, // 以写入权限映射
                                 0,
                                 0,
                                 BUF_SIZE);
                                 

    if (pBuf == NULL) {
        std::cerr << "Could not map view of file (" << GetLastError() << ").\n";
        CloseHandle(hMapFile);
        return;
    }
#else
    //创建和打开共享内存
    int shm_fd = shm_open(FILE_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::perror("Could not create file mapping object!");
    }
    //设置共享内存大小
    ftruncate(shm_fd, BUF_SIZE);
    //映射共享内存到进程地址空间
    void *pBuf = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
#endif

    //循环发送数据
    loopSend(pBuf);

#ifdef WINDOWS
    // 解除映射视图
    UnmapViewOfFile(pBuf);
    // 关闭文件映射对象
    CloseHandle(hMapFile);
#else
    //关闭共享内存描述符
    close(shm_fd);
#endif
}


int main(int argc, char** argv)
{
    createShareMem();

    return 0;
}