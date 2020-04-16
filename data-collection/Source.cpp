/*
 * File: Source.cpp 
 * Author: Kurt Burdick 
 *
 * Created 12/4/2019
 * Last modified: 4/15/2020
 *
 * Purpose: To set up IQ and Real Time data collection for the ICSRT
 *	through the use of the Signal Hound SA-44B USB Spectrum Analyzer.
 *	Their are built-in functions within the Signal Hound SDK that have been modified to
 *	write output data into CSV files for analysis and plotting. 
 *
 * Test plots of the preliminary data collection are done with a system call 
 * to an R program that plots the CSV data created in this program. (if I have time and thorough testing)
 *
 * NOTE:
 *		Please refer to the Signal Hound Software Development Kit for genearl API information and 
 *		function documentation/specifications.
 *
 *		SDK link:	https://signalhound.com/software/signal-hound-software-development-kit-sdk/
 *		home site:	https://signalhound.com/
 */

// include directives for the SDK
#include "sa_api.h"
#pragma comment(lib, "sa_api.lib")

#include <iostream>
#include <fstream>
#include <stdbool.h>

// Radio data collection signal constants
#define FREQ	2.4500e9
#define SPAN	200.00e3
#define RBW		1.0000e3


// forward declaration of functions for compiler processes 
void write_IQData(std::string fileName, saIQPacket inputData, int rowCount);
void write_realtimeData(std::string fileName);
void realTimeSweep();
saIQPacket iqBlockCapture(const int blockSizeIn);


// Real Time data capture function
void realTimeSweep()
{
	int handle = -1;
	saStatus openStatus = saOpenDevice(&handle);
	if (openStatus != saNoError) {
		// Handle unable to open/find device error here
		std::cout << saGetErrorString(openStatus) << std::endl;
		return;
	}

	// Configure real-time analysis to be centered on a local
	// FM broadcast frequency, with a 1kHz RBW.
	// Set a frame rate of 30fps, and 100dB height on persistence frames.
	saConfigCenterSpan(handle, FREQ, SPAN);
	saConfigAcquisition(handle, SA_MIN_MAX, SA_LOG_SCALE);
	saConfigLevel(handle, -10.0);
	saConfigSweepCoupling(handle, 1.0e3, 1.0e3, true);
	saConfigRealTime(handle, 100.0, 30);

	// Initialize the device with the configuration just set
	saStatus initiateStatus = saInitiate(handle, SA_REAL_TIME, 0);
	if (initiateStatus != saNoError) {
		// Unable to initialize
		std::cout << saGetErrorString(initiateStatus) << std::endl;
		return;
	}

	// Get sweep and frame characteristics
	int sweepLen;
	double startFreq, binSize;
	saQuerySweepInfo(handle, &sweepLen, &startFreq, &binSize);
	int frameWidth, frameHeight;
	saQueryRealTimeFrameInfo(handle, &frameWidth, &frameHeight);

	// Allocate memory for the sweep and frame
	float *max = new float[sweepLen];
	float *frame = new float[frameWidth * frameHeight];

	// Get 30 frames and sweeps, representing 1 second of real-time analysis
	int frames = 0;
	while (frames < 30) {
		// Don't care about alpha frame, just pass NULL
		saStatus sweepStatus = saGetRealTimeFrame(handle, max, frame, nullptr);
		frames++;

		// output the data to the console for first viewing
		std::cout << *frame << ',' << *max << '\n';

	}

	// call write data function while inside this function

	saAbort(handle);
	delete[] max;
	delete[] frame;

	saCloseDevice(handle);
}


/*
 * This function shows configuring the device and retrieving one
 * block of IQ data of a given length.
 *
 */

