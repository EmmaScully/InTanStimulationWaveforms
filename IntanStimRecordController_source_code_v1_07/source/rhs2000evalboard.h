//----------------------------------------------------------------------------------
// rhs2000evalboard.h
//
// Intan Technologies RHS2000 Interface API
// Rhs2000EvalBoard Class Header File
// Version 1.01 (28 March 2017)
//
// Copyright (c) 2013-2017 Intan Technologies LLC
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any applications that
// use Intan Technologies integrated circuits, and to alter it and redistribute it
// freely.
//
// See http://www.intantech.com for documentation and product information.
//----------------------------------------------------------------------------------

#ifndef RHS2000EVALBOARD_H
#define RHS2000EVALBOARD_H

#define MAX_NUM_SPI_PORTS 4
#define USB_BUFFER_SIZE 2400000
#define RHYTHM_BOARD_ID 800
#define MAX_NUM_DATA_STREAMS 8
#define FIFO_CAPACITY_WORDS 67108864
#define RHS_BOARD_MODE 14

// The maximum number of Rhs2000DataBlock objects we will need is set by the need
// to perform electrode impedance measurements at very low frequencies.
// (Maximum command length = 1024 for one period; seven periods required in worst case.)
#define MAX_NUM_BLOCKS 60

#include <queue>
#include <mutex>

using namespace std;

class okCFrontPanel;
class Rhs2000DataBlock;
class Rhs2000Registers;

class Rhs2000EvalBoard
{

public:
	Rhs2000EvalBoard();
    ~Rhs2000EvalBoard();

	int open();
	bool uploadFpgaBitfile(string filename);
	void initialize();

	enum AmplifierSampleRate {
		SampleRate1000Hz,
		SampleRate1250Hz,
		SampleRate1500Hz,
		SampleRate2000Hz,
		SampleRate2500Hz,
		SampleRate3000Hz,
		SampleRate3333Hz,
		SampleRate4000Hz,
		SampleRate5000Hz,
		SampleRate6250Hz,
		SampleRate8000Hz,
		SampleRate10000Hz,
		SampleRate12500Hz,
		SampleRate15000Hz,
		SampleRate20000Hz,
		SampleRate25000Hz,
		SampleRate30000Hz,
        SampleRate40000Hz  // Note: This sample rate does not work reliably in the FPGA
	};

	bool setSampleRate(AmplifierSampleRate newSampleRate);
	double getSampleRate() const;
	AmplifierSampleRate getSampleRateEnum() const;

	enum AuxCmdSlot {
        AuxCmd1 = 0,
        AuxCmd2 = 1,
        AuxCmd3 = 2,
        AuxCmd4 = 3
	};

	enum BoardPort {
		PortA,
		PortB,
		PortC,
		PortD
	};

	void uploadCommandList(const vector<unsigned int> &commandList, AuxCmdSlot auxCommandSlot);
	void printCommandList(const vector<unsigned int> &commandList) const;
    void selectAuxCommandLength(AuxCmdSlot auxCommandSlot, int loopIndex, int endIndex);

	void resetBoard();
    void resetFpga();
	void setContinuousRunMode(bool continuousMode);
	void setMaxTimeStep(unsigned int maxTimeStep);
	void run();
    bool isRunning();
    unsigned int getNumWordsInFifo();
    unsigned int getLastNumWordsInFifo();
    unsigned int getLastNumWordsInFifo(bool& hasBeenUpdated);
	static unsigned int fifoCapacityInWords();

	void setCableDelay(BoardPort port, int delay);
	void setCableLengthMeters(BoardPort port, double lengthInMeters);
	void setCableLengthFeet(BoardPort port, double lengthInFeet);
	double estimateCableLengthMeters(int delay) const;
	double estimateCableLengthFeet(int delay) const;

	void setDspSettle(bool enabled);
    void setAllDacsToZero();

	enum BoardDataSource {
		PortA1 = 0,
		PortA2 = 1,
		PortB1 = 2,
		PortB2 = 3,
		PortC1 = 4,
		PortC2 = 5,
		PortD1 = 6,
		PortD2 = 7
	};

	void enableDataStream(int stream, bool enabled);
	int getNumEnabledDataStreams() const;

	void getTtlIn(int ttlInArray[]);

	void setDacManual(int value);

	void setLedDisplay(int ledArray[]);
    void setSpiLedDisplay(int ledArray[]);

	void enableDac(int dacChannel, bool enabled);
    void setDacGain(int gain);
    void setAudioNoiseSuppress(int noiseSuppress);
	void selectDacDataStream(int dacChannel, int stream);
	void selectDacDataChannel(int dacChannel, int dataChannel);
    void enableDacHighpassFilter(bool enable);
    void setDacHighpassFilter(double cutoff);
    void setDacThreshold(int dacChannel, int threshold, bool trigPolarity);

	void flush();
	bool readDataBlock(Rhs2000DataBlock *dataBlock);
    long readDataBlocksRaw(int numBlocks, unsigned char* buffer);
	bool readDataBlocks(int numBlocks, queue<Rhs2000DataBlock> &dataQueue);
	int queueToFile(queue<Rhs2000DataBlock> &dataQueue, std::ofstream &saveOut);
    int getBoardMode();
	int getCableDelay(BoardPort port) const;
	void getCableDelay(vector<int> &delays) const;
	void enableDcAmpConvert(bool enable);
	void setExtraStates(unsigned int extraStates);

	void configureChip(Rhs2000Registers *chipRegisters);
	void runDummyCommands(Rhs2000Registers *chipRegisters);
	void configureRegister(Rhs2000Registers *chipRegisters, int reg);

    int readDigitalInManual(bool& expanderBoardDetected);
    void readDigitalInExpManual();

