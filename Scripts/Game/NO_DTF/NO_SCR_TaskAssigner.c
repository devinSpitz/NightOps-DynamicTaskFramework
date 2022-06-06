[EntityEditorProps(category: "GameScripted/Coop", description: "Allows task to get assigned")]
class NO_SCR_TaskAssignerClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class NO_SCR_TaskAssigner : ScriptComponent
{
	
	[Attribute("", UIWidgets.Auto, "Which task to assign", category: "TaskManager:")]
	int m_iTaskIDFramework;	
	
	[Attribute("USSR", UIWidgets.EditBox, "Faction key to assign to tasks.", category: "TaskManager:")]
	protected FactionKey m_sAssignedFaction;
	
	private RplComponent m_pRplComponent;
	private IEntity Owner;
	private NO_SCR_TaskManager manager;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		Owner = owner;
		
		if(!m_pRplComponent)	
			m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
			
	}
	
	void AssignTask()
	{
		if(!m_pRplComponent)	
			m_pRplComponent = RplComponent.Cast(Owner.FindComponent(RplComponent));
		
		if(!m_pRplComponent) Debug.Error("NO_SCR_TaskAssigner cannot hook to the RplComponent please add one!");
		//Check for Master
		if(!m_pRplComponent.IsMaster()) return;
				
		//Get task manager
		
		if(!manager)	
		{
			SCR_BaseTaskManager taskmanager = GetTaskManager();
			manager = NO_SCR_TaskManager.Cast(taskmanager);
		}
			
		
		//NO_SCR_EditorTask task = manager.GetTaskById(m_iTaskIDFramework);
		
		
		//Faction targetFaction = GetGame().GetFactionManager().GetFactionByKey(m_sAssignedFaction);
		//manager.SetTaskFaction(task,targetFaction);
		
		//Assign task to all players
	}

}
