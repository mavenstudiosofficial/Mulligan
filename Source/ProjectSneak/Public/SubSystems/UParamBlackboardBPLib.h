#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UParamBlackboard.h"
#include "UParamBlackboardBPLib.generated.h"

UCLASS()
class PROJECTSNEAK_API UParamBlackboardBPLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	// Getters
	UFUNCTION(BlueprintPure, Category="ParamBlackboard")
	static bool GetInt(const FParamBlackboard& BB, FName Key, int32& OutValue)
	{
		return BB.TryGetParam<int32>(Key, OutValue);
	}

	UFUNCTION(BlueprintPure, Category="ParamBlackboard")
	static bool GetInt64(const FParamBlackboard& BB, FName Key, int64& OutValue)
	{
		return BB.TryGetParam<int64>(Key, OutValue);
	}

	UFUNCTION(BlueprintPure, Category="ParamBlackboard")
	static bool GetFloat(const FParamBlackboard& BB, FName Key, float& OutValue)
	{
		return BB.TryGetParam<float>(Key, OutValue);
	}

	UFUNCTION(BlueprintPure, Category="ParamBlackboard")
	static bool GetString(const FParamBlackboard& BB, FName Key, FString& OutValue)
	{
		return BB.TryGetParam<FString>(Key, OutValue);
	}

	UFUNCTION(BlueprintPure, Category="ParamBlackboard")
	static bool GetObject(const FParamBlackboard& BB, FName Key, UObject*& OutValue)
	{
		return BB.TryGetParam<UObject*>(Key, OutValue);
	}

	// Setters
	UFUNCTION(BlueprintCallable, Category = "ParamBlackboard", meta = (AutoCreateRefTerm = "BB"))
	static void SetInt(UPARAM(ref) FParamBlackboard& BB, FName Key, int32 Value)
	{
		BB.SetParam<int32>(Key, Value);
	}

	UFUNCTION(BlueprintCallable, Category = "ParamBlackboard", meta = (AutoCreateRefTerm = "BB"))
	static void SetInt64(UPARAM(ref) FParamBlackboard& BB, FName Key, int64 Value)
	{
		BB.SetParam<int64>(Key, Value);
	}

	UFUNCTION(BlueprintCallable, Category = "ParamBlackboard", meta = (AutoCreateRefTerm = "BB"))
	static void SetFloat(UPARAM(ref) FParamBlackboard& BB, FName Key, float Value)
	{
		BB.SetParam<float>(Key, Value);
	}

	UFUNCTION(BlueprintCallable, Category = "ParamBlackboard", meta = (AutoCreateRefTerm = "BB"))
	static void SetString(UPARAM(ref) FParamBlackboard& BB, FName Key, const FString& Value)
	{
		BB.SetParam<FString>(Key, Value);
	}

	UFUNCTION(BlueprintCallable, Category = "ParamBlackboard", meta = (AutoCreateRefTerm = "BB"))
	static void SetObject(UPARAM(ref) FParamBlackboard& BB, FName Key, UObject* Value)
	{
		BB.SetParam<UObject*>(Key, Value);
	}

	//extra functions
	//UFUNCTION(BlueprintPure, Category = "ParamBlackboard")
	//static bool Has(const FParamBlackboard& BB, FName Key)
	//{
	//	return BB.HasParam(Key);
	//}

	//UFUNCTION(BlueprintCallable, Category = "ParamBlackboard")
	//static void Clear(UPARAM(ref) FParamBlackboard& BB)
	//{
	//	BB.Params.Empty();
	//}
	//UFUNCTION(BlueprintPure, Category = "ParamBlackboard", DisplayName = "Make Param Blackboard")
	//static FParamBlackboard MakeParamBlackboard()
	//{
	//	return FParamBlackboard{};
	//}
};