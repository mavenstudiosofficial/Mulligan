// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AkInclude.h"
#include "Modules/ModuleManager.h"
#include "TencentGMEPlugin.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTencentGME, Log, All);


class TENCENTGME_WWISE_API FTencentGMEDevice
{
public:
	static FString GetGMEVersion();
	static int SetUserID(const FString& openID);
	static int SetRoomID(const FString& roomID);

	static void ReceivePlugin_SetReceiveOpenIDWithGameObjectID(AkGameObjectID gameObjectID, const FString& voiceID);
	static void ReceivePlugin_GetReceiveOpenIDWithGameObjectID(AkGameObjectID gameObjectID, FString& voiceID);
	static void SendPlugin_EnableLoopbackWithGameObjectID(AkGameObjectID gameObjectID, bool enableLoopback);
	static bool SendPlugin_GetEnableLoopbackWithGameObjectID(AkGameObjectID gameObjectID);

	static void SetRegion(GMEWWisePlugin_Region region);
	static void SetAudioStreamProfile(GMEWWisePlugin_STREAMPROIFLE profile);
	static void SetLogLevel(GMEWWisePlugin_LogLevel logLevelWrite, GMEWWisePlugin_LogLevel logLevelPrint);
	static void GMEPause();
	static void GMEResume();
	static int GetAudioSendStreamLevel();
	static int GetAudioRecvStreamLevel(const FString& targetID);
	static void AddAudioBlockList(const FString& targetID);
	static void RemoveAudioBlockList(const FString& targetID);
	static void EnableBluetoothMic(bool enable);
	
	static void SetRangeAudioTeamID(int teamID);
	static void SetRangeAudioTeamMode(GMEWWisePlugin_TeamMode teamMode);
	static void SetRangeAudioRecvRange(int range);
	static void SetSelfPosition(int positionX, int positionY, int positionZ);
	
	static int StartRecordingWithRecognition(const FString& speechLanguage);
	static int StartRecording();
	static int StopRecording();
	static int PlayRecordFile(const FString& fileid);
	static int StopPlayFile();
	static int SpeechToText(const FString& fileid, const FString& speechLanguage, const FString& translateLanguage);
	static int GetVoiceFileDuration(const FString& fileid);
	
	static int GetGMEMessage(int& localUTCTime, int& messageType, int& code, FString& message1, FString& message2);
	
	static int SetServerAudioRoute(GMEWWisePlugin_AudioRouteMode sendType, const FString& sendList, GMEWWisePlugin_AudioRouteMode recvType, const FString& recvList);
	static int GetServerAudioRoute(GMEWWisePlugin_AudioRouteMode& sendType, FString& sendListBuf, GMEWWisePlugin_AudioRouteMode& recvType, FString& recvListBuf);

	static int GetMicCount();
	static void GetMicList(DEV_INFO* name, int count);
	static void SelectMicDevice(int micId);
    static int GetCurrentMicDeviceInfo(DEV_INFO* pInfo);
};
