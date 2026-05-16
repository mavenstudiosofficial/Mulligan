// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TencentGMEBlueprintLibrary.h"
#include "TencentGMEDevice.h"
#include "TencentGMEPlugin.h"
#include "AkAudioDevice.h"
#include "AkComponent.h"

FString UTencentGMEBlueprintLibrary::GetGMEVersion()
{
	return FTencentGMEDevice::GetGMEVersion();
}

int UTencentGMEBlueprintLibrary::SetUserID(const FString& openID)
{
	return FTencentGMEDevice::SetUserID(openID);
}

int UTencentGMEBlueprintLibrary::SetRoomID(const FString& roomID)
{
	return FTencentGMEDevice::SetRoomID(roomID);
}

void UTencentGMEBlueprintLibrary::ReceivePlugin_SetReceiveOpenIDWithAkComponent(UAkComponent* AkComponent, const FString& voiceID)
{
	FTencentGMEDevice::ReceivePlugin_SetReceiveOpenIDWithGameObjectID(AkComponent->GetAkGameObjectID(), voiceID);
}

void UTencentGMEBlueprintLibrary::ReceivePlugin_GetReceiveOpenIDWithAkComponent(UAkComponent* AkComponent, FString& voiceID)
{
	FTencentGMEDevice::ReceivePlugin_GetReceiveOpenIDWithGameObjectID(AkComponent->GetAkGameObjectID(), voiceID);
}

void UTencentGMEBlueprintLibrary::SendPlugin_EnableLoopbackWithAkComponent(UAkComponent* AkComponent, bool enableLoopback)
{
	FTencentGMEDevice::SendPlugin_EnableLoopbackWithGameObjectID(AkComponent->GetAkGameObjectID(), enableLoopback);
}

bool UTencentGMEBlueprintLibrary::SendPlugin_GetEnableLoopbackWithAkComponent(UAkComponent* AkComponent)
{
	return FTencentGMEDevice::SendPlugin_GetEnableLoopbackWithGameObjectID(AkComponent->GetAkGameObjectID());
}

void UTencentGMEBlueprintLibrary::ReceivePlugin_SetReceiveOpenIDWithActor(AActor* Actor, const FString& voiceID)
{
	UAkComponent* AkComp = GetAkComponentFromActor(Actor);
	if (AkComp)
	{
		ReceivePlugin_SetReceiveOpenIDWithAkComponent(AkComp, voiceID);
	}
}

void UTencentGMEBlueprintLibrary::ReceivePlugin_GetReceiveOpenIDWithActor(AActor* Actor, FString& voiceID)
{
	UAkComponent* AkComp = GetAkComponentFromActor(Actor);
	if (AkComp)
	{
		ReceivePlugin_GetReceiveOpenIDWithAkComponent(AkComp, voiceID);
	}
}

void UTencentGMEBlueprintLibrary::SendPlugin_EnableLoopbackWithActor(AActor* Actor, bool enableLoopback)
{
	UAkComponent* AkComp = GetAkComponentFromActor(Actor);
	if (AkComp)
	{
		SendPlugin_EnableLoopbackWithAkComponent(AkComp, enableLoopback);
	}
}

bool UTencentGMEBlueprintLibrary::SendPlugin_GetEnableLoopbackWithActor(AActor* Actor)
{
	bool ret = false;
	UAkComponent* AkComp = GetAkComponentFromActor(Actor);
	if (AkComp)
	{
		ret = SendPlugin_GetEnableLoopbackWithAkComponent(AkComp);
	}

	return ret;
}

void UTencentGMEBlueprintLibrary::SetRegion(UTencentGME_Region region)
{
	GMEWWisePlugin_Region gmesdkRegion = (GMEWWisePlugin_Region)region;
	FTencentGMEDevice::SetRegion(gmesdkRegion);
}

