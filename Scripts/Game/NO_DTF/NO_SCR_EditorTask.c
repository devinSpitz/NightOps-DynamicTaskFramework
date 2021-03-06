[EntityEditorProps(category: "GameScripted/Tasks", description: "Move task.", color: "0 0 255 255")]
class NO_SCR_EditorTaskClass: SCR_EditorTaskClass
{
};

//------------------------------------------------------------------------------------------------
class NO_SCR_EditorTask : SCR_EditorTask
{	
	ref array<NO_SCR_EditorTask> m_aChildren = new array<NO_SCR_EditorTask>();
	
	[Attribute("1", UIWidgets.CheckBox, desc: "Activate Task Marker For This Task", category: "TaskManager:"),RplProp()]
	bool m_bActivateTaskMarkerForThisTask;
	
	[Attribute("USSR", UIWidgets.EditBox, "Faction is to ask which fraction can activate the trigger", category: "TaskManager:")]
	FactionKey m_faction;	
	
	[Attribute("0", UIWidgets.CheckBox, "Create the task from the beginning", category: "TaskManager:")]
	bool m_bCreateOnStart;	
	
	[Attribute("1", UIWidgets.CheckBox, "Assign the task to its faction from the beginning", category: "TaskManager:")]
	bool m_bAssignToFactionOnStart;	
	
	[Attribute("0", UIWidgets.CheckBox,"Does fail or complete only temporarly so the task can be reused!", category: "TaskManager:")]
	bool m_bFailOrCompleteTemporarly;
	
	[Attribute("", UIWidgets.EditBox, "When set and the a task is assigned as well as the TaskManager has the checkbox set the marker will be on the position of the ObjectName you entered here!", category: "TaskManager:" )]
	string m_sAdditionalMarkerPosition;
	
	[Attribute("", UIWidgets.EditBox, "Add names of tasks to be created when Task is finished.", category: "Successor:" )]
	ref array<string> m_sCreateTaskNamesSuccess;
	
	[Attribute("", UIWidgets.EditBox, "Add names of tasks to be created when Task is failed.", category: "Successor:" )]
	ref array<string> m_sCreateTaskNamesFail;
		
	[Attribute("0", UIWidgets.CheckBox, "Assign the first task in the list", category: "Successor:")]
	bool m_bAssignFirstTask;	
	
	[Attribute("0", UIWidgets.CheckBox, desc: "End game when Task is finished", category: "Game Over")]
	protected bool m_bEnableGameOverOnSuccess;
	
	[Attribute("EDITOR_FACTION_VICTORY", UIWidgets.ComboBox, desc: "Customize these on SCR_GameOverScreenManagerComponent on SCR_BaseGameMode.", category: "Game Over", enums: ParamEnumArray.FromEnum(ESupportedEndReasonsTask))]
	protected int m_iGameOverTypeSuccess;
	
	[Attribute("US", UIWidgets.EditBox, desc: "Key of winning faction, or player faction if draw.", category: "Game Over")]
	protected string m_sWinningFactionKeySuccess;
	
	
	[Attribute("0", UIWidgets.CheckBox, desc: "End game when Task is failed", category: "Game Over")]
	protected bool m_bEnableGameOverOnFail;
	
	[Attribute("EDITOR_FACTION_VICTORY", UIWidgets.ComboBox, desc: "Customize these on SCR_GameOverScreenManagerComponent on SCR_BaseGameMode.", category: "Game Over", enums: ParamEnumArray.FromEnum(ESupportedEndReasonsTask))]
	protected int m_iGameOverTypeFail;
	
	[Attribute("USSR", UIWidgets.EditBox, desc: "Key of winning faction, or player faction if draw.", category: "Game Over")]
	protected string m_sWinningFactionKeyFail;
	
	
	
	
	[RplProp()]
	TriggerType TaskState = null;
	
	RplComponent m_pRplComponent;
	IEntity Owner;
	private bool alreadyAssigned = false;
	
	ArmaReforgerScripted game;
	BaseWorld world;
	NO_SCR_TaskManager realManager;

