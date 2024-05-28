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
    //д��λ����
    HANDLE writeMutex = CreateMutex(NULL, FALSE, TEXT("writeMutex"));
    //��ȡλ����
    HANDLE readMutex = CreateMutex(NULL, FALSE, TEXT("readMutex"));
#endif

    //������λ������
    char* writeIndexBuf = static_cast<char*>(pBuf);
    char* readIndexBuf = static_cast<char*>(pBuf)+1;
    char* imageBuf = static_cast<char*>(pBuf) + 2;

    //��дλ������
    char writeIndex = 1;
    char readIndex = 0;
    // ��ʼ��д��λ��
    *writeIndexBuf = writeIndex;
    // ��ʼ����ȡλ��
    *readIndexBuf = readIndex;

    int index = 0;

    while(1)
    {
        //���ڶ���֡�����ܵ��ڼ���д��֡
        do {
            // �鿴��ȡλ��
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
            
            // д�����ݵ������ڴ�
            memcpy(imageBuf + IMAGE_SIZE * writeIndex, image, IMAGE_SIZE); // ����ĩβ�Ŀ��ַ�
            
            unsigned long long imageHash = 0;
            for (unsigned int i = 0; i < IMAGE_SIZE; i++)
            {
                imageHash += image[i];
            }
            std::cout << index << "hash:" << imageHash << std::endl;
            index++;
        }
        

        // ��ǰ֡д�����
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
    // �������һ���ļ�ӳ�����
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // �����ļ����
        NULL, // Ĭ�ϰ�ȫ����
        PAGE_READWRITE,          // �����д
        0,
        BUF_SIZE,
        FILE_NAME);               // �ļ�ӳ��������Ϊ�����ʶ

    if (hMapFile == NULL) {
        std::cerr << "Could not create file mapping object (" << GetLastError() << ").\n";
        return;
    }

    // ӳ����ͼ����ǰ���̵�ַ�ռ�
    void* pBuf = MapViewOfFile(hMapFile,   // �ļ�ӳ����
                                 FILE_MAP_READ | FILE_MAP_WRITE, // ��д��Ȩ��ӳ��
                                 0,
                                 0,
                                 BUF_SIZE);
                                 

    if (pBuf == NULL) {
        std::cerr << "Could not map view of file (" << GetLastError() << ").\n";
        CloseHandle(hMapFile);
        return;
    }
#else
    //�����ʹ򿪹����ڴ�
    int shm_fd = shm_open(FILE_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::perror("Could not create file mapping object!");
    }
    //���ù����ڴ��С
    ftruncate(shm_fd, BUF_SIZE);
    //ӳ�乲���ڴ浽���̵�ַ�ռ�
    void *pBuf = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
#endif

    //ѭ����������
    loopSend(pBuf);

#ifdef WINDOWS
    // ���ӳ����ͼ
    UnmapViewOfFile(pBuf);
    // �ر��ļ�ӳ�����
    CloseHandle(hMapFile);
#else
    //�رչ����ڴ�������
    close(shm_fd);
#endif
}


int main(int argc, char** argv)
{
    createShareMem();

    return 0;
}