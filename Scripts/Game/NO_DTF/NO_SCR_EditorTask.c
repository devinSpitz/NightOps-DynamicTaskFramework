[EntityEditorProps(category: "GameScripted/Tasks", description: "Move task.", color: "0 0 255 255")]
class NO_SCR_EditorTaskClass: SCR_EditorTaskClass
{
};

//------------------------------------------------------------------------------------------------
class NO_SCR_EditorTask : SCR_EditorTask
{	
	

	ref array<NO_SCR_EditorTask> m_aChildren = new array<NO_SCR_EditorTask>();
	[Attribute("USSR", UIWidgets.EditBox, "Faction is to ask whitch fraction can activeate the trigger", category: "TaskManager:")]
	FactionKey m_faction;	
	
	[Attribute("1", UIWidgets.CheckBox, "Assign the task to its faction from the beginning", category: "TaskManager:")]
	bool m_bAssignToFactionOnStart;	
	
	
	[Attribute("", UIWidgets.EditBox, "When set and the a task is assigned as well as the TaskManager has the checkbox set the marker will be on the position of the ObjectName you entered here!", category: "TaskManager:" )]
	string m_sAdditionalMarkerPosition;
	
	[Attribute("", UIWidgets.EditBox, "Add names of tasks to be created when Task is finished.", category: "TaskManager:" )]
	ref array<string> m_sCreateTaskNamesSuccess;
	
	[Attribute("", UIWidgets.EditBox, "Add names of tasks to be created when Task is failed.", category: "TaskManager:" )]
	ref array<string> m_sCreateTaskNamesFail;
		
	[Attribute("0", UIWidgets.CheckBox, "Assign the first task in the list", category: "TaskManager:")]
	bool m_bAssignFirstTask;
	
	[Attribute("0", UIWidgets.CheckBox, desc: "End game when Task is finished", category: "Game Over")]
	protected bool m_bEnableGameOverSuccess;
	
	[Attribute("EDITOR_FACTION_VICTORY", UIWidgets.ComboBox, desc: "Customize these on SCR_GameOverScreenManagerComponent on SCR_BaseGameMode.", category: "Game Over", enums: ParamEnumArray.FromEnum(ESupportedEndReasonsTask))]
	protected int m_iGameOverTypeSuccess;
	
	[Attribute("US", UIWidgets.EditBox, desc: "Key of winning faction, or player faction if draw.", category: "Game Over")]
	protected string m_sWinningFactionKeySuccess;
	
	
	[Attribute("0", UIWidgets.CheckBox, desc: "End game when Task is finished", category: "Game Over")]
	protected bool m_bEnableGameOverFail;
	
	[Attribute("EDITOR_FACTION_VICTORY", UIWidgets.ComboBox, desc: "Customize these on SCR_GameOverScreenManagerComponent on SCR_BaseGameMode.", category: "Game Over", enums: ParamEnumArray.FromEnum(ESupportedEndReasonsTask))]
	protected int m_iGameOverTypeFail;
	
	[Attribute("USSR", UIWidgets.EditBox, desc: "Key of winning faction, or player faction if draw.", category: "Game Over")]
	protected string m_sWinningFactionKeyFail;
	
	TriggerType TaskState = null;
	RplComponent m_pRplComponent;
	IEntity Owner;
	private bool alreadyAssigned = false;
	
	ArmaReforgerScripted game;
	BaseWorld world;
	
	override void EOnInit(IEntity owner)
	{
		
		super.EOnInit(owner);
		Owner = owner;
		
		if (!GetGame().InPlayMode()) return;
		
		//tell the taskmanager that we are here
		SCR_BaseTaskManager manager = GetTaskManager();
		NO_SCR_TaskManager realManager = NO_SCR_TaskManager.Cast(manager);
		realManager.m_aTasks.Insert(this);
	
		
		m_pRplComponent = RplComponent.Cast(this.FindComponent(RplComponent));
		if(!m_pRplComponent) Debug.Error("NO_SCR_EditorTask cannot hook to the RplComponent please add one!");
		

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
		
		if(!m_pRplComponent.IsMaster()) return;
			
		
				
		if(m_bAssignToFactionOnStart) 
		{
			ChangeStateOfTask(TriggerType.Create);
			ChangeStateOfTask(TriggerType.Assign);
		}
		
		
		IEntity Parent = GetParent();
		if(!Parent) Debug.Error("NO_SCR_TaskTrigger cannot hook to the Parent so its not a child of it!");
		
		NO_SCR_EditorTask parentTask = NO_SCR_EditorTask.Cast(Parent);
		if(parentTask&&Owner!=null)
		{
			parentTask.m_aChildren.Insert(this);
		}
		
	}
	
	
	void SetJIPState()
	{
		if(TaskState)
		ChangeStateOfTask(TaskState);
	}
	