	//Dont Insert stuff inside of this script!
	ref ScriptInvoker s_OnTaskFail = new ref ScriptInvoker();
	ref ScriptInvoker s_OnTaskComplete = new ref ScriptInvoker();
	ref ScriptInvoker s_OnTaskCreate = new ref ScriptInvoker();
	ref ScriptInvoker s_OnTaskAssign = new ref ScriptInvoker();
	//---------------------------

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		Owner = owner;
		
		if (!GetGame().InPlayMode()) return;
		
		//tell the taskmanager that we are here
		SCR_BaseTaskManager manager = GetTaskManager();
		realManager = NO_SCR_TaskManager.Cast(manager);
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
		
		
		manager.SetTaskFaction(this,GetGame().GetFactionManager().GetFactionByKey(realManager.m_Dummyfaction));
		
		if(m_bCreateOnStart && !m_bAssignToFactionOnStart)
		{
			ChangeStateOfTask(TriggerType.Create);
		}
				
		if(m_bAssignToFactionOnStart) 
		{
			ChangeStateOfTask(TriggerType.Assign);
		}
		
		
		IEntity Parent = GetParent();
		if(!Parent) 
		{
			Print("NO_SCR_TaskTrigger cannot hook to the Parent so its not a child of it!");
			return;
		}
		