    void setDacRerefSource(int stream, int channel);
    void enableDacReref(bool enabled);

    void resetSequencers();

    // Stimulation sequencer register addresses
    enum StimRegister {
        TriggerParams = 0,
        StimParams = 1,
        EventAmpSettleOn = 2,
        EventAmpSettleOff = 3,
        EventStartStim = 4,
        EventStimPhase2 = 5,
        EventStimPhase3 = 6,
        EventEndStim = 7,
        EventRepeatStim = 8,
        EventChargeRecovOn = 9,
        EventChargeRecovOff = 10,
        EventAmpSettleOnRepeat = 11,
        EventAmpSettleOffRepeat = 12,
        EventEnd = 13,
        DacBaseline = 9,
        DacPositive = 10,
        DacNegative = 11
    };

    enum StimShape {
        Biphasic = 0,
        BiphasicWithInterphaseDelay = 1,
        Triphasic = 2,
        Monophasic = 3,
		Triangular = 4,
        Ramp = 5,
        RampWithInterphaseDelay = 6
    };

    void setStimCmdMode(bool enabled);
    void programStimReg(int stream, int channel, StimRegister reg, int value);
    void configureStimTrigger(int stream, int channel, int triggerSource, bool triggerEnabled, bool edgeTriggered, bool triggerOnLow);
    void configureStimPulses(int stream, int channel, int numPulses, StimShape shape, bool negStimFirst);
    void setAnalogInTriggerThreshold(double voltageThreshold);
    void setManualStimTrigger(int trigger, bool triggerOn);

    void enableAuxCommandsOnAllStreams();
    void enableAuxCommandsOnOneStream(int stream);

    void setGlobalSettlePolicy(bool settleWholeHeadstageA, bool settleWholeHeadstageB, bool settleWholeHeadstageC,
                               bool settleWholeHeadstageD, bool settleAllHeadstages);

    void setTtlOutMode(bool mode1, bool mode2, bool mode3, bool mode4, bool mode5, bool mode6, bool mode7, bool mode8);
    void setAmpSettleMode(bool useFastSettle);
    void setChargeRecoveryMode(bool useSwitch);

private:
	okCFrontPanel *dev;
	AmplifierSampleRate sampleRate;
    unsigned int usbBufferSize;
	int numDataStreams; // total number of data streams currently enabled
	int dataStreamEnabled[MAX_NUM_DATA_STREAMS]; // 0 (disabled) or 1 (enabled)
	vector<int> cableDelay;

    // Methods in this class are designed to be thread-safe.  This variable is used to ensure that.
    std::mutex okMutex;

	// Buffer for reading bytes from USB interface
    unsigned char* usbBuffer;

	// Buffers for writing bytes to command RAM
	unsigned char commandBufferMsw[65536];
	unsigned char commandBufferLsw[65536];

	// Opal Kelly module USB interface endpoint addresses
	enum OkEndPoint {
		WireInResetRun = 0x00,
		WireInMaxTimeStepLsb = 0x01,
		WireInMaxTimeStepMsb = 0x02,
		WireInDataFreqPll = 0x03,
		WireInMisoDelay = 0x04,
        WireInStimCmdMode = 0x05,
        WireInStimRegAddr = 0x06,
        WireInStimRegWord = 0x07,
		WireInDcAmpConvert = 0x08,
		WireInExtraStates = 0x09,
        WireInDacReref = 0x0a,
//		unused = 0x0b,
        WireInAuxEnable = 0x0c,
        WireInGlobalSettleSelect = 0x0d,
//		unused = 0x0e,
        WireInAdcThreshold = 0x0f,
        WireInSerialDigitalInCntl = 0x10,
		WireInLedDisplay = 0x11,
        WireInManualTriggers = 0x12,
        WireInTtlOutMode = 0x13,
		WireInDataStreamEn = 0x14,
//      unused = 0x15,
		WireInDacSource1 = 0x16,
		WireInDacSource2 = 0x17,
		WireInDacSource3 = 0x18,
		WireInDacSource4 = 0x19,
		WireInDacSource5 = 0x1a,
		WireInDacSource6 = 0x1b,
		WireInDacSource7 = 0x1c,
		WireInDacSource8 = 0x1d,
		WireInDacManual = 0x1e,
		WireInMultiUse = 0x1f,

		TrigInDcmProg = 0x40,
		TrigInSpiStart = 0x41,
		TrigInRamAddrReset = 0x42,
        TrigInDacThresh = 0x43,
        TrigInDacHpf = 0x44,
        TrigInAuxCmdLength = 0x45,

		WireOutNumWordsLsb = 0x20,
		WireOutNumWordsMsb = 0x21,
		WireOutSpiRunning = 0x22,
		WireOutTtlIn = 0x23,
		WireOutDataClkLocked = 0x24,
		WireOutBoardMode = 0x25,
        WireOutSerialDigitalIn = 0x26,
		WireOutBoardId = 0x3e,
		WireOutBoardVersion = 0x3f,

		PipeInAuxCmd1Msw = 0x80,
		PipeInAuxCmd1Lsw = 0x81,
		PipeInAuxCmd2Msw = 0x82,
		PipeInAuxCmd2Lsw = 0x83,
		PipeInAuxCmd3Msw = 0x84,
		PipeInAuxCmd3Lsw = 0x85,
		PipeInAuxCmd4Msw = 0x86,
		PipeInAuxCmd4Lsw = 0x87,

		PipeOutData = 0xa0
	};

	string opalKellyModelName(int model) const;
	double getSystemClockFreq() const;

	bool isDcmProgDone() const;
	bool isDataClockLocked() const;

    unsigned int lastNumWordsInFifo;
    bool numWordsHasBeenUpdated;
    unsigned int numWordsInFifo();
};

#endif // RHS2000EVALBOARD_H

