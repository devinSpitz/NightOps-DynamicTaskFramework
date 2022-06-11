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
	
	TriggerType TaskState = null;
	RplComponent m_pRplComponent;
	IEntity Owner;
	
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
		
		
		ArmaReforgerScripted game = GetGame();
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
		}
		else if(m_tTriggerType==TriggerType.Finish)
		{
			manager.FinishTask(ParentTask);
			TaskState = TriggerType.Finish;
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
		
}