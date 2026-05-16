// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/StringLibrary.h"

bool UStringLibrary::SGreaterThan(FString StringA, FString StringB)
{
	return (FCString::Strcmp(*StringA, *StringB) > 0);
}


bool UStringLibrary::SLessThan(FString StringA, FString StringB)
{
	return (FCString::Strcmp(*StringA, *StringB) < 0);
}