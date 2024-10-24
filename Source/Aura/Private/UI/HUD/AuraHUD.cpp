// Copyright (c) 2024 Nicholas Solomon All Rights Reserved


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"

void AAuraHUD::BeginPlay()
{
	Super::BeginPlay();

	if (OverlayWidgetClass)
	{
		OverlayWidget = CreateWidget<UAuraUserWidget>(GetOwningPlayerController(), OverlayWidgetClass);
	}
	if (OverlayWidget)
	{
		OverlayWidget->AddToViewport();
	}
	
}
