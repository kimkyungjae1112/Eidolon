// Fill out your copyright notice in the Description page of Project Settings.


#include "EIGameplayTags.h"

namespace EIGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Attacking, "Character.State.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Rolling, "Character.State.Rolling");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_GeneralAction, "Character.State.GeneralAction");

	UE_DEFINE_GAMEPLAY_TAG(Character_Action_Equip, "Character.Action.Equip");
	UE_DEFINE_GAMEPLAY_TAG(Character_Action_Unequip, "Character.Action.Unequip");
}