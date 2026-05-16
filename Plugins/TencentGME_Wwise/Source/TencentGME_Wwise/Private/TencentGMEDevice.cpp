// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TencentGMEDevice.h"
#include "TencentGMEPlugin.h"

DEFINE_LOG_CATEGORY(LogTencentGME);

FString FTencentGMEDevice::GetGMEVersion()
{
	return FString(UTF8_TO_TCHAR(GMEWWisePlugin_GetVersion()));
}

int FTencentGMEDevice::SetUserID(const FString& openID)
{
	return GMEWWisePlugin_SetUserID(TCHAR_TO_ANSI(*openID));
}

int FTencentGMEDevice::SetRoomID(const FString& roomID)
{
	return GMEWWisePlugin_SetRoomID(TCHAR_TO_ANSI(*roomID));
}

void FTencentGMEDevice::ReceivePlugin_SetReceiveOpenIDWithGameObjectID(AkGameObjectID gameObjectID, const FString& voiceID)
{
	GMEWWisePlugin_ReceivePlugin_SetReceiveOpenIDWithGameObjectID(gameObjectID, TCHAR_TO_ANSI(*voiceID));
}

void FTencentGMEDevice::ReceivePlugin_GetReceiveOpenIDWithGameObjectID(AkGameObjectID gameObjectID, FString& voiceID)
{
	char TmpVoiceId[128];
	GMEWWisePlugin_ReceivePlugin_GetReceiveOpenIDWithGameObjectID(gameObjectID, TmpVoiceId);
	voiceID = FString(TmpVoiceId);
}

void FTencentGMEDevice::SendPlugin_EnableLoopbackWithGameObjectID(AkGameObjectID gameObjectID, bool enableLoopback)
{
	GMEWWisePlugin_SendPlugin_EnableLoopbackWithGameObjectID(gameObjectID, enableLoopback);
}

bool FTencentGMEDevice::SendPlugin_GetEnableLoopbackWithGameObjectID(AkGameObjectID gameObjectID)
{
	return GMEWWisePlugin_SendPlugin_GetEnableLoopbackWithGameObjectID(gameObjectID) == 0 ? false : true;
}

void FTencentGMEDevice::SetRegion(GMEWWisePlugin_Region region)
{
	GMEWWisePlugin_SetRegion(region);
}

void FTencentGMEDevice::SetAudioStreamProfile(GMEWWisePlugin_STREAMPROIFLE profile)
{
	GMEWWisePlugin_SetAudioStreamProfile(profile);
}

void FTencentGMEDevice::SetLogLevel(GMEWWisePlugin_LogLevel logLevelWrite, GMEWWisePlugin_LogLevel logLevelPrint)
{
	GMEWWisePlugin_SetLogLevel(logLevelWrite, logLevelPrint);
}

void FTencentGMEDevice::GMEPause()
{
	GMEWWisePlugin_Pause();
}

void FTencentGMEDevice::GMEResume()
{
	GMEWWisePlugin_Resume();
}

int FTencentGMEDevice::GetAudioSendStreamLevel()
{
	return GMEWWisePlugin_GetAudioSendStreamLevel();
}

int FTencentGMEDevice::GetAudioRecvStreamLevel(const FString& targetID)
{
	return GMEWWisePlugin_GetAudioRecvStreamLevel(TCHAR_TO_ANSI(*targetID));
}

void FTencentGMEDevice::AddAudioBlockList(const FString& targetID)
{
	GMEWWisePlugin_AddAudioBlockList(TCHAR_TO_ANSI(*targetID));
}

void FTencentGMEDevice::RemoveAudioBlockList(const FString& targetID)
{
	GMEWWisePlugin_RemoveAudioBlockList(TCHAR_TO_ANSI(*targetID));
}

void FTencentGMEDevice::EnableBluetoothMic(bool enable)
{
	GMEWWisePlugin_EnableBluetoothMic(enable);
}

void FTencentGMEDevice::SetRangeAudioTeamID(int teamID)
{
	GMEWWisePlugin_SetRangeAudioTeamID(teamID);
}

void FTencentGMEDevice::SetRangeAudioTeamMode(GMEWWisePlugin_TeamMode teamMode)
{
	GMEWWisePlugin_SetRangeAudioTeamMode(teamMode);
}

