class NO_SCR_DtfStatics
{
	//Thanks to narcoleptic marshmallow for his message on the arma discord: https://discord.com/channels/105462288051380224/976155351999201390/978395568453865622 
	static void GetAllChildren(IEntity parent, notnull inout array<IEntity> allChildren)
    {
		
		if (GetGame().InPlayMode()) return;
        if (!parent)
            return;
        
        IEntity child = parent.GetChildren();
        
        while (child)
        {
            allChildren.Insert(child);
            child = child.GetSibling();
        }
    }

	
}

enum WhenTypeTriggerNamed
{
	Appear,
	DisappearOrDie,
};

enum WhenTypeTrigger
{
	All,
	One,
	Half,
	MoreThanHalf,
	LessThanHalf,
	AnyChimera,
	NamedObject
};
enum TriggerType
{
	Assign = 1,
	Fail = 2,
	Finish = 3,
	Create = 4
};
enum ActivateType
{
	Enter,
	Leave
};
enum WhenType
{
	All,
	One,
	Half,
	MoreThanHalf,
	LessThanHalf
};

enum TaskType
{
	Faction,
	PlayerSpecific
};