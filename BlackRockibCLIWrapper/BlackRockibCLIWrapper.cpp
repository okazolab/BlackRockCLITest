
#include <msclr/marshal_cppstd.h>

#include <msclr\marshal_cppstd.h>


#include <windows.h>
#include "cbsdk.h"
#include <iostream>
#include <string.h>


using namespace System;
using namespace msclr::interop;
using namespace System::Runtime::InteropServices;




public ref class  BlackRockEngine
{
	int UnitIndex = 0;

public:
	BlackRockEngine::BlackRockEngine(int Index)
	{
		UnitIndex = Index;
	}

	~BlackRockEngine()
	{

	}

	bool Start()
	{
		cbSdkConnectionType T;
		cbSdkInstrumentType I;

		//cbSdkResult res1 = cbSdkGetType(0, &T, &I);
		cbSdkResult res = cbSdkOpen(UnitIndex, CBSDKCONNECTION_DEFAULT);
		cbSdkResult res2 = cbSdkGetType(UnitIndex, &T, &I);
		//cbSdkTrialCont *trial=new cbSdkTrialCont();
		if (res != CBSDKRESULT_SUCCESS)
		{
			return false;
		}
		return true;
	}

	void Stop()
	{
		cbSdkResult close = cbSdkClose(UnitIndex);
		if (close != CBSDKRESULT_SUCCESS)
		{
			return;
		}
	}


	array<System::String^>^ BlackRockEngine::getChannelList()
	{
		//std::vector<eemagine::sdk::channel> ChannelList=EAmplifier->getChannelList();
		array<System::String^>^ local = gcnew array<System::String^>(cbNUM_ANALOG_CHANS);
		for (int i = 0; i < cbNUM_FE_CHANS; i++)
		{
			local[i] = "Front end channel " + System::Convert::ToString(i + 1);
		}

		for (int i = 0; i < cbNUM_ANAIN_CHANS; i++)
		{
			local[i] = "Analog input channel " + System::Convert::ToString(i + 1);
		}
		return local;
	}

	int BlackRockEngine::GetAnalogChannelNumber()
	{
		return cbNUM_ANALOG_CHANS;
	}

	//cbSdkTrialCont *trial;

	bool BlackRockEngine::StartAcquisition(int SamplingRateGroupIndex)
	{

		//setup the first channel only (continuous recording at 30kHz, no filter)
		cbPKT_CHANINFO chan_info = { 0 };
		//get current channel configuration	

		for (int i = 0; i < cbNUM_ANALOG_CHANS; i++)
		{
			cbSdkResult r = cbSdkGetChannelConfig(UnitIndex, i + 1, &chan_info);
			//change configuration
			chan_info.smpgroup = SamplingRateGroupIndex;// 5; //continuous sampling rate (30kHz)
									//set channel configuration
			r = cbSdkSetChannelConfig(UnitIndex, i + 1, &chan_info); //note: channels start at 1
		}
		//ask to send trials (only continuous data)                                                                                                                                                 
		cbSdkResult res = cbSdkSetTrialConfig(UnitIndex, 1, 0, 0, 0, 0, 0, 0, false, 0, cbSdk_CONTINUOUS_DATA_SAMPLES, 0, 0, 0, true);

		if (res != CBSDKRESULT_SUCCESS)
		{

			return false;
		}

		return true;
	}



	array<double>^ BlackRockEngine::GetData()//[Out] array<int>^ SampleCounter)
	{		
		cbSdkTrialCont trial;
		trial.time = 0;

		array<int>^ SampleCounter = gcnew array<int>(cbNUM_ANALOG_CHANS);



		for (int i = 0; i < cbNUM_ANALOG_CHANS; i++)
		{
			/// ************************************
			/// it always crashes with 0xc000005 inside of this loop, when the hardware is connected
			/// ************************************
			trial.samples[i] = malloc(cbSdk_CONTINUOUS_DATA_SAMPLES * sizeof(INT16));
		}


		cbSdkResult res1 = cbSdkInitTrialData(UnitIndex, 1, NULL, &trial, NULL, NULL);	

		cbSdkResult res = cbSdkGetTrialData(UnitIndex, 1, NULL, &trial, NULL, NULL);

		int TotalSampleNumber = 0;
		for (int i = 0; i < cbNUM_ANALOG_CHANS; i++)
		{
			SampleCounter[i] = trial.num_samples[i];
			TotalSampleNumber = TotalSampleNumber + SampleCounter[i];
		}

		array<double>^ local = gcnew array<double>(TotalSampleNumber);
		int n = 0;
		if (res == CBSDKRESULT_SUCCESS)
		{
			//if the trial is not empty
			if (trial.count > 0)
			{
				///cout << "Channel 1" << endl;
				for (int i = 0; i < cbNUM_ANALOG_CHANS; i++)
				{
					INT16* myIntPtr = (INT16*)trial.samples[i];
					for (int z = 0; z < trial.num_samples[i]; z++)
					{
						local[n] = myIntPtr[z];
						n++;
					}
					///cout << myIntPtr[z] << endl; //Print each sample
				}
			}
		}

		for (int i = 0; i < cbNUM_ANALOG_CHANS; i++)
		{
			free(trial.samples[i]);
		}
		return local;
	}



	void BlackRockEngine::StopAcquisition()
	{
		/*for (int i = 0; i < cbNUM_ANALOG_CHANS; i++)
		{
			free(trial->samples[i]);
		}

		delete trial;*/
	}
	//free trial

};


