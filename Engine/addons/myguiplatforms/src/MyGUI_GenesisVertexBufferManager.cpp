/*!
	@file
	@author		Albert Semenov
	@date		04/2009
*/
/****************************************************************************
Copyright (c) 2011-2013,WebJet Business Division,CYOU

http://www.genesis-3d.com.cn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "stdneb.h"
#include "MyGUI_GenesisVertexBufferManager.h"
#include "myguiplatforms/include/MyGUI_GenesisVertexBuffer.h"

namespace MyGUI
{
	__ImplementClass(GenesisVertexBufferMgr,'WFBM',Core::RefCounted);
	__ImplementImageSingleton(GenesisVertexBufferMgr);

	GenesisVertexBufferMgr::GenesisVertexBufferMgr()
	{
		__ConstructImageSingleton;
	}

	GenesisVertexBufferMgr::~GenesisVertexBufferMgr()
	{
		__DestructImageSingleton;
	}

	void GenesisVertexBufferMgr::AddVertexBuffer(GenesisVertexBuffer* const& pBuffer)
	{
		IndexT res = m_AllVertexBuffers.FindIndex(pBuffer);

		if (res == InvalidIndex)
		{
			m_AllVertexBuffers.Append(pBuffer);
		}
	}

	void GenesisVertexBufferMgr::RemoveVertexBuffer( GenesisVertexBuffer* const& pBuffer )
	{
		IndexT res = m_AllVertexBuffers.FindIndex(pBuffer);

		if (res != InvalidIndex)
		{
			m_AllVertexBuffers.EraseIndex(res);
		}
	}

	void GenesisVertexBufferMgr::ResetAllBuffers() const
	{
		SizeT nCount = m_AllVertexBuffers.Size();

		for (IndexT i = 0; i < nCount; ++i)
		{
			m_AllVertexBuffers[i]->resizeVertexBuffer();
		}
	}


};