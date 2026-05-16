#include "EventManagerSubsystem.h"

bool UEventManagerSubsystem::RemoveListener(FName EventKey, FDelegateHandle Handle)
{
	if (FOnEvent* Pipe = ListenerMap.Find(EventKey))
	{
		return Pipe->Remove(Handle);
	}
	return false;
}

void UEventManagerSubsystem::RemoveAllForObject(FName EventKey, const void* Obj)
{
	if (FOnEvent* Pipe = ListenerMap.Find(EventKey))
	{
		Pipe->RemoveAll(Obj);
	}
}

// BPs API
void UEventManagerSubsystem::AddListenerBP(FName EventKey, UObject* ListenerObject, FName FunctionName)
{
	if (!ListenerObject) return;

	FOnEventDyn& Pipe = ListenerMapBP.FindOrAdd(EventKey);

	FScriptDelegate ScriptDel;
	ScriptDel.BindUFunction(ListenerObject, FunctionName);
	Pipe.Add(ScriptDel);
}

void UEventManagerSubsystem::RemoveListenerBP(FName EventKey, UObject* ListenerObject, FName FunctionName)
{
	if (!ListenerObject) return;

	if (FOnEventDyn* Pipe = ListenerMapBP.Find(EventKey))
	{
		FScriptDelegate ScriptDel;
		ScriptDel.BindUFunction(ListenerObject, FunctionName);
		Pipe->Remove(ScriptDel);
	}
}

void UEventManagerSubsystem::RemoveAllForObjectBP(FName EventKey, UObject* ListenerObject)
{
	if (!ListenerObject) return;

	if (FOnEventDyn* Pipe = ListenerMapBP.Find(EventKey))
	{
		Pipe->RemoveAll(ListenerObject);
	}
}

void UEventManagerSubsystem::CallEventBP(FName EventKey)
{
	CallEvent(EventKey, FParamBlackboard::EMPTY);
}

void UEventManagerSubsystem::CallEventWithBlackboardBP(FName EventKey, const FParamBlackboard& Blackboard)
{
	CallEvent(EventKey, Blackboard);
}


// C++ API
void UEventManagerSubsystem::CallEvent(FName EventKey)
{
	CallEvent(EventKey, FParamBlackboard::EMPTY);
}

void UEventManagerSubsystem::CallEvent(FName EventKey, const FParamBlackboard& Blackboard)
{                 
	UE_LOG(LogTemp, Warning, TEXT("CallEvent: %s  BP:%d  C++:%d"),
		*EventKey.ToString(),
		ListenerMapBP.Contains(EventKey) ? 1 : 0,
		ListenerMap.Contains(EventKey) ? 1 : 0);

	if (FOnEvent* Pipe = ListenerMap.Find(EventKey))
	{
		Pipe->Broadcast(Blackboard);         
	}
	if (FOnEventDyn* PipeBP = ListenerMapBP.Find(EventKey))
	{
		PipeBP->Broadcast(Blackboard);        
	}
}