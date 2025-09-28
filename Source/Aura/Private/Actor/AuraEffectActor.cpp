// Copyright (c) 2024 Nicholas Solomon All Rights Reserved


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle =  TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == ApplyOnOverlap)
	{
		for (const auto InstantEffect : InstantGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, InstantEffect);
		}
	}
	if (DurationEffectApplicationPolicy == ApplyOnOverlap)
	{
		for (const auto DurationEffect : DurationGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, DurationEffect);
		}
	}
	if (InfiniteEffectApplicationPolicy == ApplyOnOverlap)
	{
		for (const auto InfiniteEffect : InfiniteGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, InfiniteEffect);
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == ApplyOnEndOverlap)
	{
		for (const auto InstantEffect : InstantGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, InstantEffect);
		}
	}
	if (DurationEffectApplicationPolicy == ApplyOnEndOverlap)
	{
		for (const auto DurationEffect : DurationGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, DurationEffect);
		}
	}
	if (InfiniteEffectApplicationPolicy == ApplyOnEndOverlap)
	{
		for (const auto InfiniteEffect : InfiniteGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, InfiniteEffect);
		}
	}
	if (InfiniteEffectRemovalPolicy == RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}


