[EntityEditorProps(category: "GameScripted/Triggers", description: "Trigger that can aissgn, fail or finish task on complation of other tasks")]
class NO_SCR_MultiTaskTriggerComponentClass: ScriptComponentClass
{
};


class NO_SCR_MultiTaskTriggerComponent : ScriptComponent
{
	

	//Action
	[Attribute("1", UIWidgets.ComboBox, "What should that trigger do to its parent Task?","", ParamEnumArray.FromEnum(TriggerType), category: "TaskManager:" )]
	protected TriggerType m_tTriggerType;	
	
	//Slider
	[Attribute("3", UIWidgets.Slider, "How many seconds between each call should be waited?", "0 240 1", category: "TaskManager:" )]
	protected int m_iSecondsInBetweenTriggerChecks;	
	
	[Attribute("", UIWidgets.EditBox, "The object name that you want to trigger this action?", category: "TaskManager:" )]
	ref array<string> m_sTaskNames;
	
	[Attribute("0", UIWidgets.CheckBox, "Not only allow completed task to trigger but also failed", category: "TaskManager:")]
	bool m_allowFailedTasks;	
	
	
	private RplComponent m_pRplComponent;
	private IEntity Owner;
	private NO_SCR_EditorTask OwnTask;
	
	private bool alreadyTriggered = false;
	private NO_SCR_TaskManager manager;
	
	ArmaReforgerScripted game;
	BaseWorld world;
	
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		if (!GetGame().InPlayMode()) return;
		SetEventMask(owner, EntityEvent.INIT);
	}

	
	void FrequentlyCheckTrigger()
	{
		Do();
	}
	
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		Owner = owner;
		if (!GetGame().InPlayMode()) return;
		
		OwnTask = NO_SCR_EditorTask.Cast(owner);
		if(!OwnTask) Debug.Error("NO_SCR_TaskTrigger cannot hook to the Editor Task so not part of one!");
		
		
		m_pRplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		if(!m_pRplComponent) Debug.Error("NO_SCR_TaskTrigger cannot hook to the RplComponent please add one!");
		
		
		if(m_pRplComponent.IsMaster())
		{
			SCR_BaseTaskManager taskmanager = GetTaskManager();
			manager = NO_SCR_TaskManager.Cast(taskmanager);
			
			if(!manager)
			{
				Debug.Error("NO_SCR_TaskTrigger cannot hook on the TaskManager!");	
				return;
			} 
			
			
			game = GetGame();
			if(!game)
			{
				Debug.Error("NO_SCR_TaskTrigger cannot hook on the Game!");	
				return;
			} 
			
			world = game.GetWorld();
			if(!world)
			{
				Debug.Error("NO_SCR_TaskTrigger cannot hook on the World!");	
				return;
			} 
			
			GetGame().GetCallqueue().CallLater(FrequentlyCheckTrigger, m_iSecondsInBetweenTriggerChecks, true);	
		}
		
		
		
	}
	
	
	
	//return as soon as its not valide anymore
	void Do()
	{
		if(alreadyTriggered) return;
		if(!m_pRplComponent.IsMaster()) return;
		

		
		
		foreach(string task : m_sTaskNames)
		{
			IEntity taskEntity = world.FindEntityByName(task);
			NO_SCR_EditorTask taskObject = NO_SCR_EditorTask.Cast(taskEntity);
			
			if(taskObject &&taskObject.IsBadTask(m_allowFailedTasks)) return;
				
		}
		
		foreach(NO_SCR_EditorTask task: OwnTask.m_aChildren)
		{
			
			if(task && task.IsBadTask(m_allowFailedTasks)) return;
		}
		


		OwnTask.ChangeStateOfTask(m_tTriggerType);
		alreadyTriggered = true;
		
	}
	
	
}