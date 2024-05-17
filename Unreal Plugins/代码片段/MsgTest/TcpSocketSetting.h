// SpartanTools 2019

#pragma once

#include "TcpSocketSetting.generated.h"

UCLASS(config = Engine, defaultconfig)
class NIC_API UTcpSocketSetting : public UObject
{
	GENERATED_BODY()
	
public:
	/** Post errors to message log. */
	UPROPERTY(Config, EditAnywhere, Category = "TcpSocketPlugin")
	bool bPostErrorsToMessageLog;	
};
