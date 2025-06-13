// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
#define EI_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("%s ") Format, LOG_CALLINFO, ##__VA_ARGS__)

DECLARE_LOG_CATEGORY_EXTERN(LogEI, Log, All);