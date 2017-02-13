#pragma once
#include "PluginBase.hpp"
#include "UnitTree.hpp"
#include <queue>
#include <ctime>
#include "Menu.hpp"

#define BASE_ATTACKSPEED 0.67f
#define Q_BASE_DELAY .4f
#define Q_EMPOWERED_BASE_DELAY .35f

using namespace std;
using namespace Utils;

/*
 * TODO LIST:
 * 1) LaneClear use E (60%)
 * 2) Menu
 * 
 * n) Wall monster dash
 *
 */

class YasuoPlus : public PluginBase
{
public:
	ISpell2* Q;
	ISpell2* QEmpowerd;
	ISpell2* QCircle;
	ISpell2* W;
	ISpell2* E;
	ISpell2* R;

	UnitDash LastDash;
	UnitTree UnitTree;
	Menu* YasuoMenu;
	
	void OnLoad() override
	{
		Log("Loaded!");
		ZeroMemory(&LastDash, sizeof(UnitDash));
		this->LoadSpells();
		YasuoMenu = new Menu();
	}

	void OnUnLoad() override
	{
		UnitTree.Clear();
		delete YasuoMenu;
	}

	void OnRender() override
	{
		if(YasuoMenu->DrawingQ->Enabled())
		{
			if(IsQEmpovered())
				GRender->DrawOutlinedCircle(Player()->GetPosition(), Vec4(224, 37, 8, 180), QEmpowerd->Range());
			else
				GRender->DrawOutlinedCircle(Player()->GetPosition(), Vec4(224, 37, 8, 180), Q->Range());
		}

		if (YasuoMenu->DrawingE->Enabled())
				GRender->DrawOutlinedCircle(Player()->GetPosition(), Vec4(50, 255,248, 180), E->Range());

		if (YasuoMenu->DrawingR->Enabled())
			GRender->DrawOutlinedCircle(Player()->GetPosition(), Vec4(207, 50, 255, 180), R->Range());

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

		if (YasuoMenu->ComboRUse->Enabled())
			UseRSmart();
		
		IUnit* target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, (E->IsReady() ? 
			1500 : (IsQEmpovered())?QEmpowerd->Range():Q->Range()));

		if (YasuoMenu->HarassQ)
			Harass(target);

		if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		{
			DoCombo(target);
		}

		if(GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		{
			LaneClear();
		}

		if(GOrbwalking->GetOrbwalkingMode() == kModeLastHit)
		{
			LastHit();
		}
		if (GetAsyncKeyState(YasuoMenu->FleeFleeKey->GetInteger()) & 0x8000)
			Flee(GGame->CursorPosition());

	}

