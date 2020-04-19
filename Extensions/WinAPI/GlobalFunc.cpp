// Common Include
#include	"common.h"


//全局窗口句柄
HWND CurrentWindowHandle = NULL;
std::deque <HWND> WS;

//窗口锁定情况
bool Lock = false;

//所有创建线程的进程名
std::deque <LPTSTR> RunApplicationName;

//开辟新的内存空间复制，并保存指向这片内存空间的指针
void AddNewApplicationName(LPTSTR lpApplicationName) {
	size_t total_length = _MAX_FNAME + _MAX_EXT;
	LPTSTR ANCL = new WCHAR[total_length];
	wcscpy_s(ANCL, total_length, PathFindFileName(lpApplicationName));
	RunApplicationName.push_back(ANCL);
}

//清理全部空间
void DeleteAllApplicationName() {
	for (auto t = RunApplicationName.begin(); t != RunApplicationName.end(); t++) {
		delete[](*t);
	}
	RunApplicationName.clear();
}

//通过位置清理清理空间
void DeleteRunApplicationNameByPos(size_t pos) {
	delete[] RunApplicationName[pos];

	auto t = RunApplicationName.begin() + pos;
	RunApplicationName.erase(t);	
}

//通过进程名清理清理空间
void DeleteRunApplicationName(LPCTSTR ApplicationName) {
	for (auto t = RunApplicationName.begin(); t != RunApplicationName.end();) {
		//依照是否删除元素更新迭代器
		if (wcscmp(ApplicationName, *t) == 0) {
			delete[](*t);
			t = RunApplicationName.erase(t);
		}
		else {
			t++;
		}
	}
	return;
}

//通过句柄停止全部创建的线程
void StopAllApplication() {
	
	//如果没有运行程序则跳过这一阶段
	if (RunApplicationName.size() == 0) {
		return;
	}
		
	//获取快照
	HANDLE	snapshot;
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//循环遍历
	PROCESSENTRY32* info;
	info = new PROCESSENTRY32;
	info->dwSize = sizeof(PROCESSENTRY32);

	Process32First(snapshot, info);
	while (Process32Next(snapshot, info) != FALSE) {
		for (size_t i = 0; i < RunApplicationName.size(); i++) {
			//进程名一致则结束进程		
			if (wcscmp(RunApplicationName[i], info->szExeFile) == 0) {
				//PROCESS_TERMINATE表示为结束操作打开,FALSE=可继承,info.th32ProcessID=进程ID    
				TerminateProcess(OpenProcess(PROCESS_TERMINATE, FALSE, info->th32ProcessID), 0);
			}
		}
	}
	
	DeleteAllApplicationName();	
	
	delete info;
	return;
}


