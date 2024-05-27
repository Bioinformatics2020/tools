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
    //д��λ����
    HANDLE writeMutex = CreateMutex(NULL, FALSE, TEXT("Global\\writeMutex")); // ע��ǰ׺"Global\"ʹ�û���������̿ɼ�
    if (writeMutex == NULL) {
        // ������
    }
    char writeIndex = 1;

    //��ȡλ����
    HANDLE readMutex = CreateMutex(NULL, FALSE, TEXT("Global\\readMutex")); // ע��ǰ׺"Global\"ʹ�û���������̿ɼ�
    if (readMutex == NULL) {
        // ������
    }
    char readIndex = 0;
    
    char* writeIndexBuf = static_cast<char*>(pBuf);
    char* readIndexBuf = static_cast<char*>(pBuf)+1;
    char* imageBuf = static_cast<char*>(pBuf) + 2;

    // ��ʼ��д��λ��
    if (WaitForSingleObject(writeMutex, INFINITE) != WAIT_OBJECT_0) {
        // ������
    }
    *writeIndexBuf = writeIndex;
    ReleaseMutex(writeMutex);

    // ��ʼ����ȡλ��
    if (WaitForSingleObject(readMutex, INFINITE) != WAIT_OBJECT_0) {
        // ������
    }
    *readIndexBuf = readIndex;
    ReleaseMutex(readMutex);

    int index = 0;

    while(1)
    {
        //���ڶ���֡�����ܵ��ڼ���д��֡
        do {
            // �鿴��ȡλ��
            if (WaitForSingleObject(readMutex, INFINITE) != WAIT_OBJECT_0) {
                // ������
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
        if (WaitForSingleObject(writeMutex, INFINITE) != WAIT_OBJECT_0) {
            // ������
        }
        *writeIndexBuf = writeIndex;
        ReleaseMutex(writeMutex);
    }
}

void createShareMem()
{
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
    //memset(pBuf, 0, BUF_SIZE);

    //ѭ����������
    loopSend(pBuf);
    
    // ���ӳ����ͼ
    UnmapViewOfFile(pBuf);

    // �ر��ļ�ӳ�����
    CloseHandle(hMapFile);
}


int main(int argc, char** argv)
{
    createShareMem();

    return 0;
}