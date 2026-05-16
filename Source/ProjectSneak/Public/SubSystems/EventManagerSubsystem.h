#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UParamBlackboard.h"
#include "EventManagerSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEvent, const FParamBlackboard&)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventDyn, const FParamBlackboard&, Blackboard);


UCLASS()
class PROJECTSNEAK_API UEventManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	//Initialize and Deinitialize
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		Super::Initialize(Collection);
		bInitialized = true;
	}

	virtual void Deinitialize() override
	{
		bInitialized = false;
		Super::Deinitialize();
	}

	//C++ API
	template<typename UserClass>
		FDelegateHandle AddListener(FName EventKey, UserClass* Obj, void (UserClass::*Func)(const FParamBlackboard&))
	{
		FOnEvent& Pipe = ListenerMap.FindOrAdd(EventKey);
		return Pipe.AddUObject(Obj, Func);
	}

	template<typename Functor>
	FDelegateHandle AddListenerLambda(FName EventKey, Functor&& Fn)
	{
		FOnEvent& Pipe = ListenerMap.FindOrAdd(EventKey);
		return Pipe.AddLambda(Forward<Functor>(Fn));
	}

	bool RemoveListener(FName EventKey, FDelegateHandle Handle);
	void RemoveAllForObject(FName EventKey, const void* Obj);

	// Convenience overloads like your Unity sugar:
	template<typename T1>
	void CallEvent(FName EventKey, FName P1Key, const T1& P1Val)
	{
		FParamBlackboard BB; 
		BB.SetParam(P1Key, P1Val); 
		CallEvent(EventKey, BB);
	}

	template<typename T1, typename T2>
	void CallEvent(FName EventKey, FName P1Key, const T1& P1Val, FName P2Key, const T2& P2Val)
	{
		FParamBlackboard BB; 
		BB.SetParam(P1Key, P1Val); 
		BB.SetParam(P2Key, P2Val); 
		CallEvent(EventKey, BB);
	}

	template<typename T1, typename T2, typename T3>
	void CallEvent(FName EventKey, FName P1Key, const T1& P1Val, FName P2Key, const T2& P2Val, FName P3Key, const T3& P3Val)
	{
		FParamBlackboard BB; 
		BB.SetParam(P1Key, P1Val); 
		BB.SetParam(P2Key, P2Val); 
		BB.SetParam(P3Key, P3Val); 
		CallEvent(EventKey, BB);
	}

	////BP API
	UFUNCTION(BlueprintCallable, Category="Events")
	void AddListenerBP(FName EventKey, UObject* ListenerObject, FName FunctionName);
	
	UFUNCTION(BlueprintCallable, Category="Events")
	void RemoveListenerBP(FName EventKey, UObject* ListenerObject, FName FunctionName);
	
	UFUNCTION(BlueprintCallable, Category="Events")
	void RemoveAllForObjectBP(FName EventKey, UObject* ListenerObject);
	
	UFUNCTION(BlueprintCallable, Category="Events")
	void CallEventBP(FName EventKey);
	
	UFUNCTION(BlueprintCallable, Category="Events")
	void CallEventWithBlackboardBP(FName EventKey, const FParamBlackboard& Blackboard);
	
	//C++ Broadcasts
	void CallEvent(FName EventKey);
	void CallEvent(FName EventKey, const FParamBlackboard& Blackboard);

	
protected:
	// Called whenever enabled state changes (derived classes override)
	virtual void OnEnabledChanged(bool bNowEnabled) {}
	
	template<typename T>
	T* GetSystem() const
	{
		UWorld* World = GetWorld();
		return World ? World->GetSubsystem<T>() : nullptr;
	}

protected:
	bool bInitialized = false;

	TMap<FName, FOnEvent> ListenerMap;

	UPROPERTY()
	TMap<FName, FOnEventDyn> ListenerMapBP;
};