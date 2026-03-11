// Copyright TonyL. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEverMemOSEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void HandleEndPIE(bool bIsSimulating);
	FDelegateHandle EndPIEHandle;
};