void FTencentGMEDevice::SetRangeAudioRecvRange(int range)
{
	GMEWWisePlugin_SetRangeAudioRecvRange(range);
}

void FTencentGMEDevice::SetSelfPosition(int positionX, int positionY, int positionZ)
{
	GMEWWisePlugin_SetSelfPosition(positionX, positionY, positionZ);
}

int FTencentGMEDevice::StartRecordingWithRecognition(const FString& speechLanguage)
{
	return GMEWWisePlugin_StartRecordingWithRecognition(TCHAR_TO_ANSI(*speechLanguage));
}

int FTencentGMEDevice::StartRecording()
{
	return GMEWWisePlugin_StartRecording();
}

int FTencentGMEDevice::StopRecording()
{
	return GMEWWisePlugin_StopRecording();
}

int FTencentGMEDevice::PlayRecordFile(const FString& fileid)
{
	return GMEWWisePlugin_PlayRecordFile(TCHAR_TO_ANSI(*fileid));
}

int FTencentGMEDevice::StopPlayFile()
{
	return GMEWWisePlugin_StopPlayFile();
}

int FTencentGMEDevice::SpeechToText(const FString& fileid, const FString& speechLanguage, const FString& translateLanguage)
{
	return GMEWWisePlugin_SpeechToText(TCHAR_TO_ANSI(*fileid), TCHAR_TO_ANSI(*speechLanguage), TCHAR_TO_ANSI(*translateLanguage));
}

int FTencentGMEDevice::GetVoiceFileDuration(const FString& fileid)
{
	return GMEWWisePlugin_GetVoiceFileDuration(TCHAR_TO_ANSI(*fileid));
}

int FTencentGMEDevice::GetGMEMessage(int& localUTCTime, int& messageType, int& code, FString& message1, FString& message2)
{
	int ret = 0;
	const int bufferSize = 1024;
	char p_message1[bufferSize] = {};
	char p_message2[bufferSize] = {};

	ret = GMEWWisePlugin_GetMessage(&localUTCTime, &messageType, &code, p_message1, bufferSize, p_message2, bufferSize);
	if (ret != -4)
	{
		p_message1[bufferSize - 1] = 0;
		p_message2[bufferSize - 1] = 0;

		message1 = FString(UTF8_TO_TCHAR(p_message1));
		message2 = FString(UTF8_TO_TCHAR(p_message2));
	}

	return ret;
}

int FTencentGMEDevice::SetServerAudioRoute(GMEWWisePlugin_AudioRouteMode sendType, const FString& sendList, GMEWWisePlugin_AudioRouteMode recvType, const FString& recvList)
{
	int ret = 0;
	
	ret = GMEWWisePlugin_SetServerAudioRoute(sendType, TCHAR_TO_ANSI(*sendList), recvType, TCHAR_TO_ANSI(*recvList));
	
	return ret;
}

int FTencentGMEDevice::GetServerAudioRoute(GMEWWisePlugin_AudioRouteMode& sendType, FString& sendListBuf, GMEWWisePlugin_AudioRouteMode& recvType, FString& recvListBuf)
{
	int ret = 0;
	const int bufferSize = 1024;
	char p_sendListBuf[bufferSize] = {};
	char p_recvListBuf[bufferSize] = {};	
	
	ret = GMEWWisePlugin_GetServerAudioRoute(&sendType, p_sendListBuf, bufferSize, &recvType, p_recvListBuf, bufferSize);
	
	if (ret == 0)
	{
		p_sendListBuf[bufferSize - 1] = 0;
		p_recvListBuf[bufferSize - 1] = 0;

		sendListBuf = FString(UTF8_TO_TCHAR(p_sendListBuf));
		recvListBuf = FString(UTF8_TO_TCHAR(p_recvListBuf));		
	}
	
	return ret;
}

int FTencentGMEDevice::GetMicCount()
{
	return GMEWWisePlugin_GetMicCount();
}

void FTencentGMEDevice::GetMicList(DEV_INFO* micList, int count)
{
	
	GMEWWisePlugin_GetMicList(micList, count);
}

void FTencentGMEDevice::SelectMicDevice(int micId)
{
	GMEWWisePlugin_SelectMicDevice(micId);
}
int FTencentGMEDevice::GetCurrentMicDeviceInfo(DEV_INFO* pInfo)
{
	return GMEWWisePlugin_GetCurrentMicDeviceInfo(pInfo);
}
