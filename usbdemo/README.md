# SwLiveExp 项目实验USBCtrl
* 测试通过 DeviceIoControl 禁用USB端口
 ```
 void CusbdemoDlg::UsbCtrl()
 {
 	HANDLE hDevice;               // handle to the drive to be examined
 	BOOL bResult;                 // results flag
 	DWORD junk;                   // discard results
 	DWORD dwError;
 	// Open the volume
 	hDevice = CreateFile(L"\\\\.\\H:", // drive to open
 		GENERIC_READ | GENERIC_WRITE,
 		FILE_SHARE_READ | FILE_SHARE_WRITE,  // share mode
 		NULL,    // default security attributes
 		OPEN_EXISTING,  // disposition
 		0,       // file attributes
 		NULL);   // don't copy any file's attributes

 	if (hDevice == INVALID_HANDLE_VALUE) // can't open the drive
 	{
 		dwError = GetLastError();
 		AfxMessageBox(L"Fail to create file!");
 		return ;
 	}

 	//Dismount the volume
 	bResult = DeviceIoControl(
 		hDevice,  // handle to volume
 		FSCTL_DISMOUNT_VOLUME,       // dwIoControlCode
 		NULL,                        // lpInBuffer
 		0,                           // nInBufferSize
 		NULL,                        // lpOutBuffer
 		0,                           // nOutBufferSize
 		&junk, // discard count of bytes returned
 		(LPOVERLAPPED) NULL);  // synchronous I/O
 	if (!bResult) // IOCTL failed
 	{
 		dwError = GetLastError();
 		AfxMessageBox(L"Fail to dismount the volume!");
 	}

 	// Close the volume handle
 	bool bRet = CloseHandle(hDevice);
 	if (!bRet)
 	{
 		dwError = GetLastError();
 		AfxMessageBox(L"Fail to close handle!");
 	}
 }
 ```
