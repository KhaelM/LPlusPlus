#pragma once
#include "cpplinq.hpp"
#include "PluginBase.hpp"
#include "UnitTree.hpp"
#include <queue>

#define BASE_ATTACKSPEED 0.67f
#define Q_BASE_DELAY .4f
#define Q_EMPOWERED_BASE_DELAY .35f

using namespace std;
using namespace Utils;

class YasuoPlus : public PluginBase
{
public:
	ISpell2* Q;
	ISpell2* QEmpowerd;
	ISpell2* QCircle;
	ISpell2* W;
	ISpell2* E;
	ISpell2* R;

	UnitDash* lastDash;
	vector<IUnit*> InRange;
	UnitTree UnitTree;
	
	void OnLoad() override
	{
		Log("Loaded!");
		lastDash = 0;
		this->LoadSpells();
	}

	void OnUnLoad() override
	{
		UnitTree.Clear();
	}

	void OnRender() override
	{
		GRender->DrawOutlinedCircle(Player()->GetPosition(), Vec4(255, 255, 0, 255), 475);
		//return;
		for (auto unit : InRange)
		{
			if(unit != NULL)
				GRender->DrawOutlinedCircle(unit->GetPosition(), Vec4(255, 255, 0, 255), 70);
		}
	}

	void OnGameUpdate() override
	{
		UpdateChampionData();
		UnitTree.Clear();
		auto mins = GEntityList->GetAllUnits();
		random_shuffle(mins.begin(), mins.end());
		UnitTree.Insert(Player());
		for (auto unit : mins)
		{
			if(unit == NULL || unit == Player())
				continue;
			UnitTree.Insert(unit);
		}

		InRange.clear();
		UnitTree.FindInRange(Player()->GetPosition().To2D(), 475, InRange);

		IUnit* target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, (E->IsReady() ? 1500 : 475));

		if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		{
			//cout << "inserted!: " << endl;

			cout << "X " << Player()->GetPosition().x << " Y " << Player()->GetPosition().z << endl;

			//cout << "Q Speed: " << GetQSpeed() << endl;
			DoCombo(target);
		}

		UseRSmart();
	}

	void OnDash(UnitDash* dashInfo) override
	{
		if (dashInfo->Source != Player())
			return;
		lastDash = dashInfo;
	}

protected:
	void DrawMenu() override
	{
	}

private:

	void DoCombo(IUnit* target)
	{
		if (target == 0) {
			GapCloseE(GGame->CursorPosition().To2D());
			return;
		}
		set<IUnit*> ignores;
		ignores.insert(target);
		if (!GapCloseE(target->GetPosition().To2D(), &ignores))
		{
			if (UseESmart(target))
				return;
		}
		else
		{
			return;
		}

		UseQSmart(target);
	}

	void Harass()
	{

	}


