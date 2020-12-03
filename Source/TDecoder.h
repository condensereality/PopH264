#pragma once

#include "Array.hpp"
#include "HeapArray.hpp"
#include "SoyTime.h"
#include <functional>

class SoyPixelsImpl;

namespace PopH264
{
	class TDecoder;
	
	class TInputNaluPacket;
	typedef uint32_t FrameNumber_t;
}

class PopH264::TInputNaluPacket
{
public:
	Array<uint8_t>	mData;
	uint32_t		mFrameNumber = 0;	//	warning, as we've split data into multiple nalu-packets per-frame, this is NOT unique
};


class PopH264::TDecoder
{
public:
	TDecoder(std::function<void(const SoyPixelsImpl&,FrameNumber_t)> OnDecodedFrame);
	
	void			Decode(ArrayBridge<uint8_t>&& PacketData,FrameNumber_t FrameNumber);

	//	gr: this has a callback because of flushing old packets. Need to overhaul the framenumber<->packet relationship
	void			PushEndOfStream();
	
protected:
	void			OnDecodedFrame(const SoyPixelsImpl& Pixels,FrameNumber_t FrameNumber);
	void			OnDecodedEndOfStream();
	virtual bool	DecodeNextPacket()=0;	//	returns true if more data to proccess
	
	bool			HasPendingData()		{	return !mPendingDatas.IsEmpty();	}
	bool			PopNalu(ArrayBridge<uint8_t>&& Buffer,FrameNumber_t& FrameNumber);
	void			UnpopNalu(ArrayBridge<uint8_t>&& Buffer,FrameNumber_t FrameNumber);
	
private:
	std::mutex				mPendingDataLock;
	//	this is currently a bit ineffecient (pool for quick fix?) but we need to keep input frame numbers in sync with data
	Array<std::shared_ptr<TInputNaluPacket>>	mPendingDatas;
	bool					mPendingDataFinished = false;	//	when we know we're at EOS
	
	std::function<void(const SoyPixelsImpl&,FrameNumber_t)>	mOnDecodedFrame;
};
