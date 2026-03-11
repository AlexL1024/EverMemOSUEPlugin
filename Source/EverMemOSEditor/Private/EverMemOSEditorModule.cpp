// Copyright TonyL. All Rights Reserved.

#include "EverMemOSEditorModule.h"
#include "EverMemOS/EverMemOSLog.h"
#include "EverMemOS/EverMemOSSettings.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "FEverMemOSEditorModule"

void FEverMemOSEditorModule::StartupModule()
{
	UE_LOG(LogEverMemOS, Log, TEXT("EverMemOS Editor module started"));

	// Clear transient credentials when leaving PIE to avoid leaving secrets visible in the editor UI.
	EndPIEHandle = FEditorDelegates::EndPIE.AddRaw(this, &FEverMemOSEditorModule::HandleEndPIE);
}

void FEverMemOSEditorModule::ShutdownModule()
{
	if (EndPIEHandle.IsValid())
	{
		FEditorDelegates::EndPIE.Remove(EndPIEHandle);
		EndPIEHandle.Reset();
	}

	UE_LOG(LogEverMemOS, Log, TEXT("EverMemOS Editor module shutdown"));
}

void FEverMemOSEditorModule::HandleEndPIE(bool bIsSimulating)
{
	if (UEverMemOSSettings* Settings = GetMutableDefault<UEverMemOSSettings>())
	{
		Settings->SetBearerToken(TEXT(""));
		Settings->SetHMACSecret(TEXT(""));
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEverMemOSEditorModule, EverMemOSEditor)
