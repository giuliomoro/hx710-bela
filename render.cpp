#include <Bela.h>
#include <libraries/BelaLibpd/BelaLibpd.h>
#include "Hx710.h"

Hx710 hx710;
const unsigned int kClockPin = 0;
const unsigned int kDataPin = 1;

void Bela_userSettings(BelaInitSettings *settings)
{
	settings->uniformSampleRate = 1;
	settings->interleave = 0;
	settings->analogOutputsPersist = 0;
}

bool setup(BelaContext* context, void* userData)
{
	hx710.setup(context, kClockPin, kDataPin);
	return BelaLibpd_setup(context, userData, {});
}

void render(BelaContext* context, void* userData)
{
	bool dataReady = hx710.process(context);
	if(dataReady)
	{
		libpd_float("hx710", hx710.getData());
	}
	BelaLibpd_render(context, userData);
}

void cleanup(BelaContext* context, void* userData)
{
	BelaLibpd_cleanup(context, userData);
}
