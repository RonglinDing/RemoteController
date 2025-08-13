
// RemoteClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "RemoteClient.h"
#include "RemoteClientDlg.h"
#include "afxdialogex.h"
#include "Clientsocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoteClientDlg 对话框



CRemoteClientDlg::CRemoteClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTECLIENT_DIALOG, pParent)
	, m_server_address(0)
	, m_nport(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_IPADDRESS_SERVER, m_server_address);
	DDX_Text(pDX, IDC_EDIT_IDPORT, m_nport);
	DDX_Control(pDX, IDC_TREE_DIR, m_tree);
	DDX_Control(pDX, IDC_LIST_fild, m_list);
}

int CRemoteClientDlg::SendCommondPacket(int nCmd, bool autoClose, BYTE* pData, size_t nLength)
{
	UpdateData();
	CClientSocket* pClient = CClientSocket::GetInstance();
	bool ret = pClient->InitSocket(m_server_address, atoi((LPCTSTR)m_nport));
	if (!ret) {
		MessageBox(_T("连接失败"), _T("提示"), MB_OK | MB_ICONINFORMATION);
		return -1;
	}
	CPacket packet(nCmd, pData, nLength);
	ret = pClient->SendData(packet); // 发送测试数据
	TRACE("send ret:%d \r\n", ret);
	int cmd = pClient->DealCommand();
	TRACE("cmd:%d \r\n", cmd);
	if(autoClose) 	pClient->CloseSocket(); // 关闭套接字连接
	return cmd;
}

BEGIN_MESSAGE_MAP(CRemoteClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CRemoteClientDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_TEST2, &CRemoteClientDlg::OnBnClickedButtonTest2)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DIR, &CRemoteClientDlg::OnNMDblclkTreeDir)
	ON_NOTIFY(NM_CLICK, IDC_TREE_DIR, &CRemoteClientDlg::OnNMClickTreeDir)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_fild, &CRemoteClientDlg::OnNMRClickListfild)
	ON_COMMAND(ID_DOWNFILE, &CRemoteClientDlg::OnDownfile)
	ON_COMMAND(ID_DELETEFILE, &CRemoteClientDlg::OnDeletefile)
	ON_COMMAND(ID_OPENFILE, &CRemoteClientDlg::OnOpenfile)
END_MESSAGE_MAP()


// CRemoteClientDlg 消息处理程序

BOOL CRemoteClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	UpdateData();
	m_server_address = 0x7f000001 ; // 初始化服务器地址为
	m_nport = _T("9527"); // 设置默认端口号
	UpdateData(FALSE); // 更新控件数据
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRemoteClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRemoteClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// Replace _L with _T to fix the undefined identifier error
void CRemoteClientDlg::OnBnClickedButtonTest()
{
	SendCommondPacket(1981);
}


void CRemoteClientDlg::OnBnClickedButtonTest2()
{
	int ret = SendCommondPacket(1);
	if (ret == -1) {
		AfxMessageBox(_T("连接失败，请检查服务器地址和端口号"));
	}
	CClientSocket* pClient = CClientSocket::GetInstance();
	std::string drivers = pClient->GetPacket().strData;
	std::string dr;
	m_tree.DeleteAllItems(); // 清空树控件
	for (int i = 0; i < drivers.size(); i++) {
		if (drivers[i] == ',') {
			dr += ":";
			HTREEITEM hItem = m_tree.InsertItem(dr.c_str(), TVI_ROOT, TVI_LAST);
			m_tree.InsertItem(NULL, hItem,TVI_LAST);
			dr.clear(); // 清空当前目录字符串
			continue; // 跳过逗号
		}
		dr += drivers[i]; // 添加字符到当前目录字符串
	}
}

void CRemoteClientDlg::LoadFileCUrrentPath()
{
	HTREEITEM hItem =  m_tree.GetSelectedItem(); // 获取选中的树项
	CString strPath = GetPath(hItem);
	m_list.DeleteAllItems(); // 清空列表控件
	int nCmd = SendCommondPacket(2, false, (BYTE*)(LPCTSTR)strPath, strPath.GetLength());
	CClientSocket* pClient = CClientSocket::GetInstance();
	PFileInfo pInfo = (PFileInfo)pClient->GetPacket().strData.c_str();
	while (pInfo->hasNext) {
		if (!pInfo->isDirectory) {
			m_list.InsertItem(0, pInfo->fileName);
		}
		int cmd = pClient->DealCommand();
		if (cmd < 0)break;
		pInfo = (PFileInfo)CClientSocket::GetInstance()->GetPacket().strData.c_str(); // 获取文件信息
	}
	pClient->CloseSocket();
}

void CRemoteClientDlg::LoadFileInfo()
{
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	m_tree.ScreenToClient(&ptMouse);

	HTREEITEM hTreeSelected = m_tree.HitTest(ptMouse, 0);
	if (hTreeSelected == NULL)
		return;
	if(m_tree.GetChildItem(hTreeSelected) == NULL)
		return; // 确保获取到子项
	DeleteTreeChildrenItem(hTreeSelected);
	m_list.DeleteAllItems(); // 清空列表控件
	CString strPath = GetPath(hTreeSelected);
	int nCmd = SendCommondPacket(2, false, (BYTE*)(LPCTSTR)strPath, strPath.GetLength());
	CClientSocket* pClient = CClientSocket::GetInstance();
	PFileInfo pInfo = (PFileInfo)pClient->GetPacket().strData.c_str();
	while (pInfo->hasNext) {
		if (pInfo->isDirectory) {
			if (CString(pInfo->fileName) == "." || (CString(pInfo->fileName) == "..")) {
				int cmd = pClient->DealCommand();
				if (cmd < 0)break;
				pInfo = (PFileInfo)CClientSocket::GetInstance()->GetPacket().strData.c_str(); // 获取文件信息

				continue; // 跳过当前目录
			}
			HTREEITEM hTemp = m_tree.InsertItem(pInfo->fileName, hTreeSelected, TVI_LAST);

			m_tree.InsertItem("", hTemp, TVI_LAST);
		}
		else {
			m_list.InsertItem(0, pInfo->fileName);
		}
		int cmd = pClient->DealCommand();
		if (cmd < 0)break;
		pInfo = (PFileInfo)CClientSocket::GetInstance()->GetPacket().strData.c_str(); // 获取文件信息
	}
	pClient->CloseSocket();
}

