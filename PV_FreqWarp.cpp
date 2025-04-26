#include "SC_fftlib.h"
#include "FFT_UGens.h"
#include "SCComplex.h"
#include "SC_PlugIn.hpp"
#include <vector>
#include <algorithm>

InterfaceTable *ft;

struct PV_FreqWarp : PV_Unit
{
  float m_fmagbufnum;
  SndBuf *m_magbuf;
};

void PV_FreqWarp_Ctor(PV_FreqWarp *unit);
void PV_FreqWarp_next(PV_FreqWarp *unit, int inNumSamples);

void PV_FreqWarp_Ctor(PV_FreqWarp *unit)
{
	SETCALC(PV_FreqWarp_next);
    ZOUT0(0) = ZIN0(0);
    unit->m_fmagbufnum = -1e9f;
}

void PV_FreqWarp_next(PV_FreqWarp *unit, int inNumSamples)
{
   PV_GET_BUF
   SCPolarBuf *p = ToPolarApx(buf);
  float fmagbufnum = ZIN0(1);
  //data buffer
  if (fmagbufnum != unit->m_fmagbufnum) {
		uint32 magbufnum = (uint32)fmagbufnum;
		World *world = unit->mWorld; //World represents all the data structures
    //available on the SC server, such as mNumSndBufs, mSndBufs etc etc
		if (magbufnum >= world->mNumSndBufs) magbufnum = 0;
		unit->m_magbuf = world->mSndBufs + magbufnum;
	}

  SndBuf *magbuf = unit->m_magbuf;

	if(!magbuf) { //if magbuf does not exist, returns an error
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
		return;
	}
	float *magbufData __attribute__((__unused__)) = magbuf->data;
	if (!magbufData) { //if magBufData is not empty
	    OUT0(0) = -1.f; //ClearUnitOutputs(unit, inNumSamples);
		return;
	}

int tableSize = magbuf->samples; //number of samples
float *table = magbufData; //actual data
int32 maxindex = tableSize - 1; // size of the buffer

std::vector<float> warpedMags(maxindex, 0.0f);

//amplitude compensation: I sum the magnitude of every duplicated bins into warpedMags[] and then add them to the corresponding bin to avoid loss of amplitude -> check with the SC output meter
for (int i = 0; i < numbins; ++i) {
	warpedMags[int(table[i])] += p->bin[i].mag / 4; //avoid clipping
}

for (int i = 0; i < int(numbins / 2); ++i) {
    p->bin[i].mag = p->bin[int(table[i])].mag + warpedMags[i];
	p->bin[i].phase = p->bin[int(table[i])].phase;
}
}

PluginLoad(PV_FreqWarp)
	{
		ft = inTable;
		DefineSimpleUnit(PV_FreqWarp);
	}
