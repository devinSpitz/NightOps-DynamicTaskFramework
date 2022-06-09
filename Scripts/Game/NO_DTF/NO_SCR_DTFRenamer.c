
[ComponentEditorProps(category: "GameScripted/Coop", description: "Allows spawning of AI groups.")]
class NO_SCR_DTFRenamerComponentClass : ScriptComponentClass
{
}


//------------------------------------------------------------------------------------------------
class NO_SCR_DTFRenamerComponent : ScriptComponent
{
	
	[Attribute("1", UIWidgets.EditBox, "Which name should be set at EOnInit?", category: "TaskManager:" )]
	string m_sNewName;
	
	RplComponent m_pRplComponent;
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		 m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));

		SetEventMask(owner, EntityEvent.INIT);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		if(m_pRplComponent.IsMaster())
			owner.SetName(m_sNewName);
	}
}