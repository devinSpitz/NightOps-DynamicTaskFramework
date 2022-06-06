[EntityEditorProps(category: "GameScripted/Tasks", description: "Move task.", color: "0 0 255 255")]
class NO_SCR_EditorTaskClass: SCR_EditorTaskClass
{
};

//------------------------------------------------------------------------------------------------
class NO_SCR_EditorTask : SCR_EditorTask
{	

	[Attribute("USSR", UIWidgets.EditBox, "Faction is to ask whitch fraction can activeate the trigger", category: "TaskManager:")]
	FactionKey m_faction;	
	
		
}