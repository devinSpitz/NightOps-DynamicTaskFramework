[EntityEditorProps(category: "GameScripted/Coop", description: "Allows taskto get triggered as finsihed")]
class NO_SCR_TaskFinisherClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
class NO_SCR_TaskFinisher : ScriptComponent
{
	
	[Attribute("", UIWidgets.Auto, "Which task to end", category: "TaskManager:")]
	int m_iTaskIDFramework;	

	
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

	
	void EndTask()
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
			
		//EndTask
		
		//NO_SCR_EditorTask task = manager.GetTaskById(m_iTaskIDFramework);
		//manager.FinishTask(task);
		
	}
}
