[EntityEditorProps(category: "GameScripted/Triggers", description: "Trigger that can aissgn, fail or finish Task")]
class NO_SCR_TaskNamedObjectTriggerComponentClass: ScriptComponentClass
{
};


class NO_SCR_TaskNamedObjectTriggerComponent : ScriptComponent
{
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		Owner = owner;
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		SetEventMask(owner, EntityEvent.INIT);
	}

	
	//Action
	[Attribute("1", UIWidgets.ComboBox, "What should that trigger do to its parent Task?","", ParamEnumArray.FromEnum(TriggerType), category: "TaskManager:" )]
	protected TriggerType m_tTriggerType;	
	


	[Attribute("1", UIWidgets.ComboBox, "Condition?","", ParamEnumArray.FromEnum(WhenTypeTriggerNamed), category: "TaskManager:" )]
	protected WhenTypeTriggerNamed m_eWhenTypeTrigger;
	
	//Slider
	[Attribute("3", UIWidgets.Slider, "How many seconds between each call should be waited?", "0 240 1", category: "TaskManager:" )]
	protected int m_iSecondsInBetweenTriggerChecks;	
	
	[Attribute("1", UIWidgets.CheckBox, "Should the trigger only work one time?", category: "TaskManager:" )]
	bool m_bOneTimeTrigger;
	
	
	[Attribute("", UIWidgets.EditBox, "The object name that you want to trigger this action?", category: "TaskManager:" )]
	string m_sTriggerNamedObject;
	
	private IEntity InTrigger;
	private RplComponent m_pRplComponent;
	private IEntity Owner;
	private IEntity Parent;
	private NO_SCR_EditorTask ParentTask;
	
	private bool alreadyTriggered = false;
	private bool TriggeredWith = false;
	private NO_SCR_TaskManager manager;
	private bool wasAlive = false;
	
	ArmaReforgerScripted game;
	BaseWorld world;
	
	void FrequentlyCheckTrigger()
	{
		Do();
	}
	
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode()) return;
		
		
		Parent = owner.GetParent();
		if(!Parent) Debug.Error("NO_SCR_TaskTrigger cannot hook to the Parent so its not a child of it!");
		
		
		ParentTask = NO_SCR_EditorTask.Cast(Parent);
		if(!ParentTask) Debug.Error("NO_SCR_TaskTrigger cannot hook to the Editor Task so its not a child of it!");
		
		
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if(!m_pRplComponent) Debug.Error("NO_SCR_TaskTrigger cannot hook to the RplComponent please add one!");
		
		
		if(m_pRplComponent.IsMaster())
		{
			SCR_BaseTaskManager taskmanager = GetTaskManager();
			manager = NO_SCR_TaskManager.Cast(taskmanager);
			
			if(!manager)
			{
				Debug.Error("NO_SCR_TaskTrigger cannot hook on the TaskManager!");	
				return;
			} 
			
			
			game = GetGame();
			if(!game)
			{
				Debug.Error("NO_SCR_TaskTrigger cannot hook on the Game!");	
				return;
			} 
			
			world = game.GetWorld();
			if(!world)
			{
				Debug.Error("NO_SCR_TaskTrigger cannot hook on the World!");	
				return;
			} 
			
			GetGame().GetCallqueue().CallLater(FrequentlyCheckTrigger, m_iSecondsInBetweenTriggerChecks, true);	
		}
		
		
		
	}
	
	
	
	//return as soon as its not valide anymore
	void Do()
	{
		if(alreadyTriggered && m_bOneTimeTrigger) return;
		if(!m_pRplComponent.IsMaster()) return;
		
		
		//PlayerManager playerManager = game.GetPlayerManager();
		//array<int> players = {};
		//playerManager.GetAllPlayers(players);
		
		
		
		
		if(m_eWhenTypeTrigger == WhenTypeTriggerNamed.Appear)
		{
			if(!InTrigger)
			{
				InTrigger = world.FindEntityByName(m_sTriggerNamedObject);
				if(InTrigger)
				{
					//Object appeared
					ParentTask.ChangeStateOfTask(m_tTriggerType);
					alreadyTriggered = true;
				}
			}
			Reset();
		}
		else if(m_eWhenTypeTrigger == WhenTypeTriggerNamed.OnlyAiOnDeath || m_eWhenTypeTrigger == WhenTypeTriggerNamed.Disappear )
		{
			if(!InTrigger && !wasAlive)
			{
				InTrigger = world.FindEntityByName(m_sTriggerNamedObject);
			}
			
			if(InTrigger && !wasAlive)
			{
				Print("wasAlive");
				wasAlive = true;
			}
			else if(wasAlive && !InTrigger)
			{
				
				Print("Finished Task");
				//SCR_ChimeraCharacter Died
				alreadyTriggered = true;
				ParentTask.ChangeStateOfTask(m_tTriggerType);
				Reset();
			}
			
			
		}


		
	}

	protected bool IsAlive(IEntity entity)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
		if (damageManager)
			return damageManager.GetState() != EDamageState.DESTROYED;
		else
			return true;
	}
	
	void Reset()
	{
		if(InTrigger && !m_bOneTimeTrigger)
		{
			wasAlive = false;
			InTrigger = null;
		}
	}
	
}