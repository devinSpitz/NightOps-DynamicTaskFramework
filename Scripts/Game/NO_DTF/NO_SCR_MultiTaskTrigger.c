[EntityEditorProps(category: "GameScripted/Triggers", description: "Trigger that can aissgn, fail or finish task on complation of other tasks")]
class NO_SCR_MultiTaskTriggerComponentClass: ScriptComponentClass
{
};


class NO_SCR_MultiTaskTriggerComponent : ScriptComponent
{
	

	//Action
	[Attribute("1", UIWidgets.ComboBox, "What should that trigger do to its parent Task?","", ParamEnumArray.FromEnum(TriggerType), category: "TaskManager:" )]
	protected TriggerType m_tTriggerType;	
	
	//Slider
	[Attribute("3", UIWidgets.Slider, "How many seconds between each call should be waited?", "0 240 1", category: "TaskManager:" )]
	protected int m_iSecondsInBetweenTriggerChecks;	
	
	[Attribute("", UIWidgets.EditBox, "The object name that you want to trigger this action?", category: "TaskManager:" )]
	ref array<string> m_sTaskNames;
	
	[Attribute("0", UIWidgets.CheckBox, "Not only allow completed task to trigger but also failed", category: "TaskManager:")]
	bool m_allowFailedTasks;	
	
	[Attribute("", UIWidgets.EditBox, "Add names of tasks to be created when MultiTask is complete.", category: "TaskManager:" )]
	ref array<string> m_sCreateTaskNames;
		
	[Attribute("0", UIWidgets.CheckBox, "Assign the first task in the list", category: "TaskManager:")]
	bool m_bAssignFirstTask;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "End game when Multi Task complete", category: "TaskManager:")]
	protected bool m_bEnableGameOver;
	
	[Attribute("EDITOR_FACTION_VICTORY", UIWidgets.ComboBox, desc: "Customize these on SCR_GameOverScreenManagerComponent on SCR_BaseGameMode.", category: "TaskManager:", enums: ParamEnumArray.FromEnum(ESupportedEndReasons))]
	protected int m_iGameOverType;
	
	[Attribute("US", UIWidgets.EditBox, desc: "Key of winning faction, or player faction if draw.", category: "TaskManager:")]
	protected string m_sWinningFactionKey;
	
	
	private RplComponent m_pRplComponent;
	private IEntity Owner;
	private NO_SCR_EditorTask OwnTask;
	
	private bool alreadyTriggered = false;
	private bool alreadyAssigned = false;
	private NO_SCR_TaskManager manager;
	
	ArmaReforgerScripted game;
	BaseWorld world;
	
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		if (!GetGame().InPlayMode()) return;
		SetEventMask(owner, EntityEvent.INIT);
	}

	
	void FrequentlyCheckTrigger()
	{
		Do();
	}
	
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		Owner = owner;
		if (!GetGame().InPlayMode()) return;
		
		OwnTask = NO_SCR_EditorTask.Cast(owner);
		if(!OwnTask) Debug.Error("NO_SCR_TaskTrigger cannot hook to the Editor Task so not part of one!");
		
		
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
		if(alreadyTriggered) return;
		if(!m_pRplComponent.IsMaster()) return;
		

		
		
		foreach(string task : m_sTaskNames)
		{
			IEntity taskEntity = world.FindEntityByName(task);
			NO_SCR_EditorTask taskObject = NO_SCR_EditorTask.Cast(taskEntity);
			
			if(taskObject &&taskObject.IsBadTask(m_allowFailedTasks)) return;
				
		}
		
		foreach(NO_SCR_EditorTask task: OwnTask.m_aChildren)
		{
			
			if(task && task.IsBadTask(m_allowFailedTasks)) return;
		}
		


		OwnTask.ChangeStateOfTask(m_tTriggerType);
		foreach(string task : m_sCreateTaskNames)
		{
			IEntity newtaskEntity = world.FindEntityByName(task);
			NO_SCR_EditorTask newtaskObject = NO_SCR_EditorTask.Cast(newtaskEntity);
			newtaskObject.ChangeStateOfTask(TriggerType.Create);	
			if (alreadyAssigned) return;
			if (!m_bAssignFirstTask) return;
			newtaskObject.ChangeStateOfTask(TriggerType.Assign);		
			alreadyAssigned = true;
		}
		alreadyTriggered = true;
		GameOver();
		
	}
	
	protected void GameOver()
	{
		if (!m_bEnableGameOver)
			return;

		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return;

		Faction winningFaction = GetGame().GetFactionManager().GetFactionByKey(m_sWinningFactionKey);
		if (!winningFaction)
			return;

		int winningFactionIndex = GetGame().GetFactionManager().GetFactionIndex(winningFaction);

		if (winningFactionIndex != -1)
			gameMode.EndGameMode(SCR_GameModeEndData.CreateSimple(m_iGameOverType, -1, winningFactionIndex));
	}
	
	
}
	
enum ESupportedEndReasons
{
	UNDEFINED = -1,
	TIMELIMIT = -2,
	SCORELIMIT = -3,
	DRAW = -4,
	SERVER_RESTART = -5,

	EDITOR_NEUTRAL = 1000,
	EDITOR_FACTION_NEUTRAL = 1001,
	EDITOR_FACTION_VICTORY = 1002,
	//EDITOR_FACTION_DEFEAT = 1003,
	EDITOR_FACTION_DRAW = 1004
}