#pragma region yasuoLogic

	typedef pair<float, Vec2> VecPair;

	IUnit* BestEJump(Vec2 target)
	{
		auto playerPos = Player()->GetPosition().To2D();

		float jumpCost = 0.6f - (((float)Player()->GetSpellBook()->GetLevel(kSlotE)) / 10.0f) + 0.2f;
		float fastestTime = (playerPos - target).Length() / Player()->MovementSpeed();
		IUnit* best = NULL;
		vector<IUnit*> around;
		set<IUnit*> *path = new set<IUnit*>();
		UnitTree.FindInRange(playerPos, E->Range(), around);
		for (auto u : around)
		{
			if(!EnemyIsJumpable(u))
				continue;
			auto time = GetCost(u, playerPos, target, jumpCost, jumpCost, path);
			if(time<fastestTime)
			{
				fastestTime = time;
				best = u;
			}
		}
		delete path;
		return best;
	}


	float GetCost(IUnit* jump, Vec2 from, Vec2 target, float jumpCost, float totalCost, set<IUnit*> *path)
	{
		//Doesnt contain
		if (path->count(jump) == 0)
		{
			path->insert(jump);
			auto pos = jump->GetPosition().To2D();
			//Prediction if first check
			if (path->size() == 1)
			{
				Vec3 pred;
				GPrediction->GetFutureUnitPosition(jump, E->GetDelay(), true, pred);
				pos = pred.To2D();
			}
			Vec2 posAfterJump = from.Extend(pos, E->Range());

			vector<IUnit*> around;
			UnitTree.FindInRange(posAfterJump, E->Range(), around);
			float fastestTime = (posAfterJump - target).Length()/Player()->MovementSpeed();

			for(auto u : around)
			{
				if (!EnemyIsJumpable(u))
					continue;
				auto time = GetCost(u, posAfterJump, target, jumpCost, totalCost + jumpCost, path);
				if (time < fastestTime)
					fastestTime = time;
			}
			path->erase(jump);
			return fastestTime;
		}
		return totalCost + (from - target).Length() / Player()->MovementSpeed();
	}


	void GetJumpablesAround(Vec2 orgin, float radius, vector<IUnit*> result)
	{
		vector<IUnit*> temp;
		UnitTree.FindInRange(orgin, radius, temp);
		for(auto unit : temp)
		{
			if (EnemyIsJumpable(unit))
				result.push_back(unit);
		}
	}

	void UseQSmart(IUnit* target, bool onlyEmpowered = false)
	{
		if (target == 0 || !Q->IsReady())
			return;

		if (!IsDashing())
		{
			if (IsQEmpovered())
				CastQEmpowered(target);
			else if (!onlyEmpowered)
				CastQ(target);
		}
		else
		{
			CastQCircle(target);
		}
	}

	bool GapCloseE(Vec2 position, set<IUnit*> *ignore = NULL)
	{
		if (!E->IsReady())
			return false;

		IUnit* bestJumpUnit = NULL;
		float bestRange = Distance(Player(), position);

		//cout << " bestDist: " << bestRange << endl;
		//Remnove ignores
		//if(ignore != NULL)
		//	for(IUnit* ign : *ignore)
		//		UnitTree.Remove(ign);

		bestJumpUnit = BestEJump(position);

		//Restre tree
		//if (ignore != NULL)
		//	for (IUnit* ign : *ignore)
		//		UnitTree.Insert(ign);

		if (bestJumpUnit != NULL)
		{
			CastE(bestJumpUnit);
			return true;
		}
		return false;
	}

	bool UseESmart(IUnit* target)
	{
		if (!E->IsReady())
			return false;
		float trueAARange = Player()->AttackRange() + target->BoundingRadius();
		Vec3 predictionPos;
		GPrediction->GetFutureUnitPosition(target, E->GetDelay(), true, predictionPos);
		float dist = Distance(Player(), predictionPos);
		Vec2 posAfterJump = Player()->GetPosition().To2D().Extend(predictionPos.To2D(), E->Range());
		float distAfter = (predictionPos.To2D() - posAfterJump).Length();
		if (dist < distAfter || target->IsMoving())
			return false;

		float movespeedDelta = (Player()->MovementSpeed() - target->MovementSpeed());
		if (movespeedDelta == 0)
			movespeedDelta = 0.001f;

		float timeToReach = (dist - trueAARange) / movespeedDelta;
		if (dist > trueAARange && dist < E->Range())
		{
			if (timeToReach > 1.7f || timeToReach < 0.0f)
			{
				if (CastE(target))
					return true;
			}
		}

		return false;
	}

	bool UseRSmart()
	{
		float timeToLand = FLT_MAX;
		auto airborns = GetKnockedUpEnemies(timeToLand);
		if (timeToLand > 0.4f)
			return false;
		for (auto ene : airborns)
		{
			int aroundAir = 0;
			for (auto ene2 : airborns)
			{
				if (Distance(ene, ene2) < 400)
					aroundAir++;
			}
			if (aroundAir >= 1 || ene->HealthPercent() < 35) {
				if (R->CastOnPosition(ene->GetPosition()))
					return true;
			}
		}
		return false;
	}

	bool EnemyIsJumpable(IUnit* enemy)
	{
		if (enemy->UnitFlags() != FL_HERO && enemy->UnitFlags() != FL_CREEP)
			return false;

		if (!enemy->IsValidTarget() || !enemy->IsEnemy(Player()) || enemy->IsInvulnerable() || enemy->IsDead())
			return false;

		return !enemy->HasBuff("YasuoDashWrapper");
	}

	set<IUnit*> GetKnockedUpEnemies(float &leastKockUpTime)
	{
		set<IUnit*> knockedUp;
		for (auto hero : GEntityList->GetAllHeros(false, true))
		{
			if (hero->IsDead() || hero->IsInvulnerable())
				continue;
			for (int i = 0; i < hero->GetNumberOfBuffs(); i++)
			{
				auto buffPtr = hero->GetBuffByIndex(i);
				auto buffType = GBuffData->GetBuffType(buffPtr);
				if (buffType != BUFF_Knockback && buffType != BUFF_Knockup)
					continue;

				float timeDelta = GBuffData->GetEndTime(buffPtr) - GGame->Time();
				if (leastKockUpTime > timeDelta)
					leastKockUpTime = timeDelta;

				knockedUp.insert(hero);
			}
		}
		return knockedUp;
	}

