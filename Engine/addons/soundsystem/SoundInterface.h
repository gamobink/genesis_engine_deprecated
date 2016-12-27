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
#ifndef __SoundInterface_H__
#define __SoundInterface_H__

#include "messaging/handlerthreadbase.h"
#include "threading/safequeue.h"
#include "interface/interfacehandlerbase.h"
#include "interface/interfacebase.h"
#include "SoundSystemSource.h"

namespace Sound
{
	//------------------------------��Ϣ�ඨ��------------------------------------------
	class SoundMsg: public Messaging::Message
	{
		__DeclareClass(SoundMsg);
		__DeclareMsgId;
	public:
		SoundMsg(): mResult(false)
		{  
		};
		void SetResult(bool bOK )
		{
			mResult = bOK;
		}
		bool GetResult(void) const
		{
			return mResult;
		}
	protected:
		bool mResult;	    //	��Ϣ����֮�󣬵õ���ȡ/д�� �ɹ�����ʧ�ܵĽ��
	};
	class SoundDecodeMsg: public SoundMsg
	{
		__DeclareClass(SoundDecodeMsg);
		__DeclareMsgId;
	public:
		SoundDecodeMsg()
		{ }
		void SetDecoder(GPtr<Sound::SoundSystemSource> ss)
		{
			mSoundSource = ss;
		}

		GPtr<Sound::SoundSystemSource> GetDecoder()
		{
			return mSoundSource;
		}

		void SetFileFormat(GENESIS_FILE_FORMAT formate)
		{
			mFileFormat = formate;
		}

		void SetMsgHashCode(IndexT hashCode)
		{
			mHashCode = hashCode;
		}

		GENESIS_FILE_FORMAT GetFileFormat()
		{
			return mFileFormat;
		}

		IndexT GetMsgHashCode()
		{
			return mHashCode;
		}

	protected:
		GPtr<Sound::SoundSystemSource>	mSoundSource;
		GENESIS_FILE_FORMAT				mFileFormat;	
		IndexT							mHashCode;
	};
	//--------------------------------��Դ�߳̽ӿ�----------------------------------------
	class SoundInterface : public Interface::InterfaceBase
	{
		__DeclareSubClass(SoundInterface, Interface::InterfaceBase );
		__DeclareImageSingleton(SoundInterface);
	public:
		SoundInterface();
		virtual ~SoundInterface();

		/**
		* Open  ��ʼ��SoundInterface
		* @return: void  
		* @see: 
		* @remark:  
		*/
		virtual void Open();
	};
	//------------------------------�̴߳�����------------------------------------------
	class SoundHandler : public Interface::InterfaceHandlerBase
	{
		__DeclareSubClass(SoundHandler, Interface::InterfaceHandlerBase);
	public:
		/// constructor
		SoundHandler();
		/// destructor
		virtual ~SoundHandler();

		/// open the handler
		virtual void Open();
		/// close the handler
		virtual void Close();
		/// handle a message, return true if handled
		virtual bool HandleMessage(const GPtr<Messaging::Message>& msg);

	protected:
		void OnDecode( const GPtr<SoundDecodeMsg>& msg );
	};
}	//	namespace Sound





#endif // __resourceinterface_H__
