[EntityEditorProps(category: "GameScripted/Coop", description: "Allows task management.")]
class NO_SCR_TaskManagerClass : SCR_BaseTaskManagerClass
{
}

//------------------------------------------------------------------------------------------------
class NO_SCR_TaskManager : SCR_BaseTaskManager
{
	private ref array<IEntity> m_aChildren = new array<IEntity>();;
	private ref array<NO_SCR_EditorTask> m_aTasks = new array<NO_SCR_EditorTask>();
	
	protected override void EOnInit(IEntity owner)
	{
		
		super.EOnInit(owner);
		if (!GetGame().InPlayMode()) return;
		NO_SCR_DtfStatics.GetAllChildren(owner,m_aChildren);
		foreach(IEntity child : m_aChildren)
		{
			NO_SCR_EditorTask cast = NO_SCR_EditorTask.Cast(child);
			if(cast)
			{
				m_aTasks.Insert(cast);
				
			}
		}
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
		//if (m_pRplComponent.IsMaster())
		//	SetInitialTasksFaction();
	}
	
	
}
