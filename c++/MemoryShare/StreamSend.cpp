#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using std::cout;
using std::endl;
//using namespace cv;

constexpr int BUFFER_NUM = 3;
constexpr int IMAGE_SIZE = 1920 * 1080 * 3;
constexpr wchar_t const* FILE_NAME = L"Global\\SharedMemoryDemo";
constexpr int BUF_SIZE = IMAGE_SIZE * BUFFER_NUM + 2;


bool getImage(int index, char* & imageOut)
{
    static char* image = new char[IMAGE_SIZE];

    std::string fileName = "C:\\Users\\Administrator\\Desktop\\ZXTools\\CppProject\\VideoStream\\ImageTest\\" + std::to_string(index) + ".temp";
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

void loopSend(void* pBuf)
{
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

    // 初始化写入位置
    if (WaitForSingleObject(writeMutex, INFINITE) != WAIT_OBJECT_0) {
        // 错误处理
    }
    *writeIndexBuf = writeIndex;
    ReleaseMutex(writeMutex);

    // 初始化读取位置
    if (WaitForSingleObject(readMutex, INFINITE) != WAIT_OBJECT_0) {
        // 错误处理
    }
    *readIndexBuf = readIndex;
    ReleaseMutex(readMutex);

    int index = 0;

    while(1)
    {
        //正在读的帧，不能等于即将写的帧
        do {
            // 查看读取位置
            if (WaitForSingleObject(readMutex, INFINITE) != WAIT_OBJECT_0) {
                // 错误处理
            }
            readIndex = *readIndexBuf;
            ReleaseMutex(readMutex);
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
        if (WaitForSingleObject(writeMutex, INFINITE) != WAIT_OBJECT_0) {
            // 错误处理
        }
        *writeIndexBuf = writeIndex;
        ReleaseMutex(writeMutex);
    }
}

void createShareMem()
{
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
    //memset(pBuf, 0, BUF_SIZE);

    //循环发送数据
    loopSend(pBuf);
    
    // 解除映射视图
    UnmapViewOfFile(pBuf);

    // 关闭文件映射对象
    CloseHandle(hMapFile);
}


int main(int argc, char** argv)
{
    createShareMem();

    return 0;
}