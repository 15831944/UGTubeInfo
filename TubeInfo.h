// TubeInfo.h : TubeInfo DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTubeInfoApp
// �йش���ʵ�ֵ���Ϣ������� TubeInfo.cpp
//

class CTubeInfoApp : public CWinApp
{
public:
	CTubeInfoApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
