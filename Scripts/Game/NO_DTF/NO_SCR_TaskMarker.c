[EntityEditorProps(category: "GameScripted/Biscuits", description: "COOP Friendly Task Manager Waypoints.")]
class NO_SCR_TaskMarkerComponentClass : ScriptComponentClass
{
}

class NO_SCR_TaskMarkerComponent : ScriptComponent
{
	[Attribute("1.0", UIWidgets.Range, desc: "Height above the task location the waypoint should appear.")]
	protected float m_fWaypointHeight;
	[Attribute("0.898 0.9541 0.184 1", UIWidgets.ColorPicker)]
	protected ref Color m_Color;
	

	protected ResourceName m_sBuildingIconImageset = "{F7E8D4834A3AFF2F}UI/Imagesets/Conflict/conflict-icons-bw.imageset";
	protected ImageWidget m_wWaypoint;
	protected RichTextWidget m_wWaypointDistance;

	protected NO_SCR_EditorTask m_pCurrentWaypoint;
	protected bool m_bShowWaypoint = false;
	protected bool m_bHasInitilised = false;
	private NO_SCR_TaskManager taskManager;
	private ArmaReforgerScripted game;
	private BaseWorld world;
	private WorkspaceWidget workspace;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
	}

	override void EOnInit(IEntity owner)
	{
		if (!GetGame().InPlayMode())
			return;

		if (RplSession.Mode() == RplMode.Dedicated)
			return;

		taskManager = NO_SCR_TaskManager.Cast(GetTaskManager());
		
		if(!taskManager) Debug.Error("NO_SCR_TaskMarkerComponent could get the Taskmanager!");
		taskManager.s_OnTaskAssigned.Insert(TaskAssigned);
		taskManager.s_OnTaskUnassigned.Insert(TaskUnassigned);

		
		
		game = GetGame();
		if(!game) Debug.Error("NO_SCR_TaskMarkerComponent could get the game!");
		
		world = game.GetWorld();
		if(!world) Debug.Error("NO_SCR_TaskMarkerComponent could get the world!");
		
		
		workspace = game.GetWorkspace();
		if(!workspace) Debug.Error("NO_SCR_TaskMarkerComponent could get the GetWorkspace!");
		
		
		game.GetCallqueue().CallLater(WaypointSystemInit, 2500, false);
	}

	protected void TaskAssigned()
	{
		SCR_BaseTask locallyAssignedTask = SCR_BaseTaskExecutor.GetLocalExecutor().GetAssignedTask();

		if (locallyAssignedTask)
		{
			
			Print("locallyAssignedTask");
			m_pCurrentWaypoint = NO_SCR_EditorTask.Cast(locallyAssignedTask);
			m_bShowWaypoint = true;
		}
	}

	protected void TaskUnassigned()
	{
		m_bShowWaypoint = false;
		m_pCurrentWaypoint = null;
	}

	protected void WaypointSystemInit()
	{
		Widget waypointFrame = game.GetHUDManager().CreateLayout("{4FEF75768BFDA310}UI/layouts/Tutorial/TutorialWaypoint.layout", EHudLayers.BACKGROUND);
		m_wWaypoint = ImageWidget.Cast(waypointFrame.FindAnyWidget("Icon"));
		m_wWaypointDistance = RichTextWidget.Cast(waypointFrame.FindAnyWidget("Distance"));

		if (m_wWaypoint)
		{
			m_wWaypoint.SetOpacity(0);
			m_wWaypoint.LoadImageFromSet(0, m_sBuildingIconImageset, "USSR_Base_Main_Select");
			m_wWaypoint.SetColor(m_Color);
			FrameSlot.SetSize(m_wWaypoint, 64, 64);
		}

		if (m_wWaypointDistance)
		{
			m_wWaypointDistance.SetOpacity(0);
			m_wWaypointDistance.SetColor(m_Color);
		}

		m_bHasInitilised = true;
	}

	override void EOnFrame(IEntity owner, float timeSlice)
	{
		
		if (m_wWaypoint && m_bHasInitilised)
		{
		
			if (m_pCurrentWaypoint && m_bShowWaypoint && m_pCurrentWaypoint.m_bActivateTaskMarkerForThisTask)
			{
				IEntity player = EntityUtils.GetPlayer();
				if (!player) return;

				
				vector WPPos = m_pCurrentWaypoint.GetOrigin();
				WPPos[1] = WPPos[1] + m_fWaypointHeight;
				
				if(m_pCurrentWaypoint.m_sAdditionalMarkerPosition != "" )
				{
					WPPos = world.FindEntityByName(m_pCurrentWaypoint.m_sAdditionalMarkerPosition).GetOrigin();
				}
				
				vector pos = workspace.ProjWorldToScreen(WPPos, world);
				int dist = vector.Distance(player.GetOrigin(), WPPos);

				// Handle off-screen coords
				int winX = workspace.GetWidth();
				int winY = workspace.GetHeight();
				int posX = workspace.DPIScale(pos[0]);
				int posY = workspace.DPIScale(pos[1]);

				if (posX < 0)
					pos[0] = 0;
				else if (posX > winX)
					pos[0] = workspace.DPIUnscale(winX);

				if (posY < 0)
					pos[1] = 0;
				else if (posY > winY || pos[2] < 0)
					pos[1] = workspace.DPIUnscale(winY);

				FrameSlot.SetPos(m_wWaypoint, pos[0], pos[1]);
				FrameSlot.SetPos(m_wWaypointDistance, pos[0], pos[1]);
				m_wWaypoint.SetOpacity(1);
				m_wWaypointDistance.SetTextFormat("#AR-Tutorial_WaypointUnits_meters", dist);
				m_wWaypointDistance.SetOpacity(1);
			}
			else
			{
				m_wWaypoint.SetOpacity(0);
				m_wWaypointDistance.SetOpacity(0);
			}
		}
	}
}