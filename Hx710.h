#include <Bela.h>

class Hx710
{
	public:
		int setup(BelaContext* context, uint8_t dataPin, uint8_t clockPin)
		{
			_dataPin = dataPin;
			_clockPin = clockPin;
			_count = 100000;
			pinMode(context, 0, _clockPin, OUTPUT);
			pinMode(context, 0, _dataPin, INPUT);
			return 0;
		}
		bool process(BelaContext* context)
		{
			bool dataReady = false;
			for(unsigned int n = 0; n < context->digitalFrames; ++n)
			{
				unsigned int loopbackDelay = context->digitalFrames * 2 + 1;
				if(_count > loopbackDelay + kNumBits * 2 + 200) // 200 for good measure
				{
					// data pin goes low means data is
					// ready and a new acquisition cycle
					// can start
					if(digitalRead(context, n, _dataPin) == 0)
					{
						_count = 0;
						_inputWord = 0;
					}
				}
				bool clk = 0;
				if(_count < kDataType * kFramesPerClock) // write the clock output
					clk = !(_count % kFramesPerClock);
				digitalWriteOnce(context, n, _clockPin, clk);
				unsigned int samplingCount = _count - loopbackDelay;
				if(samplingCount < kNumBits * kFramesPerClock)
				{
					// after the loopback delay, read the data in on the falling edge
					if((samplingCount % kFramesPerClock) == kSamplingBit)
					{
						// shift data in on odd frames
						bool bit = digitalRead(context, n, _dataPin);
						_inputWord <<= 1;
						_inputWord |= bit;
					}
				}
				if(loopbackDelay + kNumBits * kFramesPerClock - 1 == _count)
				{
					dataReady = true;
					// input is ready
					// extend sign
					if (_inputWord & 0x800000)
						_inputWord |= 0xFF000000;
				}
				_count++;
			}
			return dataReady;
		}
		uint32_t getData()
		{
			return _inputWord;
		}
	private:
		unsigned int _dataPin;
		unsigned int _clockPin;
		unsigned int _count;
		uint32_t _inputWord;
		unsigned int kFramesPerClock = 2;
		unsigned int kSamplingBit = kFramesPerClock - 1;
		const unsigned int kNumBits = 24;
		// number of clock pulses being written:
		// - 25: differential input
		// - 26: temperature or DVDD-AVDD
		// - 27: differential input, gain 128
		const unsigned int kDataType = 25;
};