	void ChangeStateOfTask(TriggerType m_tTriggerType)
	{
		
		if(!m_pRplComponent) return;
		if(!m_pRplComponent.IsMaster()) return;
		NO_SCR_EditorTask ParentTask = NO_SCR_EditorTask.Cast(this);
		if(!ParentTask) Debug.Error("NO_SCR_EditorTask cannot hook to the Editor Task so its not a child of it!");
		
		if(!game) return;
		
		PlayerManager playerManager = game.GetPlayerManager();
		
		
		array<int> players = {};
		playerManager.GetAllPlayers(players);
		
		SCR_BaseTaskManager manager = NO_SCR_TaskManager.Cast(GetTaskManager());
		if(m_tTriggerType==TriggerType.Assign)
		{
			foreach(int playerId  : players)
			{
				auto taskExecutor = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerId);
				manager.AssignTask(ParentTask,taskExecutor,true);
			}
			TaskState = TriggerType.Assign;
			
		}
		else if(m_tTriggerType==TriggerType.Fail)
		{
			manager.FailTask(ParentTask);
			TaskState = TriggerType.Fail;
			GameOverLose();
		}
		else if(m_tTriggerType==TriggerType.Finish)
		{
			manager.FinishTask(ParentTask);
			TaskState = TriggerType.Finish;
			GameOverWin();
		}
		else if(m_tTriggerType==TriggerType.Create)
		{
			manager.SetTaskFaction(ParentTask,game.GetFactionManager().GetFactionByKey(ParentTask.m_faction));
			TaskState = TriggerType.Create;
		}
		
		
	}
	
	bool IsBadTask(bool m_allowFailedTasks)
	{
		if(TaskState != TriggerType.Finish || (m_allowFailedTasks && TaskState != TriggerType.Finish))
		{
			return true;
		}
		
		return false;
	}
	
	protected void GameOverWin()
	{
		if (!m_bEnableGameOverSuccess)
			return;

		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return;

		Faction winningFaction = GetGame().GetFactionManager().GetFactionByKey(m_sWinningFactionKeySuccess);
		if (!winningFaction)
			return;

		int winningFactionIndex = GetGame().GetFactionManager().GetFactionIndex(winningFaction);

		if (winningFactionIndex != -1)
			gameMode.EndGameMode(SCR_GameModeEndData.CreateSimple(m_iGameOverTypeSuccess, -1, winningFactionIndex));
	}
	
	protected void GameOverLose()
	{
		if (!m_bEnableGameOverFail)
			return;

		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return;

		Faction winningFaction = GetGame().GetFactionManager().GetFactionByKey(m_sWinningFactionKeyFail);
		if (!winningFaction)
			return;

		int winningFactionIndex = GetGame().GetFactionManager().GetFactionIndex(winningFaction);

		if (winningFactionIndex != -1)
			gameMode.EndGameMode(SCR_GameModeEndData.CreateSimple(m_iGameOverTypeFail, -1, winningFactionIndex));
	}
	
	protected void CreateNewTasksWin()
	{
		alreadyAssigned = false;
		foreach(string task : m_sCreateTaskNamesSuccess)
		{
			IEntity newtaskEntity = world.FindEntityByName(task);
			NO_SCR_EditorTask newtaskObject = NO_SCR_EditorTask.Cast(newtaskEntity);
			newtaskObject.ChangeStateOfTask(TriggerType.Create);	
			if (alreadyAssigned) return;
			if (!m_bAssignFirstTask) return;
			newtaskObject.ChangeStateOfTask(TriggerType.Assign);		
			alreadyAssigned = true;
		}
	}
	
	protected void CreateNewTasksLose()
	{
		alreadyAssigned = false;
		foreach(string task : m_sCreateTaskNamesFail)
		{
			IEntity newtaskEntity = world.FindEntityByName(task);
			NO_SCR_EditorTask newtaskObject = NO_SCR_EditorTask.Cast(newtaskEntity);
			newtaskObject.ChangeStateOfTask(TriggerType.Create);	
			if (alreadyAssigned) return;
			if (!m_bAssignFirstTask) return;
			newtaskObject.ChangeStateOfTask(TriggerType.Assign);		
			alreadyAssigned = true;
		}
	}
		
}


enum ESupportedEndReasonsTask
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