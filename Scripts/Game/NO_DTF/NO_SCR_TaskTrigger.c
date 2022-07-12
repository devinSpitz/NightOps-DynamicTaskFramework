[EntityEditorProps(category: "GameScripted/Triggers", description: "Trigger that can aissgn, fail or finish Task")]
class NO_SCR_TaskTriggerClass: SCR_BaseTriggerEntityClass
{
};


class NO_SCR_TaskTrigger : SCR_BaseTriggerEntity
{
	//Action
	[Attribute("1", UIWidgets.ComboBox, "What should that trigger do to its parent Task?","", ParamEnumArray.FromEnum(TriggerType), category: "TaskManager:" )]
	protected TriggerType m_tTriggerType;	
	
	//On

	[Attribute("1", UIWidgets.ComboBox, "When should the trigger check for the action?","", ParamEnumArray.FromEnum(ActivateType), category: "TaskManager:" )]
	protected ActivateType m_eActivateType;	
	//On

	[Attribute("1", UIWidgets.ComboBox, "Condition?","", ParamEnumArray.FromEnum(WhenTypeTrigger), category: "TaskManager:" )]
	protected WhenTypeTrigger m_eWhenTypeTrigger;	
	
	
	[Attribute("", UIWidgets.EditBox, "When the Condition is on NamedObject than you ahve to set the name here", category: "TaskManager:" )]
	string m_sTriggerNamedObject;
	
	[Attribute("1", UIWidgets.CheckBox, "Should the trigger only work one time?", category: "TaskManager:" )]
	bool m_bOneTimeTrigger;
	
	[Attribute("1", UIWidgets.CheckBox, "Only player are counted", category: "TaskManager:" )]
	bool m_bPlayerOnly;
	
	private ref array<IEntity> playerInTrigger = new array<IEntity>();
	private RplComponent m_pRplComponent;
	private IEntity Owner;
	private IEntity Parent;
	private NO_SCR_EditorTask ParentTask;
	private bool alreadyTriggered = false;
	private bool TriggeredWith = false;
	private NO_SCR_TaskManager manager;
	//Thanks to Herbiie for his code and the wiki: https://github.com/Herbiie/ArmAReforgerMissionMakingGuide
    // Set up the filter
    override bool ScriptedEntityFilterForQuery(IEntity ent) {
		if (!GetGame().InPlayMode()) return false;
		if(!ParentTask) return false;
        SCR_ChimeraCharacter cc = SCR_ChimeraCharacter.Cast(ent);
        if (!cc) return false; // If the entity is not a person, filter it out
        if (!IsAlive(cc)) return false; // If the entity is dead, filter it out
		if(m_eWhenTypeTrigger == WhenTypeTrigger.AnyChimera) return true; // Any chimera character should trigger
        if (cc.GetFactionKey() != ParentTask.m_faction) return false; // If the entity does not have the Faction key of USSR, filter it out
        if (!IsPlayer(ent) && m_bPlayerOnly) return false; // If the entity does not have the Faction key of USSR, filter it out
		
        return true; // Otherwise, include it!
    }
	
	bool IsPlayer(IEntity ent) 
    {
      int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(ent); 
      return playerId > 0;
    }
	
	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		Owner=owner;
		
		if (!GetGame().InPlayMode()) return;
		if(m_eWhenTypeTrigger == WhenTypeTrigger.NamedObject && m_sTriggerNamedObject == "") 
		{
			 Debug.Error("NO_SCR_TaskTrigger condition is set to named object but there is no name set!");
		}
		
		if (!GetGame().InPlayMode()) return;
		
		
		Parent = GetParent();
		if(!Parent) Debug.Error("NO_SCR_TaskTrigger cannot hook to the Parent so its not a child of it!");
		
		
		ParentTask = NO_SCR_EditorTask.Cast(Parent);
		if(!ParentTask) Debug.Error("NO_SCR_TaskTrigger cannot hook to the Editor Task so its not a child of it!");
		
		
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if(!m_pRplComponent) Debug.Error("NO_SCR_TaskTrigger cannot hook to the RplComponent please add one!");
		
		
		if(m_pRplComponent.IsMaster())
		{
			SCR_BaseTaskManager taskmanager = GetTaskManager();
			manager = NO_SCR_TaskManager.Cast(taskmanager);
			
			if(!manager) Debug.Error("NO_SCR_TaskTrigger cannot hook on the TaskManager!");	
		}
		
		
		
	}
	
	
	
	override void OnActivate(IEntity ent)
	{
		if(m_pRplComponent.IsMaster())
		{
			playerInTrigger.Insert(ent);
			Do(true);
		}
	}
	
	override void OnDeactivate(IEntity ent)
	{
		if(m_pRplComponent.IsMaster())
		{
			playerInTrigger.RemoveItem(ent);
			Do(false);
		}
	}
	
	//return as soon as its not valide anymore
	void Do(bool enter)
	{
		if(alreadyTriggered && m_bOneTimeTrigger) return;
		if(!m_pRplComponent.IsMaster()) return;
		
		if(m_eActivateType == ActivateType.Enter && !enter) return;
		if(m_eActivateType == ActivateType.Leave && enter) return;
		
		
		ArmaReforgerScripted game = GetGame();
		if(!game) return;
		
		NO_SCR_TaskManager taskManager = NO_SCR_TaskManager.Cast(GetTaskManager());
		if (taskManager && ParentTask && taskManager.m_Dummyfaction == ParentTask.GetTargetFaction().GetFactionKey()) return;
		
		PlayerManager playerManager = game.GetPlayerManager();
		array<int> players = {};
		playerManager.GetAllPlayers(players);
		if(m_eWhenTypeTrigger == WhenTypeTrigger.All)
		{
			if(players.Count()!=playerInTrigger.Count()) return;
		}
		else if(m_eWhenTypeTrigger == WhenTypeTrigger.One)
		{
			if(playerInTrigger.Count()<=0) return;
		}
		else if(m_eWhenTypeTrigger == WhenTypeTrigger.Half)
		{
			if((players.Count()/2)!=playerInTrigger.Count()) return;
		}
		else if(m_eWhenTypeTrigger == WhenTypeTrigger.MoreThanHalf)
		{
			if((players.Count()/2)<=playerInTrigger.Count()) return;
		}
		else if(m_eWhenTypeTrigger == WhenTypeTrigger.LessThanHalf)
		{
			if((players.Count()/2)>=playerInTrigger.Count()) return;
		}
		else if(m_eWhenTypeTrigger == WhenTypeTrigger.AnyChimera)
		{
			if(playerInTrigger.Count()<=0) return;
		}
		else if(m_eWhenTypeTrigger == WhenTypeTrigger.NamedObject)
		{
			bool NamedObjectInside = false;
			foreach(IEntity player: playerInTrigger)
			{
				if(player.GetName()==m_sTriggerNamedObject)
					NamedObjectInside = true;
			}
			if(!NamedObjectInside)
				return;
		}
		
		ParentTask.ChangeStateOfTask(m_tTriggerType);
		alreadyTriggered = true;
	}
	
	
}