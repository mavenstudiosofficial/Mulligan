#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TencentGMEBlueprintLibrary.generated.h"

UENUM(BlueprintType)
enum class UTencentGME_TeamMode : uint8
{
	TEAMMODE_GLOBAL = 0 UMETA(DisplayName = "Global"),
	TEAMMODE_TEAM = 1 UMETA(DisplayName = "Team")
};

UENUM(BlueprintType)
enum class UTencentGME_Region : uint8
{
	REGION_DEFAULT = 0 UMETA(DisplayName = "Defalut"),
	REGION_INTERNATIONAL = 1 UMETA(DisplayName = "International"),
	REGION_AWS = 2 UMETA(DisplayName = "AWS")
};

UENUM(BlueprintType)
enum class UTencentGME_StreamProfile : uint8
{
	STREAMPROFILE_NA = 0 UMETA(DisplayName = "NA"),
	STREAMPROFILE_LOW_LATENCY = 1 UMETA(DisplayName = "Low Latency"),
	STREAMPROFILE_STANDARD = 2 UMETA(DisplayName = "Standard"),
	STREAMPROFILE_HD = 3 UMETA(DisplayName = "HD")
};

UENUM(BlueprintType)
enum class UTencentGME_LogLevel : uint8
{
	GMESDK_LOGLEVEL_NONE = 0 UMETA(DisplayName = "None"),
	GMESDK_LOGLEVEL_ERROR = 1 UMETA(DisplayName = "Error"),
	GMESDK_LOGLEVEL_INFO = 2 UMETA(DisplayName = "Info"),
	GMESDK_LOGLEVEL_DEBUG = 3 UMETA(DisplayName = "Debug"),
	GMESDK_LOGLEVEL_VERBOSE = 4 UMETA(DisplayName = "Verbose")
};

UENUM(BlueprintType)
enum class UTencentGME_MessageType : uint8
{
	Invalid = 0 UMETA(DisplayName = "Invalid"),
	RoomStatus = 101 UMETA(DisplayName = "Room Status"),
	RoomMemberIn = 102 UMETA(DisplayName = "Member In"),
	RoomMemberOut = 103 UMETA(DisplayName = "Member Out"),
	RecordComplete = 104 UMETA(DisplayName = "Record Complete"),
	PlayoutComplete = 105 UMETA(DisplayName = "Playout Complete"),
	ASRComplete = 106 UMETA(DisplayName = "Speech Recognition Complete"),
	GetVoiceDurationComplete = 107 UMETA(DisplayName = "Speech Duration Complete"),
	StreamingASRIntermediate = 108 UMETA(DisplayName = "Speech Recognition Intermediate")
};

UENUM(BlueprintType)
enum class UTencentGME_AudioRouteMode : uint8
{
	GMESDK_AudioRouteMode_NA = 0 UMETA(DisplayName = "NA"),
	GMESDK_AudioRouteMode_NONE = 1 UMETA(DisplayName = "None"),
	GMESDK_AudioRouteMode_ALL = 2 UMETA(DisplayName = "All"),
	GMESDK_AudioRouteMode_BLOCKLIST = 3 UMETA(DisplayName = "BlockList"),
	GMESDK_AudioRouteMode_ALLOWLIST = 4 UMETA(DisplayName = "AllowList")
};

USTRUCT(BlueprintType)
struct FTencentGME_DevInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Device Info")
    FString strDevNameUTF8;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Device Info")
    FString strDevIdUTF8;
};

USTRUCT(BlueprintType)
struct FTencentGmeMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Tencent GME", BlueprintReadOnly)
	FDateTime LocalUTCTime;

	UPROPERTY(EditAnywhere, Category = "Tencent GME", BlueprintReadOnly)
	UTencentGME_MessageType MessageType;

	UPROPERTY(EditAnywhere, Category = "Tencent GME", BlueprintReadOnly)
	int32 Code;

	UPROPERTY(EditAnywhere, Category = "Tencent GME", BlueprintReadOnly)
	FString Message1;

	UPROPERTY(EditAnywhere, Category = "Tencent GME", BlueprintReadOnly)
	FString Message2;
};


UCLASS()
class TENCENTGME_WWISE_API UTencentGMEBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static FString GetGMEVersion();

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int SetUserID(const FString& userID);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int SetRoomID(const FString& roomID);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void ReceivePlugin_SetReceiveOpenIDWithAkComponent(class UAkComponent* AkComponent, const FString& userID);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void ReceivePlugin_GetReceiveOpenIDWithAkComponent(class UAkComponent* AkComponent, FString& userID);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SendPlugin_EnableLoopbackWithAkComponent(class UAkComponent* AkComponent, bool enableLoopback);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static bool SendPlugin_GetEnableLoopbackWithAkComponent(class UAkComponent* AkComponent);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void ReceivePlugin_SetReceiveOpenIDWithActor(class AActor* Actor, const FString& userID);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void ReceivePlugin_GetReceiveOpenIDWithActor(class AActor* Actor, FString& userID);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SendPlugin_EnableLoopbackWithActor(class AActor* Actor, bool enableLoopback);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static bool SendPlugin_GetEnableLoopbackWithActor(class AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SetRegion(UTencentGME_Region region);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SetAudioStreamProfile(UTencentGME_StreamProfile profile);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SetLogLevel(UTencentGME_LogLevel logLevelWrite, UTencentGME_LogLevel logLevelPrint);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void GMEPause();
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void GMEResume();
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int GetAudioSendStreamLevel();
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int GetAudioRecvStreamLevel(const FString& targetID);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void AddAudioBlockList(const FString& targetID);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void RemoveAudioBlockList(const FString& targetID);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void EnableBluetoothMic(bool enable);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SetRangeAudioTeamID(int teamID);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SetRangeAudioTeamMode(UTencentGME_TeamMode teamMode);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SetRangeAudioRecvRange(int range);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SetSelfPosition(int positionX, int positionY, int positionZ);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int StartRecordingWithRecognition(const FString& speechLanguage);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int StartRecording();
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int StopRecording();
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int PlayRecordFile(const FString& fileid);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int StopPlayFile();
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int SpeechToText(const FString& fileid, const FString& speechLanguage, const FString& translateLanguage);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int GetVoiceFileDuration(const FString& fileid);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int GetGMEMessage(FTencentGmeMessage& TencentGmeMessage);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int SetServerAudioRoute(UTencentGME_AudioRouteMode sendType, const FString& sendList, UTencentGME_AudioRouteMode recvType, const FString& recvList);
	
	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int GetServerAudioRoute(UTencentGME_AudioRouteMode& sendType, FString& sendListBuf, UTencentGME_AudioRouteMode& recvType, FString& recvListBuf);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static int GetMicCount();

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static TArray<FTencentGME_DevInfo> GetMicList(int count);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static void SelectMicDevice(int micId);

	UFUNCTION(BlueprintCallable, Category = "TencentGME")
	static FTencentGME_DevInfo GetCurrentMicDeviceInfo();
	
private:
	static class UAkComponent* GetAkComponentFromActor(class AActor* Actor);
};
