PV_FreqWarp : PV_ChainUGen {
	*new { arg buffer, warpBuf;
		^this.multiNew('control', buffer, warpBuf)
	}
}