CString CRemoteClientDlg::GetPath(HTREEITEM hTree) {
	CString strRet, strTmp;
	do {
		strTmp = m_tree.GetItemText(hTree);
		strRet = strTmp + '\\' + strRet;
		hTree = m_tree.GetParentItem(hTree);
	} while (hTree != NULL);
	return strRet;
}



void CRemoteClientDlg::DeleteTreeChildrenItem(HTREEITEM hTreeItem)
{
	HTREEITEM hSub = NULL;
	do {
		hSub = m_tree.GetChildItem(hTreeItem);
		if (hSub != NULL) {
			m_tree.DeleteItem(hSub);
		}
	} while (hSub != NULL);
}



void CRemoteClientDlg::OnNMDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	LoadFileInfo(); // 双击加载文件信息
}

void CRemoteClientDlg::OnNMClickTreeDir(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	LoadFileInfo();
}

void CRemoteClientDlg::OnNMRClickListfild(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CPoint ptMouse,ptList;

	GetCursorPos(&ptMouse); // 获取鼠标位置
	ptList = ptMouse; // 将鼠标位置赋值给列表控件坐标
	m_list.ScreenToClient(&ptList); // 转换为客户端坐标
	int nItem = m_list.HitTest(ptList); // 获取鼠标点击的项
	if (nItem <0) return; // 如果没有点击到项，则返回
	CMenu menu; // 创建菜单对象
	menu.LoadMenu(IDR_MENU_RCLICK); // 加载菜单资源
	CMenu* pPopup = menu.GetSubMenu(0); // 获取子菜单	
	if (pPopup != NULL) {
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, this); // 弹出菜单
	}
}

void CRemoteClientDlg::OnDownfile()
{
	int nListSelected = m_list.GetSelectionMark(); // 获取选中项
	CString strFile = m_list.GetItemText(nListSelected, 0); // 获取选中文件名
	CFileDialog fileDlg(FALSE, "*",
		m_list.GetItemText(nListSelected,0), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, this);
	if (fileDlg.DoModal() == IDOK) {
		FILE* pFile = fopen(fileDlg.GetPathName(), "wb+"); // 打开文件以写入二进制数据
		if (pFile == NULL) {
			AfxMessageBox(_T("无法打开文件，请检查路径或权限"));
			return;
		}
		HTREEITEM hSelected = m_tree.GetSelectedItem(); // 获取选中的树项
		strFile = GetPath(hSelected) + strFile; // 获取完整路径
		TRACE("%s\r\n", LPCSTR(strFile));
		CClientSocket* pClient = CClientSocket::GetInstance();

		do {
			int ret = SendCommondPacket(4, false, (BYTE*)(LPCTSTR)strFile, strFile.GetLength()); // 发送下载命令
			if (ret < 0) {
				AfxMessageBox(_T("下载失败，请检查连接状态或文件路径是否正确"));
				TRACE("下载失败%s\r\n", ret);
				break;
			}
			long long nLength = *(long long*)pClient->GetPacket().strData.c_str();
			if (nLength == 0) {
				AfxMessageBox(_T("文件不存在或路径错误"));
				break;
			}
			long long nReceived = 0; // 已接收的字节数	
			while (nReceived < nLength) {
				ret = pClient->DealCommand();
				if (ret < 0) {
					AfxMessageBox(_T("下载失败，请检查连接状态或文件路径是否正确"));
					break;
				}
				fwrite(pClient->GetPacket().strData.c_str(), 1, pClient->GetPacket().strData.size(), pFile); // 写入数据到文件
				nReceived += pClient->GetPacket().strData.size();
			}
		} while (0); // 循环一次即可，因为只需要下载一次文件
		fclose(pFile); // 关闭文件
		pClient->CloseSocket();
	}
}

void CRemoteClientDlg::OnDeletefile()
{
	HTREEITEM hSelected = m_tree.GetSelectedItem();
	int nListSelected = m_list.GetSelectionMark(); // 获取选中项
	CString strFile = m_list.GetItemText(nListSelected, 0); // 获取选中文件名
	CString strPath = GetPath(hSelected) + strFile; // 获取完整路径
	int ret = SendCommondPacket(9, true, (BYTE*)(LPCTSTR)strPath, strPath.GetLength()); // 发送打开命令
	if (ret < 0) {
		AfxMessageBox(_T("删除文件失败"));
	}
	LoadFileCUrrentPath(); // 重新加载文件信息
}

void CRemoteClientDlg::OnOpenfile()
{
	HTREEITEM hSelected = m_tree.GetSelectedItem();
	int nListSelected = m_list.GetSelectionMark(); // 获取选中项
	CString strFile = m_list.GetItemText(nListSelected, 0); // 获取选中文件名
	CString strPath = GetPath(hSelected) + strFile; // 获取完整路径
	int ret = SendCommondPacket(3, true, (BYTE*)(LPCTSTR)strPath, strPath.GetLength()); // 发送打开命令
	if (ret < 0) {
		AfxMessageBox(_T("打开文件失败，请检查连接状态或文件路径是否正确"));
	}

}
