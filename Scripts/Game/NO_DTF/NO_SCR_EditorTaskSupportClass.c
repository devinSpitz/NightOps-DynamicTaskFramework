[BaseContainerProps()]
class NO_SCR_EditorTaskSupportClass : SCR_BaseTaskSupportClass
{
    //------------------------------------------------------------------------------------------------
    override SCR_BaseTaskData CreateTaskData()
    {
        return new SCR_BaseTaskData();
    }
    
    //------------------------------------------------------------------------------------------------
    void NO_SCR_EditorTaskSupportClass()
    {
        m_TypeName = NO_SCR_EditorTask;
    }
};