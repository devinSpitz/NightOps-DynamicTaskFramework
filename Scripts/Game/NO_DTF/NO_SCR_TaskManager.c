[EntityEditorProps(category: "GameScripted/Coop", description: "Allows task management.")]
class NO_SCR_TaskManagerClass : SCR_BaseTaskManagerClass
{
}

//------------------------------------------------------------------------------------------------
class NO_SCR_TaskManager : SCR_BaseTaskManager
{
			
	[Attribute("0", UIWidgets.CheckBox, "Show that the GM has assigned the Tasks!", category: "TaskManager:")]
	bool m_bShowGMMessageWhenAssigningTasks;	
	
	
	
	ref array<NO_SCR_EditorTask> m_aTasks = new array<NO_SCR_EditorTask>();
	protected override void EOnInit(IEntity owner)
	{
		
		super.EOnInit(owner);
		if (!GetGame().InPlayMode()) return;
	}
	
		
 	////////////////////////////////////////////////////////////////
	///// CALLED ON EVERY MACHINE FOR EVERY PLAYER REGISTERED //////
	// INCLUDING WHEN JIP, PREVIOUS REGISTERED PLAYERS ARE CALLED //
	////////////////////////////////////////////////////////////////
	protected override void OnPlayerRegistered(int registeredPlayerID)
	{
		super.OnPlayerRegistered(registeredPlayerID);

		// Reset faction on initial tasks for JIP (Authority only)
		//ToDO Implement cache and rplProp
		
		if (m_RplComponent.IsMaster())
		{
			foreach(NO_SCR_EditorTask task : m_aTasks)
			{
				task.SetJIPState();
			}
		}
	}
	
	
}