void UTencentGMEBlueprintLibrary::SetAudioStreamProfile(UTencentGME_StreamProfile profile)
{
	GMEWWisePlugin_STREAMPROIFLE gmesdkProfile = (GMEWWisePlugin_STREAMPROIFLE)profile;
	FTencentGMEDevice::SetAudioStreamProfile(gmesdkProfile);
}

void UTencentGMEBlueprintLibrary::SetLogLevel(UTencentGME_LogLevel logLevelWrite, UTencentGME_LogLevel logLevelPrint)
{
	GMEWWisePlugin_LogLevel gmesdkLogLevelWrite = (GMEWWisePlugin_LogLevel)logLevelWrite;
	GMEWWisePlugin_LogLevel gmesdkLogLevelPrint = (GMEWWisePlugin_LogLevel)logLevelPrint;
	FTencentGMEDevice::SetLogLevel(gmesdkLogLevelWrite, gmesdkLogLevelPrint);
}

void UTencentGMEBlueprintLibrary::GMEPause()
{
	FTencentGMEDevice::GMEPause();
}

void UTencentGMEBlueprintLibrary::GMEResume()
{
	FTencentGMEDevice::GMEResume();
}

int UTencentGMEBlueprintLibrary::GetAudioSendStreamLevel()
{
	return FTencentGMEDevice::GetAudioSendStreamLevel();
}

int UTencentGMEBlueprintLibrary::GetAudioRecvStreamLevel(const FString& targetID)
{
	return FTencentGMEDevice::GetAudioRecvStreamLevel(targetID);
}

void UTencentGMEBlueprintLibrary::AddAudioBlockList(const FString& targetID)
{
	FTencentGMEDevice::AddAudioBlockList(targetID);
}

void UTencentGMEBlueprintLibrary::RemoveAudioBlockList(const FString& targetID)
{
	FTencentGMEDevice::RemoveAudioBlockList(targetID);
}

void UTencentGMEBlueprintLibrary::EnableBluetoothMic(bool enable)
{
	FTencentGMEDevice::EnableBluetoothMic(enable);
}

UAkComponent* UTencentGMEBlueprintLibrary::GetAkComponentFromActor(AActor* Actor)
{
	UAkComponent* AkComp = nullptr;
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice && Actor)
	{
		AkComp = AkAudioDevice->GetAkComponent(Actor->GetRootComponent(), FName(), NULL, EAttachLocation::KeepRelativeOffset);
	}

	return AkComp;
}

void UTencentGMEBlueprintLibrary::SetRangeAudioTeamID(int teamID)
{
	FTencentGMEDevice::SetRangeAudioTeamID(teamID);
}

void UTencentGMEBlueprintLibrary::SetRangeAudioTeamMode(UTencentGME_TeamMode teamMode)
{
	FTencentGMEDevice::SetRangeAudioTeamMode((GMEWWisePlugin_TeamMode)teamMode);
}

void UTencentGMEBlueprintLibrary::SetRangeAudioRecvRange(int range)
{
	FTencentGMEDevice::SetRangeAudioRecvRange(range);
}

void UTencentGMEBlueprintLibrary::SetSelfPosition(int positionX, int positionY, int positionZ)
{
	FTencentGMEDevice::SetSelfPosition(positionX, positionY, positionZ);
}

int UTencentGMEBlueprintLibrary::StartRecordingWithRecognition(const FString& speechLanguage)
{
	return FTencentGMEDevice::StartRecordingWithRecognition(speechLanguage);
}
int UTencentGMEBlueprintLibrary::StartRecording()
{
	return FTencentGMEDevice::StartRecording();
}

int UTencentGMEBlueprintLibrary::StopRecording()
{
	return FTencentGMEDevice::StopRecording();
}

int UTencentGMEBlueprintLibrary::PlayRecordFile(const FString& fileid)
{
	return FTencentGMEDevice::PlayRecordFile(fileid);
}

