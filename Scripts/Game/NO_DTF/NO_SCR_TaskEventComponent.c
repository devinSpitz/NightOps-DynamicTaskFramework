
[EntityEditorProps(category: "GameScripted/Triggers", description: "Gives you events for the Task")]
class NO_SCR_TaskEventHandlerClass : GenericEntityClass
{
};



class NO_SCR_TaskEventHandler : GenericEntity
{
	
	event protected void OnFinishTask();
	//Does not work on initialize task
	event protected void OnAssignTask();
	//Does not work on initialize task
	event protected void OnCreateTask();
	event protected void OnFailTask();

	private RplComponent m_pRplComponent;
	
	
	void NO_SCR_TaskEventHandler(IEntitySource src, IEntity parent)
	{
	    SetEventMask(EntityEvent.INIT | EntityEvent.FRAME);
	    SetFlags(EntityFlags.ACTIVE, true);
	}

	override void EOnInit(IEntity owner)
	{
		
		if (!GetGame().InPlayMode()) return;
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if(!m_pRplComponent) Debug.Error("NO_SCR_TaskEventHandler cannot hook to the RplComponent please add one!");
		
		if(!m_pRplComponent.IsMaster()) return;
		NO_SCR_EditorTask task = NO_SCR_EditorTask.Cast(owner.GetParent());
		if(!task) 
		{
			Debug.Error("NO_SCR_TaskEventHandler does need a NO_SCR_EditorTask as base!");
		}
		Print("Inserting now");
		if(task.m_bAssignToFactionOnStart)
		{
			Print("Dont use the OnCreateTask or OnAssignTask invokers on initial assigned tasks!",LogLevel.WARNING);
		}
		task.s_OnTaskComplete.Insert(OnFinishTask);
		task.s_OnTaskFail.Insert(OnFailTask);
		task.s_OnTaskAssign.Insert(OnAssignTask);
		task.s_OnTaskCreate.Insert(OnCreateTask);
		
	}
	
	
	
}