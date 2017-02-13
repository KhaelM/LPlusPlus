#pragma once
#include "PluginSDK.h"
#include "Utils.hpp"
class PluginBase
{
public:
	virtual void OnLoad() = 0;
	virtual void OnUnLoad() = 0;
	virtual void OnRender() = 0;
	virtual void OnGameUpdate() = 0;
	virtual void AfterAttack(IUnit* Source, IUnit* Target) {};
	virtual void OnGapCloser(GapCloserSpell const& Args) {};
	virtual void OnDash(UnitDash* Args) {};
	int TickOffset;

	IUnit* Player() { return GEntityList->Player(); }

	int GameTicks() {
		return (int)(GGame->Time() * 1000) + TickOffset;
	}

	float Distance(IUnit* from, IUnit* to)
	{
		Vec3 fromPos = from->GetPosition();
		Vec3 toPos = to->GetPosition();
		return (fromPos.To2D() - toPos.To2D()).Length();
	}

	float Distance(IUnit* from, Vec3 toPos)
	{
		Vec3 fromPos = from->GetPosition();
		return (fromPos.To2D() - toPos.To2D()).Length();
	}

	float Distance(IUnit* from, Vec2 toPos)
	{
		Vec3 fromPos = from->GetPosition();
		return (fromPos.To2D() - toPos).Length();
	}

	float Distance(Vec2 from, Vec2 toPos)
	{
		return (from - toPos).Length();
	}

	float Distance(Vec3 from, Vec3 toPos)
	{
		return (from.To2D() - toPos.To2D()).Length();
	}

	bool IsFacing(IUnit* source, IUnit* target)
	{
		if ( target == NULL)
		{
			return false;
		}

		return IsFacing(source, target->GetPosition().To2D());
	}

	bool IsFacing(IUnit* source, Vec2 target)
	{
		if (source == NULL)
		{
			return false;
		}

		const float angle = 90;
		return source->Direction().To2D().AngleBetween((target - source->GetPosition().To2D()))
			< angle;
	}

};
