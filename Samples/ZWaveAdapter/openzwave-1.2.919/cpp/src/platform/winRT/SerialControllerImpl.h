//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Module Name:
//
//      SerialControllerImpl.h
//
//-----------------------------------------------------------------------------

#ifndef _SerialControllerImpl_H
#define _SerialControllerImpl_H

#include <Windows.h>
#include <ppltasks.h>

#include "Defs.h"
#include "platform/SerialController.h"

namespace OpenZWave
{
	class SerialControllerImpl
	{
	private:
		friend class SerialController;

		SerialControllerImpl( SerialController* _owner );
		~SerialControllerImpl();

		bool Open();
		void Close();

		uint32 Write( uint8* _buffer, uint32 _length );
		void StartReadTask();

		Windows::Devices::SerialCommunication::SerialDevice ^ m_serialDevice;
        Concurrency::cancellation_token_source   m_readTaskCancelationTokenSource;

		SerialController*			m_owner;
	};

} // namespace OpenZWave

#endif //_SerialControllerImpl_H

