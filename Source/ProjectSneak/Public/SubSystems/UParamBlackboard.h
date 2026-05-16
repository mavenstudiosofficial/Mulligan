#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UParamBlackboard.generated.h"

using FParamVariant = TVariant<int32, int64, float, FString, UObject*>;

USTRUCT(BlueprintType)
struct FParamBlackboard
{
	GENERATED_BODY()
	
	TMap<FName, FParamVariant> Params;

	template<typename T>
	void SetParam(FName Key, const T& Value)
	{
		FParamVariant V;
		V.template Set<T>(Value);   // picks the exact alternative T
		Params.Add(Key, MoveTemp(V));
	}
	
	template<typename T>
	bool TryGetParam(FName Key, T& OutValue) const
	{
		if (const FParamVariant* Found = Params.Find(Key))
		{
			if (const T* Ptr = Found->TryGet<T>()) { OutValue = *Ptr; return true; }
			UE_LOG(LogTemp, Error, TEXT("[ParamBB] Type mismatch for '%s'"), *Key.ToString());
			return false;
		}
		UE_LOG(LogTemp, Error, TEXT("[ParamBB] Missing key '%s'"), *Key.ToString());
		return false;
	}

	template<typename T>
	T GetParam(FName Key) const
	{
		T Out{};
		TryGetParam<T>(Key, Out);
		return Out;
	}

	static const FParamBlackboard EMPTY; // defined in .cpp as empty
};


/*
	UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
	class PROJECTSNEAK_API UParamBlackboard : public UActorComponent
	{
		GENERATED_BODY()

	private:
		TMap<FName, FParamVariant> Params;	
		
	public:
		template<typename T>
		void SetParam(FName Key, const T& Value)
		{
			Params.Add(Key, FParamVariant(Value));
		}

		template<typename T>
		bool GetParam(FName Key, T& OutValue)
		{
			const FParamVariant* found = Params.Find(Key);

			if (!found)
			{
				UE_LOG(LogTemp, Error, TEXT("[ParamBlackboard] Missing key '%s'"), *Key.ToString());
				return false;
			}

			if (const T* ptr = found->TryGet<T>())
			{
				OutValue = *ptr;
				return true;
			}

			UE_LOG(LogTemp, Error, TEXT("[ParamBlackboard] Type mismatch for key '%s'"), *Key.ToString());
			return false;
		}

		template<typename T>
		T GetParam(FName Key) const
		{
			T Out{};
			if (!TryGetParam<T>(Key, Out))
			{
				
			}
			return Out;
		}

		FString DebugDescribe(FName Key) const;

		bool RemoveParam(FName Key) { return Params.Remove(Key) > 0; }
		bool HasParam(FName Key) const { return Params.Contains(Key); }
	};
*/