		NO_SCR_EditorTask parentTask = NO_SCR_EditorTask.Cast(Parent);
		if(parentTask&&Owner!=null)
		{
			parentTask.m_aChildren.Insert(this);
		}
		
		
	}
	
	
	void SetJIPState()
	{
		if(TaskState)
		{
			if(TaskState == TriggerType.Assign)
			{
				
				ChangeStateOfTask(TriggerType.Create,true);
				GetGame().GetCallqueue().CallLater(ChangeStateOfTask, 1, false, TriggerType.Assign,true);
			}
			else if(TaskState == TriggerType.Create)
			{
				ChangeStateOfTask(TriggerType.Create,true);
			}
			else if(TaskState == TriggerType.Finish)
			{
				ChangeStateOfTask(TriggerType.Create,true);
				GetGame().GetCallqueue().CallLater(ChangeStateOfTask, 1, false, TriggerType.Assign,true);
				GetGame().GetCallqueue().CallLater(ChangeStateOfTask, 2, false, TriggerType.Finish,true);
			}
			else if(TaskState == TriggerType.Fail)
			{
				ChangeStateOfTask(TriggerType.Create,true);
				GetGame().GetCallqueue().CallLater(ChangeStateOfTask, 1, false, TriggerType.Assign,true);
				GetGame().GetCallqueue().CallLater(ChangeStateOfTask, 2, false, TriggerType.Fail,true);
			}
		}
		
	}
	
	void ChangeStateOfTask(TriggerType m_tTriggerType,bool forceClient = false)
	{
		
		if(!m_pRplComponent) return;
		if(!m_pRplComponent.IsMaster() && !forceClient ) return;
		NO_SCR_EditorTask ParentTask = NO_SCR_EditorTask.Cast(this);
		if(!ParentTask) Debug.Error("NO_SCR_EditorTask cannot hook to the Editor Task so its not a child of it!");
		
		if(!game) return;
		
		PlayerManager playerManager = game.GetPlayerManager();
		
		
		array<int> players = {};
		playerManager.GetAllPlayers(players);
		
		NO_SCR_TaskManager manager = NO_SCR_TaskManager.Cast(GetTaskManager());
		if(m_tTriggerType==TriggerType.Assign)
		{
			
			if(!TaskState || TaskState!=TriggerType.Create) 
			{
				ChangeStateOfTask(TriggerType.Create,forceClient);
				GetGame().GetCallqueue().CallLater(ChangeStateOfTask, 1, false, TriggerType.Assign,forceClient);
				return;
			}
				
			
			NO_SCR_EditorTask unassignTask;
			TaskState = TriggerType.Assign;
			foreach(NO_SCR_EditorTask task : manager.m_aTasks)
			{
				if(task!=this && task.TaskState && task.TaskState == TriggerType.Assign)
				{
					task.TaskState = TriggerType.Create;
					unassignTask=task;
				}
			}
		
			
			Print("changed task "+TaskState.ToString());
			
			foreach(int playerId  : players)
			{
				auto taskExecutor = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerId);
				if(unassignTask)
				{
					manager.UnassignTask(unassignTask,taskExecutor,manager.m_bShowGMMessageWhenAssigningTasks);
				}
				manager.AssignTask(ParentTask,taskExecutor,manager.m_bShowGMMessageWhenAssigningTasks);
			}
			
			s_OnTaskAssign.Invoke();
			
		}
		else if(m_tTriggerType==TriggerType.Fail)
		{
			
			TaskState = TriggerType.Fail;
			Print("changed task "+TaskState.ToString());
			if(!m_bFailOrCompleteTemporarly)
			{
				manager.FailTask(ParentTask);
			}
			else{
				foreach(int playerId  : players)
				{
					auto taskExecutor = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerId);
					manager.UnassignTask(this,taskExecutor,manager.m_bShowGMMessageWhenAssigningTasks);
				}
			
				
				MakePopUpEveryWhere("Failed: "+m_sName);
				manager.SetTaskFaction(this,GetGame().GetFactionManager().GetFactionByKey(realManager.m_Dummyfaction));
			}
			GameOverLose();
			CreateNewTasksLose();
			s_OnTaskFail.Invoke();
			manager.m_aTasks.RemoveItem(ParentTask);
		}
		else if(m_tTriggerType==TriggerType.Finish)
		{
			TaskState = TriggerType.Finish;
			Print("changed task "+TaskState.ToString());
			
			if(!m_bFailOrCompleteTemporarly)
			{
				manager.FinishTask(ParentTask);
			}
			else{
				foreach(int playerId  : players)
				{
					SCR_BaseTaskExecutor taskExecutor = SCR_BaseTaskExecutor.GetTaskExecutorByID(playerId);
					manager.UnassignTask(this,taskExecutor,manager.m_bShowGMMessageWhenAssigningTasks);
				}
				MakePopUpEveryWhere("Completed: "+m_sName);
				manager.SetTaskFaction(this,GetGame().GetFactionManager().GetFactionByKey(realManager.m_Dummyfaction));
			}
			GameOverWin();
			CreateNewTasksWin();
			s_OnTaskComplete.Invoke();
			manager.m_aTasks.RemoveItem(ParentTask);
		}
		else if(m_tTriggerType==TriggerType.Create)
		{
			TaskState = TriggerType.Create;
			Print("changed task "+TaskState.ToString());
			manager.SetTaskFaction(ParentTask,GetGame().GetFactionManager().GetFactionByKey(ParentTask.m_faction));
			s_OnTaskCreate.Invoke();
		}
		
		Replication.BumpMe();
		
	}
	
	protected void DoMessageLocal(string message)
	{
		ShowPopUpNotification(message);
	}
	
	//------------------------------------------------------------------------------------------------
	// show notification every where
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDoMessage(string message)
	{
		DoMessageLocal(message);
	}
	
	void MakePopUpEveryWhere(string message)
	{
		if(!m_pRplComponent.IsMaster()) return;
		Rpc(RpcDoMessage, message);
		ShowPopUpNotification(message);
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
		if (!m_bEnableGameOverOnSuccess)
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
		if (!m_bEnableGameOverOnFail)
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
            if (m_bAssignFirstTask && !alreadyAssigned)
             {
              newtaskObject.ChangeStateOfTask(TriggerType.Assign);
              alreadyAssigned = true;
              continue;
             };  
            newtaskObject.ChangeStateOfTask(TriggerType.Create);   
        }
    }
	protected void CreateNewTasksLose()
    {
        alreadyAssigned = false;
        foreach(string task : m_sCreateTaskNamesFail)
        {
            IEntity newtaskEntity = world.FindEntityByName(task);
            NO_SCR_EditorTask newtaskObject = NO_SCR_EditorTask.Cast(newtaskEntity);  
            if (m_bAssignFirstTask && !alreadyAssigned)
             {
              newtaskObject.ChangeStateOfTask(TriggerType.Assign);
              alreadyAssigned = true;
              continue;
             };  
            newtaskObject.ChangeStateOfTask(TriggerType.Create);   
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