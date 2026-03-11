// Copyright TonyL. All Rights Reserved.

#include "EverMemOS/EverMemOSModule.h"
#include "EverMemOS/EverMemOSLog.h"

#define LOCTEXT_NAMESPACE "FEverMemOSRuntimeModule"

void FEverMemOSRuntimeModule::StartupModule()
{
	UE_LOG(LogEverMemOS, Log, TEXT("EverMemOS Runtime module started"));
}

void FEverMemOSRuntimeModule::ShutdownModule()
{
	UE_LOG(LogEverMemOS, Log, TEXT("EverMemOS Runtime module shutdown"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEverMemOSRuntimeModule, EverMemOSRuntime)
