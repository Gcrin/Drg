// Fill out your copyright notice in the Description page of Project Settings.

#include "DrgGameplayTags.h"

namespace DrgGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "캐릭터가 죽은 상태를 나타냅니다.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Death, "Event.Death",
	                               "죽음 이벤트를 브로드캐스트합니다. 이 태그를 수신 대기하는 시스템(주로 Gameplay Ability)이 죽음 관련 로직을 실행합니다.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Multiplier, "Ability.Multiplier", "어빌리티 배율입니다.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Team, "Team", "모든 팀 태그의 부모 태그입니다.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Team_Player, "Team.Player", "현재 캐릭터의 팀 태그는 플레이어입니다.");
}
