[EntityEditorProps(category: "GameScripted/Tasks", description: "Move task.", color: "0 0 255 255")]
class NO_SCR_EditorTaskClass: SCR_EditorTaskClass
{
};

//------------------------------------------------------------------------------------------------
class NO_SCR_EditorTask : SCR_EditorTask
{	

	[Attribute("USSR", UIWidgets.EditBox, "Faction is to ask whitch fraction can activeate the trigger", category: "TaskManager:")]
	FactionKey m_faction;	
	
	void ChangeStateOfTask(TriggerType m_tTriggerType)
	{
		
		auto m_pRplComponent = RplComponent.Cast(this.FindComponent(RplComponent));
		if(!m_pRplComponent) Debug.Error("NO_SCR_EditorTask cannot hook to the RplComponent please add one!");
		
		
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
		}
		else if(m_tTriggerType==TriggerType.Fail)
		{
			manager.FailTask(ParentTask);
		}
		else if(m_tTriggerType==TriggerType.Finish)
		{
			manager.FinishTask(ParentTask);
		}
		else if(m_tTriggerType==TriggerType.Create)
		{
			manager.SetTaskFaction(ParentTask,game.GetFactionManager().GetFactionByKey(ParentTask.m_faction));
		}
	}
		
}