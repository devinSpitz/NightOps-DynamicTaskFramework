[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class NO_SCR_FactionControlTriggerClass: SCR_FactionControlTriggerEntityClass
{
};

class NO_SCR_FactionControlTrigger: SCR_FactionControlTriggerEntity
{
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
        task.ChangeStateOfTask(TriggerType.Finish);

        
    }
};