#pragma endregion yasuoLogic

#pragma region spellsExecute

	bool CastQ(IUnit* target)
	{
		return Q->CastOnTarget(target);
	}

	bool CastQEmpowered(IUnit* target)
	{
		if (!IsQEmpovered())
			return false;
		return QEmpowerd->CastOnTarget(target, 3);
	}

	bool CastQCircle(IUnit* target)
	{
		if (!IsDashing())
			return false;
		cout << "Qempow cirtlce" << endl;

		if (Distance(target, lastDash->EndPosition) < 50 && Distance(target, lastDash->EndPosition.To2D()) < QCircle->Radius())
			return Q->CastOnTarget(target);
		return false;
	}

	bool CastE(IUnit* target)
	{
		if (!E->IsReady() || Distance(target, Player()) > E->Range())
			return false;
		//For later!
		Vec2 posAfterJump = Player()->GetPosition().To2D().Extend(target->GetPosition().To2D(), E->Range());

		return E->CastOnUnit(target);
	}

#pragma endregion spellsExecute  

#pragma region spellsLoad  
	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, true, kCollidesWithNothing);
		Q->SetSkillshot(GetQSpeed(), 50.f, FLT_MAX, 475.f);

		QEmpowerd = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, true, true, kCollidesWithNothing);
		QEmpowerd->SetSkillshot(GetQEmpowerdSpeed(), 50.f, 1200.f, 1000.f);

		QCircle = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, true, kCollidesWithNothing);
		QCircle->SetSkillshot(GetQSpeed(), 325.f, FLT_MAX, 0.f);

		W = GPluginSDK->CreateSpell2(kSlotW, kLineCast, false, true, kCollidesWithMinions);
		W->SetOverrideRange(400.f);

		E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, false, false, kCollidesWithNothing);
		E->SetOverrideRange(475.f);

		R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, false, false, kCollidesWithMinions);
		W->SetOverrideRange(1200.f);
	}

	void UpdateChampionData() {
		Q->SetOverrideDelay(GetQSpeed());
		QEmpowerd->SetOverrideDelay(GetQEmpowerdSpeed());
		QCircle->SetOverrideDelay(GetQSpeed());
	}

	float GetQSpeed()
	{
		float atackSpeedPerc = (Player()->AttackSpeed() - 1) * 100;
		float reduction = atackSpeedPerc / 1.6725;
		if (reduction > 66)
			reduction = 66;

		return (Q_BASE_DELAY*(100 - reduction)) / 100;
	}

	float GetQEmpowerdSpeed()
	{
		float atackSpeedPerc = (Player()->AttackSpeed() - 1) * 100;
		float reduction = atackSpeedPerc / 1.6725;
		if (reduction > 66)
			reduction = 66;

		return (Q_EMPOWERED_BASE_DELAY*(100 - reduction)) / 100;
	}
#pragma endregion  spellsLoad 


#pragma region yasuoInfo  

	bool IsQEmpovered()
	{
		return Player()->HasBuff("yasuoq3w");
	}

	bool IsDashing()
	{
		return lastDash != 0 && lastDash->EndTick + 300 > GameTicks();
	}

#pragma endregion yasuoInfo  

};