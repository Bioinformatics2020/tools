#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>

#if _MSC_VER
#define snprintf _snprintf
#endif

#define STACK_INFO_LEN  1024

void ShowTraceStack(char* szBriefInfo)
{
    static const int MAX_STACK_FRAMES = 12;
    void *pStack[MAX_STACK_FRAMES];
    static char szStackInfo[STACK_INFO_LEN * MAX_STACK_FRAMES];
    static char szFrameInfo[STACK_INFO_LEN];

    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pStack, NULL);
    strcpy(szStackInfo, szBriefInfo == NULL ? "stack traceback:\n" : szBriefInfo);

    for (WORD i = 0; i < frames; ++i) {
        DWORD64 address = (DWORD64)(pStack[i]);

        DWORD64 displacementSym = 0;
        char buffer[sizeof(SYMBOL_INFO)+MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        DWORD displacementLine = 0;
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        if (SymFromAddr(process, address, &displacementSym, pSymbol) && 
        	SymGetLineFromAddr64(process, address, &displacementLine, &line))
        {
            snprintf(szFrameInfo, sizeof(szFrameInfo), "\t%s() at %s:%d(0x%x)\n", 
            	pSymbol->Name, line.FileName, line.LineNumber, (unsigned)pSymbol->Address);
        }
        else
        {
            snprintf(szFrameInfo, sizeof(szFrameInfo), "\terror: %d\n", GetLastError());
        }
        strcat(szStackInfo, szFrameInfo);
    }

    printf("%s", szStackInfo); // 输出到控制台，也可以打印到日志文件中
}

void func2()
{
    bool isError = true;
    if (isError)
    {
        ShowTraceStack((char*)("error in func2\n"));
    }
}

void func1()
{
    int sum = 0;
    for (int i = 0; i < 100; ++i)
        sum += i;

    func2();
}


int main(int argc, char* argv[])
{
    printf("hello world\n");
    __try
    {
        int *p = nullptr;
        *p = 0;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        ShowTraceStack((char*)("zx error\n"));
    }
    

    return 0;
}