saIQPacket iqBlockCapture(const int blockSizeIn)
{
	int handle = -1;
	saStatus openStatus = saOpenDevice(&handle);
	if (openStatus != saNoError) {
		// Handle unable to open/find device error here
		std::cout << saGetErrorString(openStatus) << std::endl;
		
		exit(1);
	}


	/* 
	 * CENTER Frequency is 1.4GHz for 21cm hydrogen line
	 * can be configured otherwise for different celestial 
	 * objects of interest
	 *
	 */

	// Set center freq, span is ignored
	saConfigCenterSpan(handle, FREQ, SPAN);
	
	// Set expected input level
	saConfigLevel(handle, -10.0);
	
	// Configure sample rate and bandwidth
	// Sample rate of 486111.11 / 4 and bandwidth of 100kHz
	saConfigIQ(handle, 4, RBW);

	// Initialize the API for IQ streaming. The device will begin streaming
	// until saAbort(), saInitiate(), or saCloseDevice() is called.
	saInitiate(handle, SA_IQ, 0);

	// Verify the sample rate and bandwidth of the IQ stream
	double bandwidth, sampleRate;
	saQueryStreamInfo(handle, 0, &bandwidth, &sampleRate);

	// Specify how many IQ samples to collect
	const int BLOCK_SIZE = blockSizeIn;

	saIQPacket pkt;
	pkt.iqData = new float[BLOCK_SIZE * 2];
	pkt.iqCount = BLOCK_SIZE;
	// By setting purge to true, you indicate to the API, that you wish
	// the acquisition of the IQ data block to occur after you call
	// the saGetIQData() function. See the iqStream example for an example
	// of setting this value to false.
	pkt.purge = true;

	// Retrieve IQ data
	saStatus iqStatus = saGetIQData(handle, &pkt);

	// At this point, BLOCK_SIZE IQ data samples have been retrieved and
	//  stored in the array created at pkt.iqData
	// saGetIQData can continue to be called to retrieve more IQ samples
	for (int i = 0; i < BLOCK_SIZE; i++) {

		// output data to the console
		std::cout << *pkt.iqData << std::endl;
	}

	// close device connection
	saAbort(handle);
	saCloseDevice(handle);

	return pkt;
}



/*
 * write Real-Time data captured by the analyzer into a text file
 */
 void write_realtimeData(std::string fileName) {

	//create output file
	std::ofstream outFile(fileName);

	if (outFile) {

		while (1) {

			// outFile << someData << ',' << someOtherData? << '\n';

		}
	}

	else {
		throw "File not Found!";
	}

}


/*
 * Write IQ data captured from the analyzer array into a text file 
 * rowCount is the amount of lines of data you want saved
 */
 void write_IQData(std::string fileName, saIQPacket inputData, int rowCount) {
	
	std::ofstream outFile(fileName);

	if (outFile) {

		// create headers/labels for columns
		outFile << "iq-data" << ',' << "int i" << '\n';

		// i value allows plotting of iq data preliminarily  
		int i = 0;

		// change loop variable to control how much data is collected
		while (i < rowCount) {

			outFile << *inputData.iqData << ',' << i << '\n';
			i++;
		}
	}

	else {
		throw "File Not Found";
	}
}



/*
 * Main Execution of Data collection Programs
 *
 * IQ data written into a .txt file named: "iq-data-test.txt"
 * Real-Time data written into a .txt file named: "real-time-test.txt"
 */
int main() {


	// create data arrays
	saIQPacket pktIn;

	// Run IQ data collection tests
	// Specify the amount of data to  capture 
	//	NOTE: Ensure that the amount of samples being written 
	//			is not larger than the argument to the capture 
	//			(data collection) function value. 

	// DEFINITELY INCREASE THE NUMBER OF SAMPLES
	// TO TEST AND SEE IF WE CAN COLLECT A MORE COMPLEX SIGNAL
	pktIn = iqBlockCapture(10000);
	write_IQData("iq-data-test.txt", pktIn, 10000);

	std::cout << "\niq data collection complete. \n" << std::endl;
	

	// Run real time data collection tests
	//realTimeSweep();

	std::cout << "\nreal time data collection complete. \n" << std::endl;
	

	// Make system calls to R plotting programs
	system("pause");


	// reallocate system memory
	delete[] pktIn.iqData;


	return 0;
}