int UTencentGMEBlueprintLibrary::StopPlayFile()
{
	return FTencentGMEDevice::StopPlayFile();
}

int UTencentGMEBlueprintLibrary::SpeechToText(const FString& fileid, const FString& speechLanguage, const FString& translateLanguage)
{
	return FTencentGMEDevice::SpeechToText(fileid, speechLanguage, translateLanguage);
}

int UTencentGMEBlueprintLibrary::GetVoiceFileDuration(const FString& fileid)
{
	return FTencentGMEDevice::GetVoiceFileDuration(fileid);
}

int UTencentGMEBlueprintLibrary::GetGMEMessage(FTencentGmeMessage& TencentGmeMessage)
{
	int UTCTime = 0;
	int MessageType = 0; // Invalid

	int ret = FTencentGMEDevice::GetGMEMessage(UTCTime, MessageType, TencentGmeMessage.Code, TencentGmeMessage.Message1, TencentGmeMessage.Message2);
	if (ret == 0)
	{
		TencentGmeMessage.LocalUTCTime = FDateTime::FromUnixTimestamp(UTCTime);
		TencentGmeMessage.MessageType = (UTencentGME_MessageType)MessageType;
	}
	return ret;
}

int UTencentGMEBlueprintLibrary::SetServerAudioRoute(UTencentGME_AudioRouteMode sendType, const FString& sendList, UTencentGME_AudioRouteMode recvType, const FString& recvList)
{
	GMEWWisePlugin_AudioRouteMode gmesdkSendType = (GMEWWisePlugin_AudioRouteMode)sendType;
	GMEWWisePlugin_AudioRouteMode gmesdkRecvType = (GMEWWisePlugin_AudioRouteMode)recvType;
	
	return FTencentGMEDevice::SetServerAudioRoute(gmesdkSendType, sendList, gmesdkRecvType, recvList);
}
	
int UTencentGMEBlueprintLibrary::GetServerAudioRoute(UTencentGME_AudioRouteMode& sendType, FString& sendListBuf, UTencentGME_AudioRouteMode& recvType, FString& recvListBuf)
{
	GMEWWisePlugin_AudioRouteMode& gmesdkSendType = (GMEWWisePlugin_AudioRouteMode&)sendType;
	GMEWWisePlugin_AudioRouteMode& gmesdkRecvType = (GMEWWisePlugin_AudioRouteMode&)recvType;
	
	return FTencentGMEDevice::GetServerAudioRoute(gmesdkSendType, sendListBuf, gmesdkRecvType, recvListBuf);
}
int UTencentGMEBlueprintLibrary::GetMicCount()
{
	return FTencentGMEDevice::GetMicCount();
}

TArray<FTencentGME_DevInfo> UTencentGMEBlueprintLibrary::GetMicList(int count)
{
	DEV_INFO*  micListRaw = new DEV_INFO[count];
	FTencentGMEDevice::GetMicList(micListRaw, count);

	TArray<FTencentGME_DevInfo> micList;
	for(int i = 0; i < count; i++)
	{
		FTencentGME_DevInfo info;
		info.strDevNameUTF8 = FString(micListRaw[i].strDevNameUTF8);
		info.strDevIdUTF8 = FString(micListRaw[i].strDevIdUTF8);
		micList.Add(info);
	}
	delete[] micListRaw;
	return micList;
}

void UTencentGMEBlueprintLibrary::SelectMicDevice(int micId)
{
	FTencentGMEDevice::SelectMicDevice(micId);
}
FTencentGME_DevInfo UTencentGMEBlueprintLibrary::GetCurrentMicDeviceInfo(){
	DEV_INFO info;
	FTencentGMEDevice::GetCurrentMicDeviceInfo(&info);
	FTencentGME_DevInfo ret;
	ret.strDevNameUTF8 = FString(info.strDevNameUTF8);
	ret.strDevIdUTF8 = FString(info.strDevIdUTF8);
	return ret;
}
