[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class NO_SCR_FactionControlTriggerClass: SCR_FactionControlTriggerEntityClass
{
};

class NO_SCR_FactionControlTrigger: SCR_FactionControlTriggerEntity
{
	
	//Action
	[Attribute("1", UIWidgets.ComboBox, "What should that trigger do to its parent Task?","", ParamEnumArray.FromEnum(TriggerType), category: "TaskManager:" )]
	protected TriggerType m_tTriggerType;	
	
	IEntity Owner;
    override void OnInit(IEntity owner)
    {
	    Owner = owner;
	    super.OnInit(owner);
    }

    override void OnActivate(IEntity ent)
    {
        //Finish Task 1
        IEntity taskEntity = GetParent();
        NO_SCR_EditorTask task = NO_SCR_EditorTask.Cast(taskEntity);
        task.ChangeStateOfTask(m_tTriggerType);

        
    }
};