	void OnDash(UnitDash* dashInfo) override
	{
		if (dashInfo->Source != Player())
			return;
		cout << "OnDash" << endl;
		memcpy(&LastDash, dashInfo, sizeof(UnitDash));
		TickOffset = LastDash.StartTick - (int)(GGame->Time() * 1000);
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

	void LastHit()
	{

		if (Q->IsReady())
		{
			vector<IUnit*> aroundQ;
			GetAround(Player()->GetPosition().To2D(), Q->Range(), aroundQ,
				[&](IUnit* unit) -> bool
			{
				return HasCreepFlags(unit) && unit->IsEnemy(Player());
			});
			for (auto unit : aroundQ)
			{
				if (getQDamage(unit) >= unit->GetHealth())
					if (CastQ(unit))
						return;
			}
		}

		if (E->IsReady())
		{
			vector<IUnit*> aroundE;
			GetAround(Player()->GetPosition().To2D(), E->Range(), aroundE,
				[&](IUnit* unit) -> bool
			{
				return HasCreepFlags(unit) && unit->IsEnemy(Player());
			});
			for (auto unit : aroundE)
			{
				if (getEDamage(unit) >= unit->GetHealth())
					if (CastE(unit))
						return;
			}
		}

	}

	void LaneClear()
	{
		//If Active dash check if woth to Q
		if(YasuoMenu->LaneClearQ->Enabled() && IsDashing() && Q->IsReady() && Distance(Player(), LastDash.EndPosition) < 80)
		{
			vector<IUnit*> aroundQCircle;
			GetAround(LastDash.EndPosition.To2D(),QCircle->Range(), aroundQCircle,
				[&](IUnit* unit) -> bool 
				{
					return HasCreepFlags(unit) && !unit->IsDead() && unit->IsEnemy(Player());
				}
			);
			if (aroundQCircle.size() > 1)
			{
				if(QCircle->CastOnPlayer())
					return;
			}
		}
		if (E->IsReady() && YasuoMenu->LaneClearE->Enabled())
		{
			vector<IUnit*> aroundE;
			GetAround(Player()->GetPosition().To2D(), E->Range(), aroundE,
				[&](IUnit* unit) -> bool
			{
				return HasCreepFlags(unit) && unit->IsEnemy(Player());
			});
			for (auto unit : aroundE)
			{
				Vec2 posAfterJump = Player()->GetPosition().To2D().Extend(unit->GetPosition().To2D(), E->Range());
				vector<IUnit*> aroundQCircle;
				GetAround(posAfterJump, QCircle->Range(), aroundQCircle,
					[&](IUnit* unit) -> bool
				{
					return HasCreepFlags(unit) && !unit->IsDead() && unit->IsEnemy(Player());
				});
				if ((Q->IsReady() && aroundQCircle.size() > 2) || getEDamage(unit) >= unit->GetHealth())
				{
					if (CastE(unit))
					{
						return;
					}
				}
			}
		}

		if(!IsDashing() && Q->IsReady() && YasuoMenu->LaneClearQ->Enabled())
		{
			if(IsQEmpovered())
			{
				Vec3 pos;
				int count;
				GPrediction->FindBestCastPosition(QEmpowerd->Range(), QEmpowerd->Radius(), true, true, true, pos, count);
				if (count > 3)
					QEmpowerd->CastOnPosition(pos);
			}
			else
			{
				Vec3 pos;
				int count;
				GPrediction->FindBestCastPosition(Q->Range(), Q->Radius(), false, true, true, pos, count);
				if (count > 0)
					Q->CastOnPosition(pos);
			}
		}

	}

	void Harass(IUnit* target)
	{
		if(target == NULL)
			return;

		vector<IUnit*> enemyTowers;
		GetAround(Player()->GetPosition().To2D(), 950, enemyTowers, [&](IUnit* unit) -> bool
		{
			return unit->UnitFlags() == FL_TURRET && !unit->IsDead() && unit->IsEnemy(Player());
		});
		if(enemyTowers.size() == 0 || !YasuoMenu->HarassUnderTower->Enabled())
			UseQSmart(target);
	}

	void Flee(Vec3 position)
	{
		GapCloseE(position.To2D());
		GGame->IssueOrder(Player(), kMoveTo, position);
		if (!YasuoMenu->FleeFleeStack->Enabled() || IsQEmpovered())
			return;
		auto spell = (IsDashing()) ? QCircle : Q;
		auto pos = (IsDashing()) ? LastDash.EndPosition.To2D() : Player()->GetPosition().To2D();
		vector<IUnit*> qTargets;
		GetAround(pos, spell->Range(), qTargets,
			[&](IUnit* unit) -> bool
		{
			return HasCreepFlags(unit) && !unit->IsDead() && unit->IsEnemy(Player());
		});
		if (qTargets.size() > 0)
			spell->CastOnTarget(qTargets[0], kHitChanceMedium);

	}


#pragma region yasuoLogic
	
	IUnit* BestEJump(Vec2 target, set<IUnit*> *ignore = NULL)
	{
		auto playerPos = Player()->GetPosition().To2D();

		float jumpCost = 0.6f - (((float)Player()->GetSpellBook()->GetLevel(kSlotE)) / 10.0f) + 0.4f;
		float fastestTime = ((playerPos - target).Length()-100) / Player()->MovementSpeed();
		IUnit* best = NULL;
		vector<IUnit*> around;
		set<IUnit*> *path = new set<IUnit*>();
		UnitTree.FindInRange(playerPos, E->Range(), around);
		auto myDist = Distance(Player(), target);
		for (auto u : around)
		{
			if(!EnemyIsJumpable(u))
				continue;
			if(ignore != NULL && ignore->count(u) != 0)
				continue;
			Vec2 posAfterJump = playerPos.Extend(u->GetPosition().To2D(), E->Range());
			auto unitDist = Distance(posAfterJump, target);
			if(unitDist -100 > myDist)
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
		if (path->find(jump) == path->end() && path->size()<4)
		{
			path->insert(jump);
			auto pos = jump->GetPosition().To2D();
			//Prediction if first check
			if (path->size() == 1 && jump->IsMoving())
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
				Vec2 fromAfterWalk = posAfterJump.Extend(target, Player()->MovementSpeed()*(jumpCost-0.4f));
				auto time = GetCost(u, fromAfterWalk, target, jumpCost, totalCost + jumpCost, path);
				if (time < fastestTime)
					fastestTime = time;
			}
			path->erase(jump);
			return fastestTime;
		}
		return totalCost + (from - target).Length() / Player()->MovementSpeed();
	}


	void GetAround(Vec2 orgin, float radius, vector<IUnit*> &result, function<bool(IUnit*)> predicate)
	{
		vector<IUnit*> temp;
		UnitTree.FindInRange(orgin, radius, temp);
		for(auto unit : temp)
		{
			if (unit != NULL && predicate(unit))
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

		IUnit* bestJumpUnit = 0;
		float bestRange = Distance(Player(), position);
		if(YasuoMenu->ComboEPathfinder)
			bestJumpUnit = BestEJump(position, ignore);
		else
		{
			vector<IUnit*> around;
			UnitTree.FindInRange(Player()->GetPosition().To2D(), E->Range(), around);
			for (auto unit : around)
			{
				if (!EnemyIsJumpable(unit))
					continue;
				if (ignore != 0 && ignore->find(unit) != ignore->end())
					continue;
				Vec3 predictionPos;
				GPrediction->GetFutureUnitPosition(unit, E->GetDelay(), true, predictionPos);

				Vec2 posAfterJump = Player()->GetPosition().To2D().Extend(predictionPos.To2D(), E->Range());
				float dist = (posAfterJump - position).Length();
				if (dist < bestRange)
				{
					bestJumpUnit = unit;
					bestRange = dist;
				}
			}
		}

		if (bestJumpUnit != 0)
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
		float trueAARange = Player()->AttackRange() ;
		Vec3 predictionPos;
		GPrediction->GetFutureUnitPosition(target, E->GetDelay(), true, predictionPos);
		float dist = Distance(Player(), predictionPos);
		Vec2 posAfterJump = Player()->GetPosition().To2D().Extend(predictionPos.To2D(), E->Range());
		float distAfter = (predictionPos.To2D() - posAfterJump).Length();
		if (dist < distAfter || target->IsMoving() || IsFacing(target,Player()))
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
		if (timeToLand > 0.4f || !YasuoMenu->ComboRUseOnLanding->Enabled())
			return false;
		for (auto ene : airborns)
		{
			int aroundAir = 0;
			for (auto ene2 : airborns)
			{
				if (Distance(ene, ene2) < 400)
					aroundAir++;
			}

			if (aroundAir >= YasuoMenu->ComboREnemiesCount->GetInteger() || ene->HealthPercent() < 35) {
				if (R->CastOnPosition(ene->GetPosition()))
					return true;
			}
		}
		return false;
	}

	bool HasCreepFlags(IUnit* unit)
	{
		return unit->UnitFlags() == FL_HERO || unit->UnitFlags() == FL_CREEP;
	}

	bool EnemyIsJumpable(IUnit* enemy)
	{
		if (!HasCreepFlags(enemy))
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
		if (!Q->IsReady() || Distance(Player(), target) > Q->Range())
			return false;
		cout << "CastQ "<< Q->Range() << endl;
		return Q->CastOnTarget(target, kHitChanceHigh);
	}

	bool CastQEmpowered(IUnit* target)
	{
		if (!IsQEmpovered() || !QEmpowerd->IsReady() || Distance(Player(), target)>QEmpowerd->Range())
			return false;
		cout << "CastQEmpowered" << endl;
		return QEmpowerd->CastOnTarget(target, kHitChanceHigh);
	}

	bool CastQCircle(IUnit* target)
	{
		if (!IsDashing())
			return false;
		cout << "Qempow cirtlce" << endl;
		Vec3 pred;
		GPrediction->GetFutureUnitPosition(target, GameTicks() - LastDash.EndTick, true, pred);

		if (Distance(target, LastDash.EndPosition.To2D()) < QCircle->Radius() 
			&& Distance(pred.To2D(), LastDash.EndPosition.To2D()) < QCircle->Radius())
			return Q->CastOnTarget(target);
		return false;
	}

	bool CastE(IUnit* target)
	{
		if (!E->IsReady())
			return false;
		Vec2 posAfterJump = Player()->GetPosition().To2D().Extend(target->GetPosition().To2D(), E->Range());
		vector<IUnit*> enemyTowers;
		GetAround(posAfterJump, 950, enemyTowers, [&](IUnit* unit) -> bool
		{
			return unit->UnitFlags() == FL_TURRET && !unit->IsDead() && unit->IsEnemy(Player());
		});

		if(YasuoMenu->ComboDontTowerDive->Enabled())
			for(auto tower : enemyTowers)
			{
				if (Distance(tower, posAfterJump) < Distance(tower, Player()))
					return false;
			}

		return E->CastOnUnit(target);
	}

#pragma endregion spellsExecute  

#pragma region spellsLoad  
	void LoadSpells()
	{
		Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, true, kCollidesWithNothing);
		Q->SetSkillshot(GetQSpeed(), 50.f, FLT_MAX, 475.f);

		QEmpowerd = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, true, kCollidesWithNothing);
		QEmpowerd->SetSkillshot(GetQEmpowerdSpeed(), 50.f, 1200.f, 1000.f);

		QCircle = GPluginSDK->CreateSpell2(kSlotQ, kCircleCast, false, true, kCollidesWithNothing);
		QCircle->SetSkillshot(GetQSpeed(), 300.f, FLT_MAX, 0.f);

		W = GPluginSDK->CreateSpell2(kSlotW, kLineCast, false, true, kCollidesWithMinions);
		W->SetOverrideRange(400.f);

		E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
		E->SetOverrideRange(475.f);
		E->SetOverrideDelay(0.1f);

		R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, false, false, kCollidesWithMinions);
		W->SetOverrideRange(1200.f);
	}

	void UpdateChampionData() {
		Q->SetOverrideDelay(GetQSpeed());
		QEmpowerd->SetOverrideDelay(GetQEmpowerdSpeed());
		QCircle->SetOverrideDelay(GetQSpeed());
	}

	float getEDamage(IUnit* target)
	{
		//TODO: implement buff
		auto base = GDamage->GetSpellDamage(Player(), target, kSlotE);
		return (float)base;
	}

	float getQDamage(IUnit* target)
	{
		//TODO: implement buff
		auto base = GDamage->GetSpellDamage(Player(), target, kSlotQ);
		return (float)base;
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
		return LastDash.EndTick+100 > GameTicks();
	}

#pragma endregion yasuoInfo  

};